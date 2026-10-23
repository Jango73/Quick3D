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

## Analysis

The actual `glDrawElements` calls (`Mesh_Draw` = 0.955/22.438 ms/frame, ~4 %) are cheap: the time is CPU-side state setup, not the GPU draws. `Material_Activate` = 8.847/22.438 ms/frame (~39 %) dominates: program bind, ~30 `setUniformValue` calls, texture binds and `setupEnvironment` rerun per chunk per pass. `Mesh_BindState` (1.122) and `Terrain_Collect` (1.931) are secondary. `Terrain_DrawLoop` (17.282) exceeds its timed parts (7.761 + 4.825 + 0.781 = 13.367), leaving ~3.9 ms/frame of loop overhead between the timers. Fix direction: cut per-chunk CPU state setup (redundant uniform updates, unsorted state changes), not the draw count.
