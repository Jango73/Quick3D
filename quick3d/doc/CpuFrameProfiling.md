# CPU Frame Profiling Report

Conditions: release build, display `:1`, hardware GL (Intel UHD Graphics 630, Mesa, glsl4 shaders), scene `Quick3DTest.xml` (266 meshes, 386686 polygons, 314 chunks, 2655 components, 643 frustum tests per frame), runs of 600 frames through `Quick3DTest --check-render`. Numbers come from `CTimeSampler` dumps in `build-release/quick3dtest/Logs/Quick3DTest.log`.

## Instrumentation

- `C3DScene::updateScene`: `Update_Controller`, `Update_VehiclePhysics`, `Update_Components`, `Update_PostUpdate`, `Update_Collisions`.
- `CCamera::render`: `Camera_ShadowPass`, `Camera_UpdateObjects`, `Camera_SetupLights`, `Camera_LayerPasses`, plus `Camera_Layer_<Far|Close>` per render layer.
- `C3DScene::paintComponentsTimed`: per-class `Paint_<ClassName>` samples, grouped locally per class and flushed once per class, so the sampler mutex is never held inside the component loop.
- `Quick3DTest::onTimer`: `Frame_SetText` around `debugInfo()` + `setText()`.
- `CTimeSampler::addSampleTime()`: accumulates an externally measured duration, for groupings that cannot use scoped samples.

## Results per run

| Run | FPS | PaintGL | Layer Far | Layer Close | Paint_CWorldTerrain | UpdateScene | SetText |
|---|---|---|---|---|---|---|---|
| Baseline, swap interval 1 | 32 | 11.77 | 10.67 (passes total) | - | - | 1.68 | - |
| Instrumented, swap 1 | 26 | 14.18 | 6.48 | 6.61 | 11.53 | 2.03 | 0.148 |
| `vblank_mode=0` (Mesa env) | 30 | 14.19 | 7.90 | 5.19 | 11.59 | 2.06 | 0.141 |
| `setSwapInterval(0)` (code, reverted after) | 30 | 12.88 | 7.04 | 4.90 | 10.64 | 1.71 | - |

Per-class paint detail (instrumented run, ms/frame): `CWorldTerrain` 11.53, `CTerrestrialVehicle` 0.50, `CSkyBox` 0.175, `CAircraft` 0.032, `CMan` 0.007, `CLight` 0.001. `run-tests`: 93/93 on every build.

## Findings

1. Terrain paint dominates measured CPU: `Paint_CWorldTerrain` is about 71% of measured frame CPU. All other update, physics, collision and light work totals about 1.3 ms/frame.
2. Both passes collect and paint the exact same 157 terrain chunks per frame (measured chunk-pointer overlap 157/157, stable across frames; per-pass counters: Far 157 chunks / 241949 polys / 166 meshes, Close 157 chunks / 144737 polys / 100 meshes, sums matching the scene totals 314 chunks / 386686 polys / 266 meshes). `CWorldTerrain::paint` → `paintRecurse` has no per-pass or per-distance partitioning: chunk selection is LOD-only and identical in both passes. The passes differ only at rasterization: each pass sets its own projection and frustum (`CCamera.cpp:442`) and `CMeshGeometry::paint` frustum-culls every geometry against the layer frustum, so out-of-range geometry skips the draw call. Every collected chunk still pays the full paint traversal twice per frame (material setup, water, bounded meshes, bushes). Restricting the terrain layer mask is NOT a valid fix and the earlier recommendation to do so was wrong: each pass needs the terrain (background beyond the close far plane in Far, foreground depth precision in Close); a single layer would clip the background or lose depth precision.
3. Vsync is exonerated. Neither `vblank_mode=0` nor `setSwapInterval(0)` lifts the frame rate above about 30 FPS, so the swap wait is not the wall.
4. About 18 ms/frame stays outside measured CPU (31-38 ms wall against 14-16 ms measured). `Frame_SetText` at 0.15 ms/frame is excluded by measurement. The remainder sits downstream of `paintGL`: GPU execution, driver Present/swap handling, `QGraphicsView` machinery and the event loop. Splitting GPU from driver wait requires GL timer queries, not done here.
5. Shadow pass costs zero in this configuration: `Camera_ShadowPass` never appears in dumps (shader quality 0.5 below the 0.90 gate).
6. PaintGL varies 11.8-14.2 ms/frame between identical runs; background worker activity is the likely source.

## Follow-ups

- Partition terrain chunk collection by layer depth range so each pass only traverses the chunks it rasterizes (Far skips chunks fully inside the close range, Close skips chunks fully outside it).
- Add GL timer queries around layer passes to split GPU execution from CPU submission inside `Camera_Layer_Far` / `Camera_Layer_Close`.
