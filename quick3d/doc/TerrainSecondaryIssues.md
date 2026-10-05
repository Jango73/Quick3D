# Terrain — secondary issues (same area, not the hole bug)

These are real defects found while investigating the terrain-hole bug. They are
kept separate because none of them can by itself produce a permanent missing
patch. Numbered for tracking.

## 1. `QSP<CWorldChunk>(this)` creates a second ownership chain

Location: `quick3d/source/mesh/CWorldChunk.cpp`, `CWorldChunk::work()` (~line 461):

```cpp
pGenerator->generate(QSP<CWorldChunk>(this));
```

`QSP` is `QExplicitlySharedDataPointer` (`quick3d/source/base/CQ3DConstants.h`)
and `CComponent` already inherits `QSharedData`. Wrapping the already-owned
`this` in a fresh `QSP` creates a second, independent shared-ownership chain
for the same object. When both chains release, the object is deleted twice
(double-delete / use-after-free risk). Timing-dependent, like the main bug,
which is why it is listed here — but it would crash or corrupt, not cleanly
skip a patch.

Fix direction: pass a `QSP` copied from the existing owner (e.g.
`QSP<CWorldChunk>(pChunk)` from the `QSP` already held by the caller) or change
`generate()` to take a raw pointer / reference.

## 2. Plain-`bool` worker flags shared between threads without synchronization

Locations:

- `quick3d/source/base/CWorker.h` — `m_bStarted`, `m_bFinished`,
  `m_bStopRequested`
- `quick3d/source/mesh/CWorldChunk.h` — `m_bOK`
- `quick3d/source/terrain/CTerrain.h` — `m_bOK`

These are written on worker threads (`CWorker::run()`, `CTerrain::work()`,
`CWorldChunk::work()`) and read on the render thread (`drawable()`,
`paintRecurse()`, `CWorkerManager::run()`) with no atomic / mutex protection.
A finished patch can therefore appear "not ready" longer than necessary
(stale read), extending holes but not creating permanent ones.

Fix direction: make the flags `std::atomic<bool>` (or `QAtomicInteger`) or
guard them with the existing mutexes.

## 3. `buildRecurse()` runs only every 6th frame

Location: `quick3d/source/mesh/CWorldTerrain.cpp`, `CWorldTerrain::paint()`
(~lines 259-270):

```cpp
iBuildCounter++;
if (iBuildCounter > 5)
{
    iBuildCounter = 0;
    buildRecurse(m_pRoot, pContext, m_iLevels);
}
```

Even in the healthy case, a newly needed patch waits several frames before its
build is even queued, so fast camera moves show transient holes. This is a
latency / responsiveness issue, not a "never appears" issue.

Fix direction: trigger a build pass as soon as `paintRecurse()` collects
nothing for a visible area, or lower the throttle when the camera moves fast.

## 4. `getFaceIndexForVertices()` key collisions (height queries only)

Location: `quick3d/source/terrain/CTerrain.cpp`,
`buildVerticesToFaceMap()` (~lines 596-617) and `getFaceIndexForVertices()`
(~lines 168-186):

```cpp
QString sKey = QString("%1%2%3%4").arg(i1).arg(i2).arg(i3).arg(i4);
```

With no separators, distinct index tuples collide (e.g. 1,23,4,5 vs 12,3,4,5),
so `getHeightAt()` can test the wrong face and return a wrong altitude. This
affects height queries only, not rendering — the mesh itself is drawn from the
face list, not from this map — so it cannot cause a visual hole.

Fix direction: join with a separator
(e.g. `QString("%1,%2,%3,%4")`) or key on a 64-bit packed integer tuple.
