# Fill-rate benchmark: maximized window FPS drop

## Conditions

Hardware on this machine is Intel UHD Graphics 630 (Mesa 25.1.5), display 1920x1080, application window forced to 1920x935 at position 0,0 for every run. Scene is the default startup scene `Quick3DTest.xml` (223 meshes, 317194 polys, 220 chunks drawn, identical across all runs). Each run renders 1200 frames headful with vsync on (`setSwapInterval(1)`), which takes 36-42 s wall time, and the reported value is the rolling FPS average over the last 20 frames (`m_FPS(20)`, kept constant for all runs including baseline). Each variant removes exactly one feature from the pristine HEAD shaders; nothing is accumulated between variants.

## Results

| Run | Isolated change vs baseline | FPS maximized |
|---|---|---|
| V0 baseline | pristine HEAD shaders | 33 |
| V1 sky-flat | `standardSkyAt()` replaced by flat `u_sky_zenith_color` in `glsl1`/`glsl4` `FS_Standard.c` | 34 (+1) |
| V2 tex-single | `getTexture()` 8-texture mix replaced by single `u_texture_diffuse_0` fetch in `glsl1`/`glsl4` `FS_Standard.c` | 38 (+5) |
| V3 one-light | `doLighting()` loop capped to a single light (`index < 1`) in `glsl1`/`glsl4` `FS_Standard.c` | 40 (+7) |
| V4 lambert | `uberShader()` Oren-Nayar + GGX replaced by `max(dot(N,L),0) * diffuse` in `glsl1`/`glsl4` `FS_Standard.c` | 37 (+4) |
| V5 no-lightrays | `u_lightrays_enable` branch forced off in `glsl1`/`glsl4` `FS_Standard.c` | 33 (+0) |
| V6 no-fog | `u_fog_enable` branch forced off in `glsl1`/`glsl4` `FS_Standard.c` | 33 (+0) |

Reference point on the same machine: V0 baseline in the small startup window measured 52 FPS, maximized it drops to 33 FPS, which reproduces the reported small-vs-maximized collapse on this hardware too.

## Ranking and conclusion

Ordered by isolated maximized-window gain: light count (+7) first, then 8-texture mix removal (+5), then Oren-Nayar + GGX to Lambert simplification (+4). Sky flattening (+1), light-rays disabling (+0) and fog disabling (+0) show no measurable effect in the nominal startup case (shadows off, clouds already skipped below `u_shaderQuality 0.85`, fog `enabled=true` at `level=0.0`, no water visible, default quality 0.5). The drop from 52 FPS small to 33 FPS maximized on unmodified shaders confirms the bottleneck scales with covered pixel count: per-pixel lighting loop plus multi-texture fetches plus BRDF dominate frame time at 1920x935, while sky, light-rays and fog ALU are negligible there.
