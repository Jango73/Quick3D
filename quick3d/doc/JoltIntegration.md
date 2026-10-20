# Jolt Integration

Physics backend for collision queries, height fields, meshes and wheeled
vehicles. Flight force generation stays in `CPhysicalComponent`.

## Sourcing

- `third/JoltPhysics`, git submodule pinned to tag `v5.6.0`.
- Static library linked `PRIVATE` into `Quick3D`; objects built with
  `POSITION_INDEPENDENT_CODE` for shared library linking.
- `DOUBLE_PRECISION` off: geo coordinates convert to local single precision
  frames at the mapping layer, never inside the wrapper.
- CMake overrides at inclusion: `INTERPROCEDURAL_OPTIMIZATION OFF` (Jolt
  pushes LTO into the including scope), `ENABLE_INSTALL OFF`,
  `ENABLE_ALL_WARNINGS OFF` (GCC 11 `-Werror=uninitialized` false positive
  in `HashTable.h` via `DebugRendererPlayback.cpp`).
- Jolt SIMD defaults kept (AVX2 baseline); target CPUs require AVX2.

## Wrapper

- `quick3d/source/physics/CPhysicsWorld.h/.cpp`: sole owner of the Jolt
  world. Header carries no Jolt include, only forward declarations, and
  stays safe for downstream applications.
- `quick3d/source/physics/CPhysicsMath.h`: engine to Jolt vector
  conversions. Internal header, engine sources only.
- Layer map: `NON_MOVING` (static) and `MOVING` (dynamic); static bodies
  never collide with each other.
- The job pool (`JobSystemThreadPool`, max 8 threads) owns its threads and
  is not bridged to `CWorkerManager`.
- Jolt trace and assert handlers route to `CLogger`.
- Process global Jolt registry (`RegisterDefaultAllocator`, `RegisterTypes`)
  runs once under a mutex with an instance counter in `CPhysicsWorld.cpp`.
  Jolt owns this global state by design; the counter is the smallest owner
  side guard for sequential world instances.
- One world instance is driven by one thread; the caller serializes steps.

## Coverage

- `quick3dtest/tests/CTestPhysics`: placement round trip through the float
  conversion, sphere drop onto a static floor, unknown body index failure,
  chassis drop settling inside suspension travel with all wheels in contact,
  full throttle drive along chassis local +Z, unknown vehicle and wheel
  index failure.
- Full headless suite: `Quick3DTest run-tests` with `QT_QPA_PLATFORM=offscreen`.

## Vehicles

- `CPhysicsWorld::addVehicle` builds a dynamic chassis box with a lowered
  center of mass and one `VehicleConstraint` per vehicle.
- `CVehicleWheel` maps to one `WheelSettingsWV`: attachment position,
  radius, width, suspension travel, spring frequency and damping, constant
  longitudinal and lateral friction levels, maximum steer angle, driven and
  hand brake flags. Vehicle forward is chassis local +Z. Attachment positions
  are expressed in the body center frame: authored origin frame positions
  minus the rotated center offset used for the body spawn.
- Wheels sharing a longitudinal station form an axle, each axle outer pair
  gets a `VehicleAntiRollBar` with the Jolt sample 1000 N/m stiffness.
- Driven wheels pair into differentials in wheel order with engine torque
  split equally; an odd driven count is rejected. Engine, brake and hand
  brake torques scale with chassis mass from the Jolt 1500 kg reference car
  (250 Nm engine, 1500 Nm brakes, 4000 Nm hand brake), engine torque also
  compensates the largest tire radius from the 0.3 m reference wheel so
  gradeability stays constant across tire sizes. Clutch strength and engine
  inertia scale with engine torque from the Jolt stock 500 Nm, 10.0 and
  0.5 references: a stock clutch slips under heavy engines and caps grade
  traction below demand, a stock inertia revs to the limiter while the
  wheels stand still. The longitudinal tire curve carries full grip at zero
  slip: a zero intercept only bites after spinning up, which the gearbox
  reads as speed and answers with an upshift and a torque cut, stalling
  slope starts and crawls. Height field bodies carry neutral 1.0 friction:
  the Jolt 0.2 default dilutes the authored tire grip through the geometric
  mean combine and halves the traction cap on every terrain. Vehicles over
  3 tons get a truck gearbox (5.2 to 0.85, short first gear for grades),
  lighter vehicles keep the Jolt car ratios. Top speed is capped by the
  `CForceController` speed limiter at the authored `SpeedMS`: throttle
  tapers to zero over the last 1 m/s before the cap and a staged brake
  (0.15 at the cap, 0.4 past cap + 0.5 m/s) holds the vehicle back, reverse
  stays available. Uphill starts keep full torque below the cap.
  `CTestPhysics::testHeavyVehicleClimbsSlope` pins a 26 ton chassis
  climbing a 30 percent grade from a hand brake held start.
- Wheels sharing a longitudinal station form an axle with the Jolt sample
 1000 N/m anti roll bar, stiffer bars couple the axles into a rocking beam
 on rough ground. Tire forces apply at the contact point (Jolt default),
 the fixed body force point stays disabled. A 60 degree pitch roll limit
 keeps the chassis off its roof on steps.
- `C3DScene::attachVehicleToPhysics` clamps the collision footprint to the
 wheel track plus 0.15 m: visual bounds overhang the wheels on armored
 hulls and a wide box strikes the ground on roll, kicking the chassis from
 side to side.
- Ownership: `ConstraintManager` stores constraints as `Ref<Constraint>`;
  `RemoveConstraint` releases the last reference and deletes the object,
  so the wrapper never manually deletes a constraint.
- Driver inputs (`setVehicleDriverInput`) reach `WheeledVehicleController`
  clamped to [-1, 1] for forward and right, [0, 1] for brakes.
- `vehicleWheelHasContact` exposes per wheel ground contact for the future
  `CWheel` mapping layer.
- `removeVehicle` unregisters the constraint and destroys the chassis body.

## Component mapping

- `CWheel` carries the mapping source: `Radius`, `Width`, `Driven`,
  `Steered`, `MaxSteerAngleDeg`, `SuspensionMinLength`,
  `SuspensionMaxLength`, `SuspensionFrequencyHz`, `SuspensionDamping`,
  `LongitudinalFriction`, `LateralFriction`, `HandBrake` under the `General`
  XML node. Wheels default to free rolling (`Driven` false, matching
  `CVehicleWheel`); `HandBrake` marks the wheels reached by the hand brake,
  typically the rear axle.
- `CVehicle::buildPhysicsWheels` converts the `CWheel` children to
  `CVehicleWheel` entries in child order, steered wheels get their maximum
  angle in radians. `attachPhysicsVehicle` registers chassis and wheels in
  a caller owned world, `detachPhysicsVehicle` unregisters them. The world
  must outlive the vehicle. `setDriverInputNorm` forwards normalized
  commands, it does nothing when detached.
- `CStandardController::driveForwardNorm` and `driveRightNorm` report the
  key and joystick commands normalized to [-1, 1]. Joystick axes inside a
  0.15 dead zone report zero, `applyJoystickDeadZone()` exposes the mapping
  for tests.
- `CForceController` drives an attached vehicle through driver inputs
  instead of raw chassis forces and yaw torque; detached targets keep the
  legacy force path unchanged. The hand brake is engaged by default so
  vehicles hold slopes at spawn, the `H` key toggles it on the driven
  vehicle and the state reaches the controller as the hand brake input.
- Chassis bounds come from the first mesh child: models carrying tall
  visual-only parts (masts, antenna panels) declare hull-only `Bounds` in
  the scene `Mesh` node, otherwise the tall box tips the vehicle.
- The arcade terrain alignment torque in `CVehicle::update` is skipped for
  physics attached vehicles. Detached vehicles keep it: no scene owned
  world and no terrain collision exist yet, removing it unconditionally
  would leave the VBCI without terrain alignment.
- `CTestVehicle` covers wheel defaults, XML loading, the child to physics
  conversion, attachment refusal without wheels, normalized inputs from key
  events, and an end to end drive of an attached vehicle with zero raw
  force applied.
- `CTestScene` covers terrain only ground rays, gravity aligned with the
  frozen reference down direction, attachment only over terrain,
  including late arriving terrain, close spawn snapping to measured ground, preserved high spawn altitude,
  joystick dead zone zero at rest, settle of a heavy low axle vehicle
  without launch or tip over with all tires in contact, and grounded full
  throttle drive without takeoff or flip.

## Next mapping steps

- `PhysMaterial` per tile for surface dependent grip.
- Contact listener blending flight forces with suspension load.
- Rebase of the frozen physics reference when the vehicle leaves the local
  area, and attach policy for trajectory driven vehicles.

## Terrain height fields

- `CWorldChunk::terrainGrid()` copies the ready terrain mesh altitudes,
  row 0 is the north edge, like the visual patch layout. It returns false
  without a finished terrain, callers run on the main thread.
- `CWorldTerrain::syncPhysicsHeightfields()` runs from `paint()` and
  registers one static Jolt height field per ready chunk, drops grids gone
  stale, and unregisters before `collectGarbageRecurse()` clears a terrain.
  Registration is retried every frame until it succeeds.
- `CPhysicsMath::heightfieldFrame()` builds the grid frame: center at zero
  datum in the physics frame, +X east, +Y up, +Z north, spacings from exact
  ECEF edge lengths. The planar approximation drops earth curvature over
  the chunk span, tiled material geoloc transforms are not applied.
- `CPhysicsWorld::addHeightfield()` takes north first rows and flips them
  to the Jolt row order internally. Ids grow monotonically and stay valid
  until removal. Flat grids get a padded compression range, null ranges
  quantize to invalid heights.
- `CPhysicsWorld::castRay()` reports the closest hit distance along a
  normalized direction, colliding with back faces so rays starting
  marginally under the surface still report the ground.
- `CPhysicsWorld::castTerrainRay()` uses the same query restricted to
  registered height field bodies, vehicles and other static bodies never
  count as terrain.
- `CPhysicsWorld::clear()` drops every vehicle, body and height field.
  `C3DScene::clearComponents()` calls it before destroying components.

## Vehicle auto attach

- `C3DScene::init()` seeds the physics reference on the first root vehicle
  geoloc so height fields register in the vehicle frame from the first
  paint. `ensurePhysicsReference()` freezes the first reference and ignores
  later calls.
- `C3DScene::ensureVehiclePhysics()` runs from `updateScene()` before the
  step: root vehicles with active physics, wheels and no body yet attach
  only after a downward terrain ray reaches a registered height field
  within 200 meters. Before that hit, no Jolt vehicle body exists; the
  arcade path keeps unreached vehicles grounded meanwhile.
- When the authored altitude is below measured terrain plus 20 meters, the
  vehicle geoloc snaps to the measured ground before the body is created.
  Attachment then lifts the snapped pose along reference up so tires start
  near mid suspension travel instead of ejecting the chassis from a buried
  spawn. A higher authored altitude is preserved instead of teleporting the
  spawn down.
- Water meshes never register, vehicles do not drive on water.

## Matrix layout convention

- `CMatrix4::Data` is row major and applied standardly: `M * X` equals
  row 0, `M * Y` row 1, `M * Z` row 2. Rotations are built with
  `makeRotation()`, `fromQuaternion()` or `fromBasisVectors()`, never by
  hand writing `Data` cells.
- `fromBasisVectors()` maps local axes to a right handed orthonormal
  basis. (East, up, south) is mirrored in the engine frame and no rotation
  realizes it, the valid terrain triple is (east, up, north).
- `CTestScene::testBasisVectorsMapLocalAxes` pins the engine mapping and
  the Jolt conversion of built bases, any layout regression fails there
  before physics symptoms appear.

## Scene owned world

- `C3DScene` owns a `CPhysicsWorld` created lazily by `physicsWorld()`,
  stepped from `updateScene()` at a fixed 1/60 s step, four steps maximum
  per frame, excess time dropped. `physicsSteps()` counts executed steps.
- `attachVehicleToPhysics()` registers a root wheeled vehicle with chassis
  bounds from its first mesh child bounds, no duplicated dimensions. It
  refuses wheel less vehicles, bound less vehicles and non root vehicles.
- The physics frame is the ECEF frame translated to a reference geoloc
  frozen on first attach from the vehicle position. Gravity follows the
  frozen reference down direction on every attach, including when terrain
  froze the reference before the first vehicle. The reference stays frozen
  while bodies are attached.
- `syncVehicleFromPhysics()` writes the chassis pose back after each step:
  geoloc from the body position minus the rotated center offset, local
  rotation from the body quaternion through the topocentric frame,
  velocities projected on the reference axes.
- Attached components skip the arcade integration in
  `CPhysicalComponent::update()` and the arcade impulse in
  `resolvePhysicalPair()` through `hasPhysicsBody()`, overridden by
  `CVehicle`. Detached components behave exactly as before.
- Rotation convention: the engine matrix operator applies the transposed
  matrix, `CMatrix4::fromQuaternion` was fixed to match `makeRotation` and
  `CQuaternion::operator *`. `CPhysicsMath::toQuaternion()` inverts it and
  matches Jolt quaternions, both directions covered by `CTestScene`.
- Auto attach and terrain bodies are covered below: root wheeled vehicles
  attach once physics ground reaches them, the arcade path holds the rest.
