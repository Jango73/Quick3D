# Jolt Integration Review

Scope: commit `c6a70d5 Integrate Jolt`, folder `quick3d/`, tests excluded.
Criteria: code duplication, OOP design, repository conventions from `AGENTS.md`.

## 1. Code duplication

1. `source/physics/CPhysicsWorld.cpp`: `vehiclePosition`, `vehicleRotation`, `vehicleLinearVelocity`, `vehicleAngularVelocity` repeat the same `BodyLockRead` + `Succeeded()` + conversion pattern. Extract a single locked-read helper.
2. `source/utils/CRemoteControlServer.cpp`: `serveSelect`, `serveVehicle` and `serveView` share the same shape (empty `name` check, blocking `invokeMethod`, `ok` JSON envelope). A single generic string-request function removes about 60 lines.
3. `source/components/CVehicle.cpp::update`: the wheeled branch and the box-corner fallback branch both compute `Front` / `Back`, `Right` / `Left`, `dDiffX` / `dDiffZ`, `bAlign` and the alignment torque. Extract a ground-alignment function taking sample heights and lever arms.
4. `source/components/CVehicle.cpp`: `chassisHalfExtentsM` and `chassisCenterOffsetM` duplicate the `radius() <= 0.0` guard over `firstMeshBounds()`. Share one bounds-resolution path.
5. `source/components/CStandardController.cpp`: `driveForwardNorm` and `driveRightNorm` duplicate the `[-1, 1]` clamp. Use `qBound`.
6. `source/physics/CPhysicsWorld.cpp`: `addStaticBox`, `addDynamicSphere`, `addVehicle` and `addHeightfield` duplicate validation + `ShapeResult` check + `CreateBody` + `AddBody`. Factor body creation.
7. `source/render/CAtmosphere.cpp`: +111 lines of commented pseudo-code (`Function calculate...`) and unused cubic-fit coefficients. Dead content: remove or move to `quick3d/doc/`.

## 2. OOP design

Positive points: `CPhysicsWorld` as isolated Jolt owner, `CPhysicsMath` as pure static conversions, `CRemoteControlHandler` interface with `CRemoteControlServer` limited to HTTP and JSON, encapsulated `CWheel` parameters with getters and setters, `hasPhysicsBody()` virtual hook on `CPhysicalComponent`.

Issues:

1. `source/components/CForceController.cpp` downcasts twice with `QSP_CAST(CVehicle, ...)` and drives `setDriverInputNorm` and `isHandBrakeEngaged`, concrete vehicle API from a generic controller. This bypasses the `CPhysicalComponent::hasPhysicsBody()` abstraction. Expose a virtual drive interface on `CPhysicalComponent` or `CTrajectorable`.
2. `source/render/C3DScene.cpp::ensureVehiclePhysics` and `stepPhysics` scan `m_vComponents` with `QSP_CAST(CVehicle)` on every frame. Repeated type switch, O(n) per frame. A vehicle registry or a virtual physics-attach hook expresses the same behavior without type queries.
3. `source/mesh/CWorldChunk.h` stores `m_iPhysicsHeightfieldIndex`, Jolt physics state inside a mesh object. This couples chunk and world in both directions. Move the chunk-to-heightfield map into `CWorldTerrain`.
4. `source/components/CVehicle.h` holds a raw `CPhysicsWorld* m_pPhysicsWorld` with an oral lifetime contract. `C3DScene::clearComponents` purges bodies before component destruction, which keeps the current order safe, but the dependency stays fragile. Use an explicit registration without a second `QSP` ownership chain.
5. `source/physics/CPhysicsMath.h::SHeightfieldFrame` and `heightfieldFrame` mix Jolt conversion with geo framing (ECEF, `getTopocentricAxis`). The geo-to-local frame computation belongs in a geo module, not in a Jolt helper.

## 3. Repository conventions

Blocking violations against `AGENTS.md`:

1. File size: `source/physics/CPhysicsWorld.cpp` holds 1448 lines, `source/render/C3DScene.cpp` holds 1156 lines. The limit is 1000 lines. Split `CPhysicsWorld.cpp` (initialization and step, vehicles, height fields, queries) and extract `attachVehicleToPhysics`, `ensureVehiclePhysics`, `stepPhysics` and `syncVehicleFromPhysics` from `C3DScene`.
2. Globals: `physicsInitMutex()`, `physicsInitCount()`, `broadPhaseLayerMapper()`, `objectBroadPhaseFilter()` and `objectLayerPairFilter()` are static singletons, plus the `Layers`, `BroadPhaseLayers` and `VehicleTorque` namespace-scope state. Global variables require an explicit request.
3. Flood control: `m_sLastDriverInputLog` in `CForceController.h` and `m_fLastForwardNorm` / `m_fLastRightNorm` / `m_fLastBrakeNorm` / `m_fLastHandBrakeNorm` in `CVehicleRecord` are ad-hoc change-only caches. Flood control belongs in a shared helper in `qt-plus` or `utils`.
4. Function headers: `CStandardController.cpp::applyJoystickDeadZone`, `driveForwardNorm` and `driveRightNorm` stack a `/****/` block and a `/*! \brief */` block. Keep a single header style.
5. Header exposure: `source/physics/CPhysicsMath.h` includes `<Jolt/...>` headers. The file comment restricts it to engine sources, but the `Quick3D.pri` glob (`source/physics/*.h`) captures the folder. Verify exclusion from the downstream include surface, else move Jolt signatures to the `.cpp` file.
6. Per-event logging: `CStandardController.cpp::q3dEvent` emits `LOG_DEBUG` for every event. Debug level permits the call, but unconditional logging on a hot path adds noise. Remove or gate the statement.

Conformant points: pointer style without space between type and asterisk, `Q_UNUSED` in `getContent`, systematic `QSP` null checks, short `LOG_METHOD_WARNING` and `LOG_METHOD_ERROR` messages with details kept at debug level, Jolt job pool separated from `CWorkerManager`, atomic chunk ready flag, English comments in new code, `CWheel::getClassName` fix from `ClassName_CVehicle` to `ClassName_CWheel`, pointers reset to `nullptr` after `delete` in `shutdown()`.

Pending decision: splitting `CPhysicsWorld.cpp` and `C3DScene.cpp` plus removal of the `CAtmosphere.cpp` comment blocks changes the delivered structure and requires confirmation before modification.
