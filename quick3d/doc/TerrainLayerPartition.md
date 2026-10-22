# Terrain LOD and Layer Partition

## Passes

`CWorldTerrain::paint` drives two passes per frame: `buildRecurse` subdivides the
quadtree and ensures cover patches, `paintRecurse` collects one drawable chunk
per zone following `CWorldLodPolicy::decideDraw`. `collectGarbageRecurse` runs
bottom-up afterwards and applies `mayDestroySelf`, rebuilding the parent cover
through `ensureChunkCover` when children go away but the zone stays needed.

## Layer depth partition

`skipChunkForLayerPass` restricts each render layer to the chunks intersecting
its depth range: the far pass skips chunks fully inside the close range, the
close pass skips chunks fully outside it. The interval is the camera-space
forward depth of the chunk in the same frame the per-geometry draw test uses,
so collect and drawing share one language. Chunks spanning the close far plane
stay in both passes. The partition margin covering point-sprite bushes is
`LayerPartitionVegetationMargin`.

## LOD policy

`CWorldLodPolicy` owns every quadtree rule: `decideDraw`, `allChildrenReady`,
`hasPartialChildren`, `isProtectedLevel`, `mayDestroySelf`, `neededRange` and
`isNeeded`. Subdivision always creates `FullChildCount` children; a partial set
is repaired, never kept. The two coarsest levels are never destroyed and serve
as fallback cover. `NeededRangeFactor` is the camera distance in ground
diagonals under which a chunk stays needed, keeping nearby chunks behind the
camera alive for rotations.

## Ownership and threading

`CWorldChunk::childStatistics` counts children owner-side in a single walk and
replaces the former triplicated counting loops. `CWorldChunk::subtreeHasWorker`
gathers the subtree once and asks `CWorkerManager::containsAnyWorker` under a
single lock. Snapshot state (`m_mLastCollectedChunks`, `m_vLastCameraPath`,
`m_gLastCameraGeoloc`, camera layer frames) is written on the render thread
and read through mutex-guarded copy accessors. Mesh draw retention per layer
pass is bounded by `CCamera::MaxMeshDraws`; overflow sets the truncation flag
surfaced in the snapshot.

## Snapshots and diagnostics

Every `ISnapshotable` serializes its own state as a `CXMLNode`; repeated child
tags form the JSON arrays and the `ok` / `error` envelopes come from
`CRemoteControlServer` through `CXMLNode::toJsonString`. Scalar values are
stored as text, vector fields are flattened to scalar attributes. The engine snapshot
carries camera layer frames with per-geometry draw outcomes, terrain collect
identities and the camera-path readiness. `CLogFloodGate` bounds repetitive
debug output to a hit count per cooldown window.

## Translation units

- `CWorldTerrain.cpp`: lifecycle, paint, queries, physics queries, dump.
- `CWorldTerrainBuild.cpp`: subdivision, cover creation, LOD detail test.
- `CWorldTerrainGarbage.cpp`: bottom-up collection and subtree destruction.
- `CWorldTerrainPhysics.cpp`: height-field registration against the physics world.
- `CWorldTerrainSnapshot.cpp`: camera-path recording and snapshot documents.
- `CMeshGeometry.cpp` / `CMeshGeometryBuilders.cpp`: processing vs construction.
- `CCamera.cpp` / `CCameraDepth.cpp`: frame render vs depth pre-passes.

## Open profiling work

GL timer queries around the layer passes are still missing: they would split
GPU execution from CPU submission inside the far and close passes. See the
`Camera_Layer_<name>` time samples for the CPU side.
