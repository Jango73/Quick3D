# Terrain holes analysis — patches that never appear

Symptom: at runtime, some terrain patches are never rendered, leaving visible
holes in the terrain. The occurrence is random and hard to reproduce.

Files involved:

- `Quick3D/Source/Base/CWorkerManager.cpp` (scheduler)
- `Quick3D/Source/Base/CWorker.{h,cpp}`
- `Quick3D/Source/Mesh/CWorldChunk.cpp` (`work()`, `drawable()`, `paint()`)
- `Quick3D/Source/Mesh/CWorldTerrain.cpp` (`buildRecurse()`, `paintRecurse()`)
- `Quick3D/Source/Terrain/CTerrain.cpp` (`work()`)

## 1. Primary cause: worker-scheduler deadlock / starvation (random, can be permanent)

`CWorkerManager::run()` only ever services the first `MAX_WORKERS_RUNNING`
(3) slots of the queue (`CWorkerManager.cpp`, lines ~91-109):

- a worker at index >= 3 is never started while the first 3 slots are occupied;
- a finished worker at index >= 3 is never reaped either (reaping also only
  scans indices 0..2).

Meanwhile `CWorldChunk::work()` (`CWorldChunk.cpp`, lines ~406-414) blocks its
thread in a busy-wait loop until its own `CTerrain` reports `isOK()`:

```cpp
while (true)
{
    if (m_pTerrain == nullptr) return;
    if (m_bStopRequested) return;
    if (m_pTerrain->isOK() == true) break;
    msleep(50);
}
```

Creation order in `CWorldTerrain::buildRecurse()` guarantees the inversion:
`new CTerrain(...)` enqueues the terrain worker first, then
`pChunk->setTerrain(...)` enqueues the chunk worker right behind it
(`CWorldChunk::setTerrain()` only enqueues the chunk for `level() < 2`,
i.e. the biggest, most visible patches).

Deadlock scenario:

1. Chunk workers occupy the 3 serviced slots and wait for their terrains.
2. Their terrains sit at queue index >= 3 and are never started.
3. The waiting chunks never finish, the slots never free, every patch queued
   behind them starves.

Which patches get stuck depends entirely on thread timing and camera movement,
hence the randomness. While the slots are blocked, the whole terrain pipeline
stalls, so several patches can be missing at once. There is no self-healing
through garbage collection either: `CWorldTerrain::collectGarbageRecurse()`
only clears a chunk whose `isExpendable()` is true, and `CExpendable` requires
`isReallyExpendable()` — which for `CWorldChunk` means terrain present AND
`isOK()`. A chunk stuck waiting for a starved terrain is therefore never
collected, and its slot never frees. Recovery is only incidental (camera
movement flipping the LOD decision), so holes can look permanent.

Fix direction: never block a worker thread on another worker. E.g. enqueue the
chunk-detail work only after its terrain is done (completion callback /
dependency), or move the detail generation out of the worker pool. And fix the
scheduler to scan the whole queue (count running workers instead of pinning
service to the first N slots) and reap finished workers at any index.

## 2. Aggravating defect: `paintRecurse()` draws nothing when data is not ready

`CWorldTerrain::paintRecurse()` (`CWorldTerrain.cpp`, lines ~612-700):

```cpp
if (bStayHere)
{
    if (pChunk->terrain() && pChunk->terrain()->isOK())
    {
        // collect chunk for drawing
    }
    else if (bChildrenDrawable)
    {
        // recurse into children
    }
    // else: NOTHING is collected -> hole
}
```

When the LOD decision says "stay here" but the patch is still building
(`!isOK()`, e.g. because of the starvation above) and no drawable children
exist (none built yet), the function collects nothing for that area: a hole
is rendered instead of falling back to an already-available parent or child
patch. Note the asymmetry: the `else` branch (more detail wanted, children not
ready) falls back to drawing the parent chunk, but the `bStayHere` branch has
no equivalent fallback.

Fix direction: in the `bStayHere` branch, if the local terrain is not ready but
children are drawable, keep drawing the children (finer data is better than a
hole); if neither is drawable, draw the closest ready ancestor instead of
nothing.

## 3. Second mechanism: fresh child chunks deleted before their first build

`CWorldTerrain::buildRecurse()` runs only every 6th `paint()` call, but
`paintRecurse()` — including its "Get rid of empty nodes" cleanup
(`CWorldTerrain.cpp`, lines ~652-661) — runs every frame:

```cpp
// Get rid of empty nodes
for (int iIndex = 0; iIndex < pChunk->childComponents().count(); iIndex++)
{
    QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChunk->childComponents()[iIndex]);
    if (pChild != nullptr && pChild->isEmpty())
    {
        pChunk->childComponents().remove(iIndex);
        iIndex--;
    }
}
```

`CWorldChunk::isEmpty()` (`CWorldChunk.cpp`, lines ~316-333) returns true for a
chunk with no terrain that is not registered as a worker. That is exactly the
state of a freshly subdivided child: `buildRecurse()` creates children with
`build()` but no terrain, and only a later `buildRecurse()` pass assigns them
one. So during the up-to-5 paint passes between the two, whenever the parent
happens to be in the `bStayHere`-with-ready-terrain state (e.g. camera moving
away right after a subdivision), its brand-new children are classified "empty"
and deleted before they ever received a patch.

The damage is permanent: `buildRecurse()` only creates children when
`childComponents().count() == 0`, so a partially pruned sibling set is never
repaired — the deleted quadrant is never subdivided again. Coming back with
the camera, `paintRecurse()` recurses into the remaining children only, and the
missing quadrant stays a hole (or a coarse parent patch at best).

Two details make this timing-dependent, hence random:

- the 6:1 ratio between the build pass and the paint/cleanup pass;
- `CWorkerManager::containsWorker()` reads `m_vWorkers` with no mutex, so
  `isEmpty()` can misfire even in cases the locking discipline should protect.

Fix direction: never delete a child that was created but not yet visited by
`buildRecurse()` (e.g. timestamp/flag on creation, or only prune children that
already had a terrain once), and/or run the pruning in the build pass instead
of the paint pass.

## 4. Ruled out

- `CMeshGeometry::createQuadPatch()` topology: it emits consistent quad grids
  (`getPointIndexForXZ` math matches), so holes do not come from corrupt mesh
  indices.
- Missing height data (`CSRTM`/`HGT`/`BIL` I/O failures): `CTerrain::work()`
  coerces `Q3D_INFINITY` altitudes to `0.0`, producing flat sea-level patches,
  not holes.
- Frustum culling: the per-chunk frustum check in `CWorldChunk::paint()` is
  commented out (chunks are always painted once collected), so disappearing
  patches are a selection/build problem, not a culling one. The known frustum
  false-negative issue from the README ("Camera" section) is a separate bug.
