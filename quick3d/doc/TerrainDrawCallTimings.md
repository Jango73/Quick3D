# Terrain draw-call timings, headed 2026-10-23 (avg ms/frame + calls)

## Dump 1 (409 frames, startup phase)

```
Camera_LayerPasses avg 13.070 ms/frame calls 409
Camera_Layer_Close avg 5.449 ms/frame calls 409
Camera_Layer_Far avg 7.478 ms/frame calls 409
Camera_Render avg 13.483 ms/frame calls 409
Camera_SetupLights avg 0.081 ms/frame calls 409
Camera_UpdateObjects avg 0.055 ms/frame calls 409
Chunk_Bounded avg 0.229 ms/frame calls 818
Chunk_Terrain avg 3.886 ms/frame calls 818
Chunk_Water avg 2.405 ms/frame calls 818
Frame_SetText avg 0.127 ms/frame calls 409
Material_Activate avg 4.530 ms/frame calls 818
Mesh_BindState avg 0.636 ms/frame calls 818
Mesh_Draw avg 0.505 ms/frame calls 818
PaintGL avg 13.593 ms/frame calls 409
Paint_CAircraft avg 0.016 ms/frame calls 818
Paint_CLight avg 0.001 ms/frame calls 818
Paint_CMan avg 0.006 ms/frame calls 818
Paint_CSkyBox avg 0.167 ms/frame calls 409
Paint_CTerrestrialVehicle avg 0.158 ms/frame calls 818
Paint_CWorldTerrain avg 11.172 ms/frame calls 818
Terrain_Collect avg 2.121 ms/frame calls 818
Terrain_DrawLoop avg 8.319 ms/frame calls 818
UpdateScene avg 2.866 ms/frame calls 409
Update_Collisions avg 0.092 ms/frame calls 409
Update_Components avg 1.479 ms/frame calls 409
Update_Controller avg 0.063 ms/frame calls 409
Update_PostUpdate avg 0.326 ms/frame calls 409
Update_VehiclePhysics avg 0.622 ms/frame calls 409
```

## Dump 2 (277 frames, steady state)

```
Camera_LayerPasses avg 21.883 ms/frame calls 277
Camera_Layer_Close avg 9.134 ms/frame calls 277
Camera_Layer_Far avg 12.614 ms/frame calls 277
Camera_Render avg 22.332 ms/frame calls 277
Camera_SetupLights avg 0.080 ms/frame calls 277
Camera_UpdateObjects avg 0.052 ms/frame calls 277
Chunk_Bounded avg 0.781 ms/frame calls 554
Chunk_Terrain avg 7.761 ms/frame calls 554
Chunk_Water avg 4.825 ms/frame calls 554
Frame_SetText avg 0.120 ms/frame calls 277
Material_Activate avg 8.847 ms/frame calls 554
Mesh_BindState avg 1.122 ms/frame calls 554
Mesh_Draw avg 0.955 ms/frame calls 554
PaintGL avg 22.438 ms/frame calls 277
Paint_CAircraft avg 0.015 ms/frame calls 554
Paint_CLight avg 0.001 ms/frame calls 554
Paint_CMan avg 0.006 ms/frame calls 554
Paint_CSkyBox avg 0.159 ms/frame calls 277
Paint_CTerrestrialVehicle avg 0.150 ms/frame calls 554
Paint_CWorldTerrain avg 20.050 ms/frame calls 554
Terrain_Collect avg 1.931 ms/frame calls 554
Terrain_DrawLoop avg 17.282 ms/frame calls 554
UpdateScene avg 3.387 ms/frame calls 277
Update_Collisions avg 0.089 ms/frame calls 277
Update_Components avg 1.617 ms/frame calls 277
Update_Controller avg 0.077 ms/frame calls 277
Update_PostUpdate avg 0.355 ms/frame calls 277
Update_VehiclePhysics avg 0.949 ms/frame calls 277
```

## Dump 3 (514 frames, post-fix steady state)

```
Camera_LayerPasses avg 12.774 ms/frame calls 513
Camera_Layer_Close avg 5.908 ms/frame calls 513
Camera_Layer_Far avg 6.736 ms/frame calls 513
Camera_Render avg 13.176 ms/frame calls 513
Camera_SetupLights avg 0.079 ms/frame calls 513
Camera_UpdateObjects avg 0.049 ms/frame calls 513
Chunk_Bounded avg 0.793 ms/frame calls 1026
Chunk_Terrain avg 2.057 ms/frame calls 1026
Chunk_Water avg 1.555 ms/frame calls 1026
Frame_SetText avg 0.100 ms/frame calls 514
Material_Activate avg 0.643 ms/frame calls 1026
Mesh_BindState avg 1.089 ms/frame calls 1026
Mesh_Draw avg 0.492 ms/frame calls 1026
PaintGL avg 13.271 ms/frame calls 513
Paint_CAircraft avg 0.017 ms/frame calls 1026
Paint_CLight avg 0.001 ms/frame calls 1026
Paint_CMan avg 0.006 ms/frame calls 1026
Paint_CSkyBox avg 0.143 ms/frame calls 513
Paint_CTerrestrialVehicle avg 0.146 ms/frame calls 1026
Paint_CWorldTerrain avg 10.983 ms/frame calls 1026
Terrain_Collect avg 1.903 ms/frame calls 1026
Terrain_DrawLoop avg 8.248 ms/frame calls 1026
UpdateScene avg 2.502 ms/frame calls 514
Update_Collisions avg 0.077 ms/frame calls 514
Update_Components avg 1.247 ms/frame calls 514
Update_Controller avg 0.059 ms/frame calls 514
Update_PostUpdate avg 0.292 ms/frame calls 514
Update_VehiclePhysics avg 0.566 ms/frame calls 514
```

## Analysis

The actual `glDrawElements` calls (`Mesh_Draw` = 0.955/22.438 ms/frame, ~4 %) are cheap: the time is CPU-side state setup, not the GPU draws. `Material_Activate` was 8.847/22.438 ms/frame (~39 %): program bind, ~30 `setUniformValue` calls, texture binds and `setupEnvironment` rerun per chunk per pass, with the terrain/water alternation defeating the identity guard and the tiled tail running unconditionally. Dump 2 (pre-fix, heading 60, 277 frames) vs Dump 3 (post-fix, heading 180, 513 frames): `Material_Activate` 8.847 -> 0.643, `PaintGL` 22.438 -> 13.271. The two dumps use different headings and frame windows and record no draw/chunk counts, so this is an indicative before/after, not a controlled A/B. Remaining: `Terrain_Collect` (1.6-1.9), `Mesh_BindState` (1.1), loop residual (~3.9).

## Fix summary

`CMaterial::activate()` is now a template method: single identity guard, then virtual `applyStaticState()` (uniforms, diffuse maps via virtual `bindDiffuseMaps()`, environment; runs only on material change) and virtual `applyDynamicState()` (per-draw hook, no-op in base). `CTiledMaterial` lost its `activate()` override and rebinds its tile in `applyDynamicState()` on every call (no per-material GL cache: the program and the texture units are shared with the bounded materials); its static diffuse binding is empty (the tile is the only diffuse source). `CWaterMaterial` sets its wave uniforms in `applyStaticState()`. `CWorldTerrain::paint()` paints an opaque phase (terrain + bounded) then a water phase, with a `boundsOnly()` branch that keeps the single `CWorldChunk::paint()` entry point for the debug view. `CTexture` sets sampling parameters once at upload; `activate()` only binds. Sampling itself is gated behind `--sample` (off by default) and accumulates in `tStatistics` with a once-per-pass flush, so production frames carry no sampling overhead. Files: `quick3d/doc/TerrainDrawCallTimings.md`, `quick3d/source/mesh/CMaterial.{h,cpp}`, `quick3d/source/mesh/CTexture.cpp`, `quick3d/source/mesh/CTiledMaterial.{h,cpp}`, `quick3d/source/mesh/CWaterMaterial.{h,cpp}`, `quick3d/source/mesh/CWorldChunk.{h,cpp}`, `quick3d/source/mesh/CWorldTerrain.cpp` (the `quick3dtest/bin/Quick3DTest.xml` Man1 heading hunk from this commit was reverted: a product fixture must not carry benchmark setup).
