# Water Waves Shared Definition

## Canonical form

`CWaterWaves` in `quick3d/source/utils/` owns the sea surface displaced by the
vertex shaders. `heightAt()` evaluates two opposed `CPerlin::turbulence()`
samples scaled by the wind driven amplitude, `surfaceAt()` adds the finite
difference normal forming the Jolt buoyancy plane. `amplitudeFromWindLevel()`
maps the scene wind level to the `u_wave_amplitude` uniform.

`CWaterMaterial::WaveHeight()` and `CWaterMaterial::activate()` delegate to
`CWaterWaves`, the material holds no wave math of its own. The GLSL
`waveHeight()` in `VS_Standard.c` (glsl1 and glsl4) mirrors the same constants:
`SpatialScale`, `TimeRate`, `SecondSampleOffsetM`, `HeightGain` and the Perlin
hash. Both shader files point back to `CWaterWaves.h`.

## Sync contract

The shader evaluates in the model matrix frame, which is world minus origin
(`CMeshGeometry.cpp`), so the CPU query subtracts `C3DScene::worldOrigin()`
before calling `heightAt()`. Time is `C3DScene::time()` on the CPU and
`u_time` on the GPU, both fed from the same scene clock. Amplitude is
`windLevel()` times `WindToAmplitudeGain` on both sides.

`CTestWaterWaves` locks the contract: calm sea flatness, wind linearity, time
variance, normal validity, material delegation against a live scene, shader
constant presence in both GLSL sources, and a numeric anchor against Perlin
drift. Any wave change updates `CWaterWaves`, both shaders, and the tests
together.

## Jolt buoyancy preparation

The sea never enters Jolt as a collision body. `CSeaVehicle` samples
`surfaceAt()` under the hull and feeds the resulting position and normal to
`Body::ApplyBuoyancyImpulse()`, following the `BoatTest` sample. The shared
definition guarantees the buoyancy plane matches the rendered surface.
