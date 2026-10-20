
#ifndef WIN32
#include <sys/param.h>
#endif

// Qt
#include <QtOpenGL>

// qt-plus
#include "CLogger.h"
#include "CMemoryMonitor.h"

// Application
#include "C3DScene.h"
#include "CView.h"
#include "CRessourcesManager.h"
#include "COBJLoader.h"
#include "CMaterial.h"
#include "CHeightField.h"
#include "CWorldTerrain.h"
#include "CTrajectorable.h"
#include "CController.h"
#include "CStandardController.h"
#include "CVehicle.h"
#include "CPhysicsMath.h"

//-------------------------------------------------------------------------------------------------

namespace
{
    const double PHYSICS_STEP_S = 1.0 / 60.0;
    const int PHYSICS_MAX_STEPS_PER_FRAME = 4;
    const double PHYSICS_GROUND_RAY_HEIGHT_M = 50.0;
    const double PHYSICS_GROUND_RAY_LENGTH_M = 200.0;
    const double PHYSICS_GROUND_SNAP_ABOVE_M = 20.0;
}

//-------------------------------------------------------------------------------------------------

/*!
    \class C3DScene
    \brief The base class for a scene that holds 3D objects.
    \inmodule Quick3D
    \sa CComponent
*/

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a C3DScene with its default parameters.
    \a bForDisplay tells if the scene will be displayed
*/
C3DScene::C3DScene(bool bForDisplay)
    : m_pRessourcesManager(nullptr)
    , m_pBuildingGenerator(nullptr)
    , m_pTreeGenerator(nullptr)
    , m_vShaders(nullptr)
    , m_pGLExtension(nullptr)
    , m_pController(nullptr)
    , m_pDefaultController(nullptr)
    , m_bForDisplay(bForDisplay)
    , m_bFrustumCheck(true)
    , m_bEditMode(false)
    , m_DebugMode(false)
    , m_bBoundsOnly(false)
    , m_bNormalsOnly(false)
    , m_bOverlookScene(false)
    , m_dShaderQuality(0.5)
    , m_pRain(nullptr)
    , m_tTimeOfDay(12, 0, 0)
    , m_FPS(100)
    , m_dWindLevel(0.5)
    , m_bRenderingShadows(false)
    , m_bforceWideFOV(false)
    , m_bforceSmallFOV(false)
    , m_bForceIR(false)
    , m_bStreamView(false)
    , m_bDepthComputing(false)
    , m_dTime(0.0)
    , m_dSunIntensity(0.0)
    , m_dOverlookFOV(90.0)
    , m_bPhysicsInitialized(false)
    , m_bPhysicsReferenceValid(false)
    , m_dPhysicsAccumulatorS(0.0)
    , m_uPhysicsSteps(0)
{
    m_pSegments = QSP<CMeshGeometry>(new CMeshGeometry(this));
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys a C3DScene.
*/
C3DScene::~C3DScene()
{
    clearComponents();
    clearViewports();

    if (m_pGLExtension != nullptr) delete m_pGLExtension;
    if (m_vShaders != nullptr) delete m_vShaders;
    if (m_pTreeGenerator != nullptr) delete m_pTreeGenerator;
    if (m_pBuildingGenerator != nullptr) delete m_pBuildingGenerator;
    if (m_pRessourcesManager != nullptr) delete m_pRessourcesManager;
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes all components, lights and viewports in the scene.
*/
void C3DScene::clear()
{
    clearComponents();
    clearViewports();

    m_pRessourcesManager->clear();

    m_pController = nullptr;
    m_pDefaultController = nullptr;
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes all components in the scene.
*/
void C3DScene::clearComponents()
{
    // Note: physics bodies hold copies only, drop them before the components
    // owning the markers are destroyed
    m_PhysicsWorld.clear();

    m_bPhysicsReferenceValid = false;
    m_dPhysicsAccumulatorS = 0.0;
    m_uPhysicsSteps = 0;

    // Destruction of the components

    foreach (QSP<CComponent> pComponent, m_vComponents)
    {
        pComponent->clearLinks(this);
    }

    m_vComponents.clear();
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes all viewports in the scene.
*/
void C3DScene::clearViewports()
{
    // Destruction of the viewports

    foreach (int iIndex, m_pViewports.keys())
    {
        delete m_pViewports[iIndex];
    }

    m_pViewports.clear();
}

//-------------------------------------------------------------------------------------------------

/*!
    Initializes the scene using components in \a vComponents.
*/
void C3DScene::init(QVector<QSP<CComponent> > vComponents)
{
    LOG_METHOD_DEBUG("");

    //-----------------------------------------------
    // Init fog and sun color

    m_tFog.enabled() = true;
    m_tFog.level() = 0.0;
    m_tFog.color() = CVector3(0.5, 0.5, 0.5);

    m_iSunColor.clear();
    m_iSunColor.addValue( 0.00, CVector4(0.0, 0.0, 0.0, 1.0));
    m_iSunColor.addValue( 0.50, CVector4(1.1, 0.5, 0.4, 1.0));
    m_iSunColor.addValue( 0.60, CVector4(1.1, 0.5, 0.4, 1.0));
    m_iSunColor.addValue( 0.75, CVector4(1.4, 1.4, 1.2, 1.0));
    m_iSunColor.addValue( 1.00, CVector4(1.4, 1.4, 1.2, 1.0));

    //-----------------------------------------------
    // Assign components

    m_vComponents = vComponents;

    foreach(QSP<CComponent> pComponent, m_vComponents)
    {
        pComponent->addItems(this);
    }

    //-----------------------------------------------

    /*
    if (m_pController != nullptr)
    {
        m_pController->solveLinks(this);
    }
    */

    foreach (QSP<CComponent> pComponent, m_vComponents)
    {
        pComponent->solveLinks(this);
    }

    autoResolveHeightFields();

    // Note: seed the physics reference at the first root vehicle so terrain
    // height fields register in the vehicle frame from the first paint
    foreach (QSP<CComponent> pComponent, m_vComponents)
    {
        QSP<CVehicle> pVehicle = QSP_CAST(CVehicle, pComponent);

        if (pVehicle != nullptr && pVehicle->isRootObject() == true)
        {
            ensurePhysicsReference(pVehicle->geoloc());
            break;
        }
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Initializes shaders.
*/
void C3DScene::initShaders()
{
    LOG_METHOD_DEBUG("");
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the world origin to \a value. \br\br
    This property is used as a center point when rendering, in order to avoid huge geocentric position values in matrices and shaders, which lead to visual artifacts.
*/
void C3DScene::setWorldOrigin(Math::CVector3 value)
{
    m_vWorldOrigin = value;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the active controller of the scene to \a pController.
*/
void C3DScene::setController(CController* pController)
{
    if (pController != nullptr)
    {
        m_pController = pController;
    }
    else
    {
        m_pController = m_pDefaultController;
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the default controller of the scene to \a pController.
*/
void C3DScene::setDefaultController(CController* pController)
{
    m_pDefaultController = pController;
    m_pController = pController;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the shader quality of the scene to \a value. \br\br
    This value is used in shaders in order to easily adjust the shader level of complexity.
*/
void C3DScene::setShaderQuality(double value)
{
    m_dShaderQuality = value;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the time of the scene to \a value, expressed in seconds. \br\br
    The time property is mainly used by animated shaders.
*/
void C3DScene::setTime(double value)
{
    m_dTime = value;

    CMaterial::setTime(value);
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the time of the scene to \a value. \br\br
    The time of day property is mainly used for sunlight computation.
*/
void C3DScene::setTimeOfDay(QTime value)
{
    m_tTimeOfDay = value;
}

//-------------------------------------------------------------------------------------------------

void C3DScene::setFogLevel(double value)
{
    m_tFog.level() = value;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the force of the wind to \a value.
*/
void C3DScene::setWindLevel(double value)
{
    m_dWindLevel = value;
    if (m_dWindLevel < 0.05) m_dWindLevel = 0.05;
}

//-------------------------------------------------------------------------------------------------

void C3DScene::setRainVisible(bool bOnOff)
{
    if (m_pRain != nullptr) m_pRain->setVisible(bOnOff);
}

//-------------------------------------------------------------------------------------------------

/*!
    Forces a wide FOV when rendering if \a value is \c true.
*/
void C3DScene::forceWideFOV(bool value)
{
    m_bforceWideFOV = value;
}

//-------------------------------------------------------------------------------------------------

/*!
    Forces a small FOV when rendering if \a value is \c true.
*/
void C3DScene::forceSmallFOV(bool value)
{
    m_bforceSmallFOV = value;
}

//-------------------------------------------------------------------------------------------------

/*!
    Forces IR effect when rendering if \a value is \c true.
*/
void C3DScene::forceIR(bool value)
{
    m_bForceIR = value;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns a vector of all lights.
*/
QVector<QSP<CLight> > C3DScene::lights()
{
    QVector<QSP<CLight> > vLights;

    foreach (QSP<CComponent> pComponent, m_vComponents)
    {
        getLightsRecurse(vLights, pComponent);
    }

    return vLights;
}

//-------------------------------------------------------------------------------------------------

void C3DScene::getLightsRecurse(QVector<QSP<CLight> >& vLights, QSP<CComponent> pComponent)
{
    if (pComponent->isLight())
    {
        QSP<CLight> pLight = QSP_CAST(CLight, pComponent);

        if (pLight != nullptr)
        {
            vLights.append(pLight);
        }
    }

    foreach (QSP<CComponent> pChild, pComponent->childComponents())
    {
        getLightsRecurse(vLights, pChild);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns a vector of components whose tag matches \a sTag.
*/
QVector<QSP<CComponent> > C3DScene::componentsByTag(const QString& sTag)
{
    QVector<QSP<CComponent> > vReturnValue;

    foreach(QSP<CComponent> pComponent, m_vComponents)
    {
        if (pComponent->tag() == sTag)
        {
            vReturnValue.append(pComponent);
        }
    }

    return vReturnValue;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns a vector of lights whose tag matches \a sTag.
*/
QVector<QSP<CLight> > C3DScene::lightsByTag(const QString& sTag)
{
    QVector<QSP<CLight> > vLights;

    foreach (QSP<CComponent> pComponent, m_vComponents)
    {
        getLightsByTagRecurse(vLights, sTag, pComponent);
    }

    return vLights;
}

//-------------------------------------------------------------------------------------------------

void C3DScene::getLightsByTagRecurse(QVector<QSP<CLight> >& vLights, const QString& sTag, QSP<CComponent> pComponent)
{
    if (pComponent->isLight() && pComponent->tag() == sTag)
    {
        QSP<CLight> pLight = QSP_CAST(CLight, pComponent);

        if (pLight != nullptr)
        {
            vLights.append(pLight);
        }
    }

    foreach (QSP<CComponent> pChild, pComponent->childComponents())
    {
        getLightsByTagRecurse(vLights, sTag, pChild);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Updates the scene using elapsed time in \a dDeltaTimeS.
*/
void C3DScene::updateScene(double dDeltaTimeS)
{
    m_FPS.append(1.0 / dDeltaTimeS);

    dDeltaTimeS = Angles::clipDouble(dDeltaTimeS, 0.0, 1.0);

    if (m_bEditMode == false)
    {
        m_pSegments->clear();
    }

    if (m_pController != nullptr)
    {
        m_pController->update(dDeltaTimeS);
    }

    if (m_bEditMode == true)
    {
        dDeltaTimeS = 0.0;
    }

    ensureVehiclePhysics();

    stepPhysics(dDeltaTimeS);

    foreach (QSP<CComponent> pComponent, m_vComponents)
    {
        pComponent->update(dDeltaTimeS);
    }

    foreach (QSP<CComponent> pComponent, m_vComponents)
    {
        pComponent->postUpdate(dDeltaTimeS);
    }

    CPhysicalComponent::computeCollisions(m_vComponents, dDeltaTimeS);
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the physics world of the scene, initializing it on first call. \br\br
    Returns nullptr when initialization fails, the world stays unusable then.
*/
CPhysicsWorld* C3DScene::physicsWorld()
{
    if (m_bPhysicsInitialized == false)
    {
        if (m_PhysicsWorld.initialize() == false)
        {
            LOG_METHOD_ERROR("Physics world initialization failed");
            return nullptr;
        }

        m_bPhysicsInitialized = true;
    }

    return &m_PhysicsWorld;
}

//-------------------------------------------------------------------------------------------------

/*!
    Registers a wheeled vehicle in the physics world, returns false on failure. \br\br
    Freezes the physics reference on first attach, gravity follows the vehicle
    down direction. The reference stays frozen while bodies are attached, moving
    it requires detaching all vehicles first (rebase is future work).
*/
bool C3DScene::attachVehicleToPhysics(QSP<CVehicle> pVehicle)
{
    if (pVehicle == nullptr || pVehicle->isRootObject() == false)
    {
        return false;
    }

    if (pVehicle->hasWheels() == false)
    {
        LOG_METHOD_WARNING("Vehicle has no wheels");
        return false;
    }

    CVector3 vHalfExtentsM = pVehicle->chassisHalfExtentsM();

    if (vHalfExtentsM.X <= 0.0 || vHalfExtentsM.Y <= 0.0 || vHalfExtentsM.Z <= 0.0)
    {
        LOG_METHOD_WARNING("Vehicle has no chassis bounds");
        return false;
    }

    QVector<CVehicleWheel> vPhysicsWheels = pVehicle->buildPhysicsWheels();

    // Note: visual bounds overhang the wheels (wide armored hulls), a wide
    // collision box strikes the ground on roll and kicks the chassis from
    // side to side, clamp the footprint to the wheel track plus a margin
    double dMaxWheelXM = 0.0;
    double dMaxWheelZM = 0.0;

    foreach (const CVehicleWheel& cWheel, vPhysicsWheels)
    {
        if (fabs(cWheel.m_vPosition.X) > dMaxWheelXM)
        {
            dMaxWheelXM = fabs(cWheel.m_vPosition.X);
        }

        if (fabs(cWheel.m_vPosition.Z) > dMaxWheelZM)
        {
            dMaxWheelZM = fabs(cWheel.m_vPosition.Z);
        }
    }

    CVector3 vCenterOffsetM = pVehicle->chassisCenterOffsetM();
    vHalfExtentsM.X = qMin(vHalfExtentsM.X, dMaxWheelXM + fabs(vCenterOffsetM.X) + 0.15);
    vHalfExtentsM.Z = qMin(vHalfExtentsM.Z, dMaxWheelZM + fabs(vCenterOffsetM.Z) + 0.15);

    CPhysicsWorld* pWorld = physicsWorld();

    if (pWorld == nullptr)
    {
        return false;
    }

    if (m_bPhysicsReferenceValid == false)
    {
        ensurePhysicsReference(pVehicle->geoloc());
    }

    // Note: the reference can be frozen far from gameplay, gravity and the
    // spawn lift follow the vehicle up direction, never that distant point.
    CVector3 vVehicleUpM = pVehicle->geoloc().getTopocentricAxis().Up;
    pWorld->setGravity(vVehicleUpM * -9.81);

    CVector3 vReferenceECEF = m_gPhysicsReference.toVector3();
    CVector3 vLocalPositionM = pVehicle->geoloc().toVector3() - vReferenceECEF;

    // Note: authored axles sit low for the arcade pose, lift the spawn so
    // tires touch at mid suspension travel instead of ejecting the chassis.
    // The pose stays level by construction, lifting along the reference up.
    double dLiftM = 0.0;

    foreach (const CVehicleWheel& cWheel, vPhysicsWheels)
    {
        double dWheelLiftM = cWheel.m_dRadiusM + 0.5 * (cWheel.m_dSuspensionMinLengthM + cWheel.m_dSuspensionMaxLengthM) - cWheel.m_vPosition.Y;

        if (dWheelLiftM > dLiftM)
        {
            dLiftM = dWheelLiftM;
        }
    }

    if (dLiftM > 0.0)
    {
        vLocalPositionM = vLocalPositionM + vVehicleUpM * dLiftM;
    }

    CQuaternion cRotation = CPhysicsMath::toQuaternion(CMatrix4::makeRotation(pVehicle->ECEFRotation()));

    return pVehicle->attachPhysicsVehicle(pWorld, vHalfExtentsM, vLocalPositionM, cRotation, pVehicle->chassisCenterOffsetM());
}

//-------------------------------------------------------------------------------------------------

/*!
    Freezes the physics reference on \a gReference when none is set. \br\br
    The first frozen reference wins, later calls are ignored.
*/
void C3DScene::ensurePhysicsReference(const CGeoloc& gReference)
{
    if (m_bPhysicsReferenceValid == true)
    {
        return;
    }

    m_gPhysicsReference = gReference;
    m_bPhysicsReferenceValid = true;
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes a vehicle from the physics world, does nothing when detached
*/
void C3DScene::detachVehicleFromPhysics(QSP<CVehicle> pVehicle)
{
    if (pVehicle == nullptr)
    {
        return;
    }

    pVehicle->detachPhysicsVehicle();
}

//-------------------------------------------------------------------------------------------------

/*!
    Attaches grounded root vehicles to the physics world. \br\br
    A candidate needs active physics, wheels and no attached body yet. A
    registered height field must reach it first: a downward terrain ray in
    the physics frame decides, the arcade path keeps unattached vehicles
    grounded meanwhile. A close spawn snaps to the measured ground before
    the body is created.
*/
void C3DScene::ensureVehiclePhysics()
{
    CPhysicsWorld* pWorld = physicsWorld();

    if (pWorld == nullptr)
    {
        return;
    }

    foreach (QSP<CComponent> pComponent, m_vComponents)
    {
        QSP<CVehicle> pVehicle = QSP_CAST(CVehicle, pComponent);

        if (pVehicle == nullptr || pVehicle->isRootObject() == false)
        {
            continue;
        }

        if (pVehicle->physicsActive() == false || pVehicle->hasPhysicsVehicle() == true)
        {
            continue;
        }

        if (pVehicle->hasWheels() == false)
        {
            continue;
        }

        ensurePhysicsReference(pVehicle->geoloc());

        CGeoloc gVehicle = pVehicle->geoloc();
        CVector3 vUp = gVehicle.getTopocentricAxis().Up;

        CGeoloc gRayOrigin(gVehicle.Latitude, gVehicle.Longitude, gVehicle.Altitude + PHYSICS_GROUND_RAY_HEIGHT_M);
        CVector3 vOrigin = gRayOrigin.toVector3() - m_gPhysicsReference.toVector3();

        double dHitDistanceM = 0.0;

        if (pWorld->castTerrainRay(vOrigin, vUp * -1.0, PHYSICS_GROUND_RAY_LENGTH_M, &dHitDistanceM) == false)
        {
            continue;
        }

        CGeoloc gGround(m_gPhysicsReference.toVector3() + vOrigin - vUp * dHitDistanceM);

        // Note: only a close spawn moves to the measured ground, a high
        // spawn keeps its authored altitude instead of teleporting down.
        if (gVehicle.Altitude < gGround.Altitude + PHYSICS_GROUND_SNAP_ABOVE_M)
        {
            pVehicle->setGeoloc(gGround);
        }

        attachVehicleToPhysics(pVehicle);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Advances the physics world at a fixed step and syncs attached vehicles. \br\br
    Caps the steps per frame, excess time is dropped to avoid spiral of death.
*/
void C3DScene::stepPhysics(double dDeltaTimeS)
{
    if (m_bPhysicsInitialized == false)
    {
        return;
    }

    QVector<QSP<CVehicle> > vAttachedVehicles;

    foreach (QSP<CComponent> pComponent, m_vComponents)
    {
        QSP<CVehicle> pVehicle = QSP_CAST(CVehicle, pComponent);

        if (pVehicle != nullptr && pVehicle->hasPhysicsVehicle() == true)
        {
            vAttachedVehicles.append(pVehicle);
        }
    }

    if (vAttachedVehicles.count() == 0)
    {
        m_dPhysicsAccumulatorS = 0.0;
        return;
    }

    if (dDeltaTimeS > 0.0)
    {
        m_dPhysicsAccumulatorS += dDeltaTimeS;
    }

    int iSteps = 0;

    while (m_dPhysicsAccumulatorS >= PHYSICS_STEP_S && iSteps < PHYSICS_MAX_STEPS_PER_FRAME)
    {
        m_PhysicsWorld.step(PHYSICS_STEP_S);
        m_dPhysicsAccumulatorS -= PHYSICS_STEP_S;
        m_uPhysicsSteps++;
        iSteps++;
    }

    if (m_dPhysicsAccumulatorS >= PHYSICS_STEP_S)
    {
        m_dPhysicsAccumulatorS = 0.0;
    }

    foreach (QSP<CVehicle> pVehicle, vAttachedVehicles)
    {
        syncVehicleFromPhysics(pVehicle);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Writes the physics chassis pose back to the vehicle geoloc and rotation. \br\br
    The physics frame is the ECEF frame translated to the frozen reference,
    velocities are projected on the reference topocentric axes.
*/
void C3DScene::syncVehicleFromPhysics(QSP<CVehicle> pVehicle)
{
    if (pVehicle == nullptr || pVehicle->hasPhysicsVehicle() == false)
    {
        return;
    }

    CPhysicsWorld* pWorld = physicsWorld();

    if (pWorld == nullptr)
    {
        return;
    }

    int iVehicle = pVehicle->physicsVehicleIndex();

    CVector3 vBodyPosition;
    CQuaternion cBodyRotation;
    CVector3 vLinearVelocity;
    CVector3 vAngularVelocity;
    CVector3 vCenterOffsetM;

    if (pWorld->vehiclePosition(iVehicle, vBodyPosition) == false)
    {
        return;
    }

    if (pWorld->vehicleRotation(iVehicle, cBodyRotation) == false)
    {
        return;
    }

    pWorld->vehicleLinearVelocity(iVehicle, vLinearVelocity);
    pWorld->vehicleAngularVelocity(iVehicle, vAngularVelocity);
    pWorld->vehicleCenterOffsetM(iVehicle, vCenterOffsetM);

    CMatrix4 mBodyRotation = CMatrix4::fromQuaternion(cBodyRotation);

    CVector3 vOriginECEF = vBodyPosition + m_gPhysicsReference.toVector3() - (mBodyRotation * vCenterOffsetM);
    CGeoloc gOrigin(vOriginECEF);

    pVehicle->setGeoloc(gOrigin);

    CAxis aBody;
    aBody.Right = CVector3(mBodyRotation.Data[0][0], mBodyRotation.Data[0][1], mBodyRotation.Data[0][2]);
    aBody.Up = CVector3(mBodyRotation.Data[1][0], mBodyRotation.Data[1][1], mBodyRotation.Data[1][2]);
    aBody.Front = CVector3(mBodyRotation.Data[2][0], mBodyRotation.Data[2][1], mBodyRotation.Data[2][2]);

    CVector3 vLocalRotation = CAxis(aBody.eulerAngles()).transferFrom(gOrigin.getTopocentricAxis()).eulerAngles();

    pVehicle->setRotation(vLocalRotation);

    CAxis aReference = m_gPhysicsReference.getTopocentricAxis();

    pVehicle->setVelocity_ms(CVector3(vLinearVelocity.dot(aReference.Right), vLinearVelocity.dot(aReference.Up), vLinearVelocity.dot(aReference.Front)));
    pVehicle->setAngularVelocity_rs(CVector3(vAngularVelocity.dot(aReference.Right), vAngularVelocity.dot(aReference.Up), vAngularVelocity.dot(aReference.Front)));
}

//-------------------------------------------------------------------------------------------------

/*!
    Paints all components, using \a pContext.
*/
void C3DScene::paintComponents(CRenderContext* pContext)
{
    foreach(QSP<CComponent> pComponent, m_vComponents)
    {
        if (pComponent->isVisible())
        {
            pComponent->paint(pContext);
            pComponent->postPaint(pContext);
        }
    }

#ifdef DRAW_BY_MATERIAL
    pContext->meshByMaterial()->paint(pContext);
#endif

    m_pSegments->paint(pContext, nullptr);
}

//-------------------------------------------------------------------------------------------------

/*!
    Paints all components that cast shadows, using \a pContext.
*/
void C3DScene::paintShadowCastingComponents(CRenderContext* pContext)
{
    foreach(QSP<CComponent> pComponent, m_vComponents)
    {
        if (pComponent->isVisible() && pComponent->castsShadows())
        {
            pComponent->paint(pContext);
            pComponent->postPaint(pContext);
        }
    }

#ifdef DRAW_BY_MATERIAL
    pContext->meshByMaterial()->paint(pContext);
#endif
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets up the environment of the scene.
*/
void C3DScene::setupEnvironment(CRenderContext* pContext, QOpenGLShaderProgram* pProgram, bool bBackgroundItem)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets up the lights of the scene.
*/
void C3DScene::setupLights(CRenderContext* pContext)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Makes the OpenGL context of this scene the current one.
*/
void C3DScene::makeCurrentRenderingContext()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds \a pComponent to the scene.
*/
void C3DScene::addComponent(QSP<CComponent> pComponent)
{
    m_vComponents.append(pComponent);
    pComponent->solveLinks(this);
    autoResolveHeightFields();
}

//-------------------------------------------------------------------------------------------------

/*!
    Assigns the first CHeightField in the scene to all CPhysicalComponent that need it for physics.
*/
void C3DScene::autoResolveHeightFields()
{
    CHeightField* pTerrain = nullptr;

    foreach (QSP<CComponent> pComponent, m_vComponents)
    {
        if (dynamic_cast<CWorldTerrain*>(pComponent.data()) != nullptr)
        {
            pTerrain = dynamic_cast<CHeightField*>(pComponent.data());
        }
        else
        {
            if (dynamic_cast<CTrajectorable*>(pComponent.data()) != nullptr)
            {
                CPhysicalComponent* pPhysical = dynamic_cast<CPhysicalComponent*>(pComponent.data());

                if (pPhysical != nullptr && pTerrain != nullptr)
                {
                    pPhysical->addField(pTerrain);
                }
            }
        }

        pComponent->update(0.0);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Deletes all components in the scene that have a tag equal to \a sTag.
*/
void C3DScene::deleteComponentsByTag(const QString& sTag)
{
    for (int iIndex = 0; iIndex < m_vComponents.count(); iIndex++)
    {
        if (m_vComponents[iIndex]->tag() == sTag)
        {
            m_vComponents[iIndex]->clearLinks(this);
            m_vComponents.remove(iIndex);
            iIndex--;
        }
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Checks if \a rRay intersects components in the scene.
*/
RayTracingResult C3DScene::intersect(Math::CRay3 rRay) const
{
    RayTracingResult dReturnResult(Q3D_INFINITY);

    foreach (QSP<CComponent> pComponent, m_vComponents)
    {
        RayTracingResult dNewResult = intersectRecurse(pComponent, rRay);

        if (dNewResult.m_dDistance < dReturnResult.m_dDistance)
        {
            dReturnResult = dNewResult;
        }
    }

    return dReturnResult;
}

//-------------------------------------------------------------------------------------------------

/*!
    Checks if \a ray intersects the component specified by \a pComponent, or any of its children.
*/
RayTracingResult C3DScene::intersectComponentHierarchy(QSP<CComponent> pComponent, Math::CRay3 aRay) const
{
    RayTracingResult aResult = intersectRecurse(pComponent, aRay);
    return aResult;
}

//-------------------------------------------------------------------------------------------------

RayTracingResult C3DScene::intersectRecurse(QSP<CComponent> pComponent, const Math::CRay3& aRay) const
{
    if (pComponent->isRaytracable())
    {
        RayTracingResult dReturnResult = pComponent->intersect(aRay);

        foreach (QSP<CComponent> pChild, pComponent->childComponents())
        {
            RayTracingResult dChildResult = intersectRecurse(pChild, aRay);

            if (dChildResult.m_dDistance < dReturnResult.m_dDistance)
            {
                dReturnResult = dChildResult;
            }
        }

        return dReturnResult;
    }

    return RayTracingResult(Q3D_INFINITY);
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds a segment in the scene from \a vStart to \a vEnd for debugging.
*/
void C3DScene::addSegment(Math::CVector3 vStart, Math::CVector3 vEnd)
{
    if (m_pSegments->vertices().count() > 1000) return;

    if (m_bEditMode == false)
    {
        m_pSegments->vertices().append(CVertex(vStart - m_vWorldOrigin));
        m_pSegments->vertices().append(CVertex(vEnd - m_vWorldOrigin));
    }
}

//-------------------------------------------------------------------------------------------------

QString C3DScene::debugInfo()
{
    CGeoloc ControlledGeoloc;
    CVector3 ControlledRotation;
    CVector3 ControledVelocity;
    CVector3 ControledTorque;
    double dSpeedMS = 0.0;

    if (m_pController != nullptr && m_pController->positionTarget() != nullptr)
    {
        QSP<CPhysicalComponent> pPhysical = QSP_CAST(CPhysicalComponent, m_pController->positionTarget()->root());

        if (pPhysical != nullptr)
        {
            ControlledGeoloc = pPhysical->geoloc();
            ControledVelocity = pPhysical->velocity_ms();
            ControledTorque = pPhysical->angularVelocity_rs();
            dSpeedMS = ControledVelocity.magnitude();
        }
    }

    if (m_pController != nullptr && m_pController->rotationTarget() != nullptr)
    {
        QSP<CPhysicalComponent> pPhysical = QSP_CAST(CPhysicalComponent, m_pController->rotationTarget()->root());

        if (pPhysical != nullptr)
        {
            ControlledRotation = pPhysical->rotation();
        }
    }

    return QString(
                "FPS %1 - LLA (%2, %3, %4) Rotation (%5, %6, %7) Kts %8 Velocity (%9, %10, %11) Torque (%12, %13, %14) \n"
                "Render : meshes %15 polys %16 chunks %17 \n"
                "Components %18, chunks %19, terrains %20, bmi %21 \n"
                "Allocated bytes : %22 \n"
                )
            .arg((int) m_FPS.getAverage())
            .arg(QString::number(ControlledGeoloc.Latitude, 'f', 6))
            .arg(QString::number(ControlledGeoloc.Longitude, 'f', 6))
            .arg(QString::number(ControlledGeoloc.Altitude, 'f', 1))
            .arg(QString::number(Math::Angles::toDeg(ControlledRotation.X), 'f', 2))
            .arg(QString::number(Math::Angles::toDeg(ControlledRotation.Y), 'f', 2))
            .arg(QString::number(Math::Angles::toDeg(ControlledRotation.Z), 'f', 2))
            .arg(QString::number(dSpeedMS * FAC_MS_TO_KNOTS, 'f', 1))
            .arg(QString::number(ControledVelocity.X, 'f', 2))
            .arg(QString::number(ControledVelocity.Y, 'f', 2))
            .arg(QString::number(ControledVelocity.Z, 'f', 2))
            .arg(QString::number(Math::Angles::toDeg(ControledTorque.X), 'f', 2))
            .arg(QString::number(Math::Angles::toDeg(ControledTorque.Y), 'f', 2))
            .arg(QString::number(Math::Angles::toDeg(ControledTorque.Z), 'f', 2))

            .arg(m_tStatistics.m_iNumMeshesDrawn)
            .arg(m_tStatistics.m_iNumPolysDrawn)
            .arg(m_tStatistics.m_iNumChunksDrawn)

            .arg(CComponent::getNumComponents())
            .arg(CComponent::componentCounter()[ClassName_CWorldChunk])
            .arg(CComponent::componentCounter()[ClassName_CTerrain])
            .arg(CComponent::componentCounter()[ClassName_CBoundedMeshInstances])

            .arg(CMemoryMonitor::getInstance()->allocatedBytes())
            ;
}

//-------------------------------------------------------------------------------------------------

/*!
    Dumps this scene to \a stream using the indentation value in \a iIdent.
*/
void C3DScene::dump(QTextStream& stream, int iIdent)
{
    dumpIndented(stream, iIdent, QString("[C3DScene]"));
    dumpIndented(stream, iIdent, QString("CComponent::getNumComponents() : %1").arg(CComponent::getNumComponents()));

    foreach (QString sName, CComponent::componentCounter().keys())
    {
        dumpIndented(stream, iIdent, QString("CComponent::componentCounter(%1) : %2")
                  .arg(sName)
                  .arg(CComponent::componentCounter()[sName]));
    }

    dumpIndented(stream, iIdent, QString("CMemoryMonitor::allocatedBytes() : %1").arg(CMemoryMonitor::getInstance()->allocatedBytes()));

    foreach (QString sName, CMemoryMonitor::getInstance()->allocationMap().keys())
    {
        dumpIndented(stream, iIdent, QString("Allocated bytes for %1 : %2")
                  .arg(sName)
                  .arg(CMemoryMonitor::getInstance()->allocationMap()[sName]));
    }

    dumpIndented(stream, iIdent, QString("Components :"));

    foreach (QSP<CComponent> pComponent, m_vComponents)
    {
        dumpOpenBlock(stream, iIdent);
        pComponent->dump(stream, iIdent + 1);
        dumpCloseBlock(stream, iIdent);
    }
}
