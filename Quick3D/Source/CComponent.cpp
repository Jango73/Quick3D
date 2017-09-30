
// qt-plus
#include "CLogger.h"

// Application
#include "Angles.h"
#include "CAxis.h"
#include "C3DScene.h"
#include "CComponent.h"
#include "CController.h"
#include "CFace.h"

//-------------------------------------------------------------------------------------------------

/*!
    \class CComponent
    \brief The base class for all 3D entities existing in the Quick3D engine.
    \inmodule Quick3D
    \sa C3DScene
    \section1 Conventions for frames, axes and rotations
    \section2 The geodetic frame
    In this frame, coordinates are expressed as latitude, longitude and altitude using the CGeoloc class.
    \section2 The geocentric frame, or ECEF (Earth-Centered Earth-Fixed)
    In this frame, the origin is the center of the earth. Coordinates are expressed as x, y and z. \br
    In order to visualize the axis conventions, imagine you are in orbit, at the latitude 0 and longitude 0 point, looking at the earth. \br
    The positive Z axis points in front of you, the positive X axis points right of you, and the positive Y axis points upwards.
    \section2 The NOLL frame (North-Oriented Local-Level)
    In this frame, the origin is a point on earth. Coordinates are expressed as x, y and z. \br
    In order to visualize the axis conventions, imagine you are somewhere on earth, looking at true north. \br
    The positive Z axis points in front of you, the positive X axis points right of you, and the positive Y axis points upwards.
    \section2 The local frame
    In this frame, the origin is the center of an object. Coordinates are expressed as x, y and z. \br
*/

//-------------------------------------------------------------------------------------------------

using namespace Math;

IMPLEMENT_MEMORY_MONITORED(CComponent, "CComponent")

//-------------------------------------------------------------------------------------------------
// Properties statiques

int CComponent::m_iNumComponents = 0;

QMap<QString, int> CComponent::m_mComponentCounter;

//-------------------------------------------------------------------------------------------------

/*!
    Instantiates a new CComponent.
    \a pScene is the scene containing the component.
*/
CComponent* CComponent::instantiator(C3DScene* pScene)
{
    return new CComponent(pScene);
}

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CComponent with its default parameters.
    \a pScene is the scene containing the component.
*/
CComponent::CComponent(C3DScene* pScene)
    : m_pScene(pScene)
    , m_pController(nullptr)
    , m_pParent(nullptr)
    , m_vPosition(0.0, 0.0, 0.0)
    , m_vRotation(0.0, 0.0, 0.0)
    , m_vScale(1.0, 1.0, 1.0)
    , m_vAnimPosition(0.0, 0.0, 0.0)
    , m_vAnimRotation(0.0, 0.0, 0.0)
    , m_vAnimScale(1.0, 1.0, 1.0)
    , m_vRotationFactor(1.0, 1.0, 1.0)
    , m_vRotationMinimum(-1000.0, -1000.0, -1000.0)
    , m_vRotationMaximum( 1000.0,  1000.0,  1000.0)
    , m_bVisible(true)
    , m_bCastShadows(true)
    , m_bReceiveShadows(true)
    , m_bRaytracable(true)
    , m_bInheritTransform(true)
    , m_bSelected(false)
    , m_dStatus(1.0)
{
    Q_UNUSED(pScene);
    LOG_METHOD_DEBUG(QString::number((quint32) this, 16));

    m_iNumComponents++;
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys a CComponent.
*/
CComponent::~CComponent()
{
    LOG_METHOD_DEBUG(QString::number((quint32) this, 16));

    m_iNumComponents--;

    foreach (QSP<CComponent> pChild, m_vChildren)
    {
        pChild->clearLinks(m_pScene);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the component's tag to \a sValue. \br\br
    The tag is a user defined value, for user defined purpose.
*/
void CComponent::setTag(const QString& sValue)
{
    m_sTag = sValue;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the component's controller to \a pController.
*/
void CComponent::setController(CController* pController)
{
    m_pController = pController;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the component's visibility to \a bValue.
*/
void CComponent::setVisible(bool bValue)
{
    m_bVisible = bValue;
}

//-------------------------------------------------------------------------------------------------

/*!
    If \a bValue is \c true, the component will cast shadows.
*/
void CComponent::setCastShadows(bool bValue)
{
    m_bCastShadows = bValue;
}

//-------------------------------------------------------------------------------------------------

/*!
    If \a bValue is \c true, the component will receive shadows.
*/
void CComponent::setReceiveShadows(bool bValue)
{
    m_bReceiveShadows = bValue;
}

//-------------------------------------------------------------------------------------------------

/*!
    If \a bValue is \c true, the component will generate interscetions with rays.
*/
void CComponent::setRaytracable(bool bValue)
{
    m_bRaytracable = bValue;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the selection flag to \a bValue.
*/
void CComponent::setSelected(bool bValue)
{
    m_bSelected = bValue;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the component's parent to \a pParent.
*/
void CComponent::setParent(QSP<CComponent> pParent)
{
    // Si l'objet a déjà un parent, on le supprime de la liste d'enfants du parent
    if (m_pParent != nullptr)
    {
        m_pParent->m_vChildren.remove(m_pParent->m_vChildren.indexOf(QSP<CComponent>(this)));
    }

    // Assignation du nom de parent et du parent
    m_sParentName = pParent->m_sName;
    m_pParent = pParent;

    if (m_bInheritTransform && m_pParent)
    {
        m_pParent->m_vChildren.append(QSP<CComponent>(this));
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Solves the links in this component after it has been loaded from an object file (XML or JSON). \br\br
    \a pScene is the scene containing this component.
*/
void CComponent::solveLinks(C3DScene* pScene)
{
    foreach (QSP<CComponent> pChild, m_vChildren)
    {
        pChild->solveLinks(pScene);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Clears the links in this component and its children. \br\br
    \a pScene is the scene containing this component.
*/
void CComponent::clearLinks(C3DScene* pScene)
{
    foreach (QSP<CComponent> pChild, m_vChildren)
    {
        pChild->clearLinks(pScene);
    }

    m_vChildren.clear();
    m_sParentName.clear();
    m_pParent.reset();
}

//-------------------------------------------------------------------------------------------------

/*!
    Looks for a component specified by \a sName, in this components tree. \br\br
    \a sName is like a file system path, it can contain '.' characters to separate component names. \br
*/
QSP<CComponent> CComponent::findComponent(QString sName, QSP<CComponent> pCaller)
{
    if (sName.isEmpty() == false)
    {
        QStringList lNames = sName.split(".", QString::KeepEmptyParts);

        if (lNames[0].isEmpty() && pCaller)
        {
            lNames[0] = pCaller->root()->name();
        }

        if (lNames[0] == m_sName)
        {
            if (lNames.count() == 1)
            {
                return QSP<CComponent>(this);
            }

            QString sRemaining;

            for (int iIndex = 1; iIndex < lNames.count(); iIndex++)
            {
                if (sRemaining.length() > 0)
                {
                    sRemaining = sRemaining + "." + lNames[iIndex];
                }
                else
                {
                    sRemaining = lNames[iIndex];
                }
            }

            foreach (QSP<CComponent> pChild, m_vChildren)
            {
                QSP<CComponent> pFound = pChild->findComponent(sRemaining);

                if (pFound != nullptr)
                {
                    return pFound;
                }
            }
        }
    }

    return QSP<CComponent>(nullptr);
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the path name of this component.
*/
QString CComponent::qualifiedName()
{
    QString sReturnValue = m_sName;

    QSP<CComponent> pParent = m_pParent;

    while (pParent)
    {
        sReturnValue = pParent->m_sName + "." + sReturnValue;
        pParent = pParent->m_pParent;
    }

    return sReturnValue;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the controller of this component.
*/
CController* CComponent::controller()
{
    return m_pController;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns \c true if the component is visible (will be rendered). \br\br
    Even when invisible, an object can generate ray intersections.
*/
bool CComponent::isVisible() const
{
    return m_bVisible;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns \c true if the component can cast shadows.
*/
bool CComponent::castsShadows() const
{
    return m_bCastShadows;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns \c true if the component can receive shadows.
*/
bool CComponent::receivesShadows() const
{
    return m_bReceiveShadows;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the geodetic location of this component to \a gGeoloc.
*/
void CComponent::setGeoloc(CGeoloc gGeoloc)
{
    // Coordinates are clipped between -180.0 and +180.0 degrees
    gGeoloc.Latitude = Math::Angles::clipAngleDegreePIMinusPI(gGeoloc.Latitude);
    gGeoloc.Longitude = Math::Angles::clipAngleDegreePIMinusPI(gGeoloc.Longitude);

    // Latitude is clipped between -89.9 and +89.9 degrees
    gGeoloc.Latitude = Math::Angles::clipDouble(gGeoloc.Latitude, -89.9, 89.9);
    gGeoloc.Altitude = Math::Angles::clipDouble(gGeoloc.Altitude, -20000.0, 12000000.0);

    // Is the object a root object or is it independent transformation wise?
    if (isRootObject() || m_bInheritTransform == false)
    {
        if (gGeoloc.valid())
        {
            m_vPosition = CVector3(0.0, 0.0, 0.0);
            m_gGeoloc = gGeoloc;
        }
    }

    if (isRootObject() && m_bInheritTransform)
    {
        m_vECEFRotation = toECEFRotation(m_vRotation);
    }
    else
    {
        m_vECEFRotation = m_vRotation;
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the geocentric location of this component, if it is a root object. Else, sets the local cartesian location.
*/
void CComponent::setPosition(CVector3 vPosition)
{
    if (isRootObject() || m_bInheritTransform == false)
    {
        m_vPosition = CVector3(0.0, 0.0, 0.0);
        m_gGeoloc = CGeoloc(vPosition);
    }
    else
    {
        m_vPosition = vPosition;
    }

    if (isRootObject() && m_bInheritTransform)
    {
        m_vECEFRotation = toECEFRotation(m_vRotation);
    }
    else
    {
        m_vECEFRotation = m_vRotation;
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the ECEF euler rotation of this component to \a Rotation, if it is a root object. Else, sets the local euler rotation. \br\br
    \a Rotation is specified in radians.
*/
void CComponent::setRotation(CVector3 vRotation)
{
    // Assign the original rotation
    m_vRotation = vRotation * m_vRotationFactor;

    // Clip angles between -PI and PI
    m_vRotation.X = Math::Angles::clipAngleRadianPIMinusPI(m_vRotation.X);
    m_vRotation.Y = Math::Angles::clipAngleRadianPIMinusPI(m_vRotation.Y);
    m_vRotation.Z = Math::Angles::clipAngleRadianPIMinusPI(m_vRotation.Z);

    if (isRootObject() || m_bInheritTransform == false)
    {
        // Compute rotation in the ECEF frame
        m_vECEFRotation = toECEFRotation(m_vRotation);
    }
    else
    {
        m_vECEFRotation = m_vRotation;
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the local scale of the component to \a Scale.
*/
void CComponent::setScale(CVector3 vScale)
{
    m_vScale = vScale;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the animated (offset) location of this component to \a Position.
*/
void CComponent::setAnimPosition(CVector3 vPosition)
{
    m_vAnimPosition = vPosition;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the animated (offset) euler rotation of this component. \br\br
    \a Rotation is specified in radians.
*/
void CComponent::setAnimRotation(CVector3 vRotation)
{
    // Assign rotation
    m_vAnimRotation = vRotation * m_vRotationFactor;

    // Clip angles between -PI and PI
    m_vAnimRotation.X = Math::Angles::clipAngleRadianPIMinusPI(m_vAnimRotation.X);
    m_vAnimRotation.Y = Math::Angles::clipAngleRadianPIMinusPI(m_vAnimRotation.Y);
    m_vAnimRotation.Z = Math::Angles::clipAngleRadianPIMinusPI(m_vAnimRotation.Z);

    if (m_vAnimRotation.X < m_vRotationMinimum.X) m_vAnimRotation.X = m_vRotationMinimum.X;
    if (m_vAnimRotation.Y < m_vRotationMinimum.Y) m_vAnimRotation.Y = m_vRotationMinimum.Y;
    if (m_vAnimRotation.Z < m_vRotationMinimum.Z) m_vAnimRotation.Z = m_vRotationMinimum.Z;

    if (m_vAnimRotation.X > m_vRotationMaximum.X) m_vAnimRotation.X = m_vRotationMaximum.X;
    if (m_vAnimRotation.Y > m_vRotationMaximum.Y) m_vAnimRotation.Y = m_vRotationMaximum.Y;
    if (m_vAnimRotation.Z > m_vRotationMaximum.Z) m_vAnimRotation.Z = m_vRotationMaximum.Z;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the animated (offset) scale of this component to \a Scale.
*/
void CComponent::setAnimScale(CVector3 vScale)
{
    m_vAnimScale = vScale;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the world transform of this component to \a value. \br\br
    This should not be called directly as it is computed from position, rotation and scale properties.
*/
void CComponent::setWorldTransform(const Math::CMatrix4& value)
{
    // Assign the transform matrix and its inverse

    m_mWorldTransform = value;
    m_mWorldTransformInverse = m_mWorldTransform.inverse();
}

//-------------------------------------------------------------------------------------------------

/*!
    If \a bValue is \c true, the component will inherit the transforms of its parent.
*/
void CComponent::setInheritTransform(bool bValue)
{
    m_bInheritTransform = bValue;
}

//-------------------------------------------------------------------------------------------------

void CComponent::setPreviousWorldTransform(const Math::CMatrix4& mTransform)
{
    m_mPreviousWorldTransform = mTransform;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the working status of the component to \a dValue.
*/
void CComponent::setStatus(double dValue)
{
    m_dStatus = dValue;
}

//-------------------------------------------------------------------------------------------------

CVector3 CComponent::toECEFRotation(CVector3 vRotation) const
{
    return CAxis(vRotation).transferTo(m_gGeoloc.getNOLLAxis()).eulerAngles();
}

//-------------------------------------------------------------------------------------------------

Math::CVector3 CComponent::fromFrame(CComponent* pFrom, Math::CVector3 vPosition) const
{
    vPosition = pFrom->m_mWorldTransform * vPosition;
    vPosition = m_mWorldTransformInverse * vPosition;

    return vPosition;
}

//-------------------------------------------------------------------------------------------------

/*!
    Copies properties of \a target to this component.
*/
CComponent& CComponent::operator = (const CComponent& target)
{
    m_pScene                    = target.m_pScene;
    m_sName                     = target.m_sName;
    m_sParentName               = target.m_sParentName;
    m_pParent                   = target.m_pParent;
    m_gGeoloc                   = target.m_gGeoloc;
    m_vECEFRotation             = target.m_vECEFRotation;
    m_vPosition                 = target.m_vPosition;
    m_vRotation                 = target.m_vRotation;
    m_vScale                    = target.m_vScale;
    m_vAnimPosition             = target.m_vAnimPosition;
    m_vAnimRotation             = target.m_vAnimRotation;
    m_vAnimScale                = target.m_vAnimScale;
    m_mWorldTransform           = target.m_mWorldTransform;
    m_mWorldTransformInverse    = target.m_mWorldTransformInverse;
    m_bVisible                  = target.m_bVisible;
    m_bInheritTransform         = target.m_bInheritTransform;

    return *this;
}

//-------------------------------------------------------------------------------------------------

/*!
    Loads the properties of this component from \a xComponent. \br\br
    \a sBaseFile is the file name from which it is loaded.
*/
void CComponent::loadParameters(const QString& sBaseFile, const CXMLNode& xComponent)
{
    Q_UNUSED(sBaseFile);

    CXMLNode xGeoloc = xComponent.getNodeByTagName(ParamName_Geoloc);
    CXMLNode xPosition = xComponent.getNodeByTagName(ParamName_Position);
    CXMLNode xRotation = xComponent.getNodeByTagName(ParamName_Rotation);
    CXMLNode xScale = xComponent.getNodeByTagName(ParamName_Scale);
    CXMLNode xRotationFactor = xComponent.getNodeByTagName(ParamName_RotationFactor);
    CXMLNode xRotationMinimum = xComponent.getNodeByTagName(ParamName_RotationMinimum);
    CXMLNode xRotationMaximum = xComponent.getNodeByTagName(ParamName_RotationMaximum);

    // Component name

    if (xComponent.attributes()[ParamName_Name].isEmpty() == false)
    {
        m_sName = xComponent.attributes()[ParamName_Name];
    }

    // Component tag

    if (xComponent.attributes()[ParamName_Tag].isEmpty() == false)
    {
        m_sTag = xComponent.attributes()[ParamName_Tag];
    }

    // Original position

    if (xPosition.isEmpty() == false)
    {
        setPosition(CVector3(
                              xPosition.attributes()[ParamName_x].toDouble(),
                              xPosition.attributes()[ParamName_y].toDouble(),
                              xPosition.attributes()[ParamName_z].toDouble()
                              ));
    }

    // Geo-location

    if (xGeoloc.isEmpty() == false)
    {
        setGeoloc(CGeoloc(
                      xGeoloc.attributes()[ParamName_Latitude].toDouble(),
                      xGeoloc.attributes()[ParamName_Longitude].toDouble(),
                      xGeoloc.attributes()[ParamName_Altitude].toDouble()
                      ));
    }

    // Original rotation

    if (xRotation.isEmpty() == false)
    {
        setRotation(CVector3(
                              Angles::toRad(xRotation.attributes()[ParamName_x].toDouble()),
                              Angles::toRad(xRotation.attributes()[ParamName_y].toDouble()),
                              Angles::toRad(xRotation.attributes()[ParamName_z].toDouble())
                              ));
    }

    // Rotation factors (allows locking one or more axes)

    if (xRotationFactor.isEmpty() == false)
    {
        m_vRotationFactor = CVector3(
                    xRotationFactor.attributes()[ParamName_x].toDouble(),
                    xRotationFactor.attributes()[ParamName_y].toDouble(),
                    xRotationFactor.attributes()[ParamName_z].toDouble()
                    );
    }

    // Minimum rotation

    if (xRotationMinimum.isEmpty() == false)
    {
        m_vRotationMinimum = CVector3(
                    Angles::toRad(xRotationMinimum.attributes()[ParamName_x].toDouble()),
                    Angles::toRad(xRotationMinimum.attributes()[ParamName_y].toDouble()),
                    Angles::toRad(xRotationMinimum.attributes()[ParamName_z].toDouble())
                    );
    }

    // Maximum rotation

    if (xRotationMaximum.isEmpty() == false)
    {
        m_vRotationMaximum = CVector3(
                    Angles::toRad(xRotationMaximum.attributes()[ParamName_x].toDouble()),
                    Angles::toRad(xRotationMaximum.attributes()[ParamName_y].toDouble()),
                    Angles::toRad(xRotationMaximum.attributes()[ParamName_z].toDouble())
                    );
    }

    // Scale

    if (xScale.isEmpty() == false)
    {
        CVector3 vScale = CVector3(
                    xScale.attributes()[ParamName_x].toDouble(),
                    xScale.attributes()[ParamName_y].toDouble(),
                    xScale.attributes()[ParamName_z].toDouble()
                    );

        if (vScale.X != 0.0 && vScale.Y != 0.0 && vScale.Z != 0.0)
        {
            setScale(vScale);
        }
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds any 2d items to \a pScene.
*/
void CComponent::addItems(C3DScene* pScene)
{
}

//-------------------------------------------------------------------------------------------------

void CComponent::updateContext(CRenderContext* pContext)
{
    // Call the updateContext method for each child component
    foreach (QSP<CComponent> pChild, m_vChildren)
    {
        pChild->updateContext(pContext);
    }
}

//-------------------------------------------------------------------------------------------------

void CComponent::updateItems(C3DScene* pScene)
{
    // Call the updateItems method for each child component
    foreach (QSP<CComponent> pChild, m_vChildren)
    {
        pChild->updateItems(pScene);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Renders the component. Meant to be overridden as the base implementation does nothing. \br\br
    \a pContext is the rendering context.
*/
void CComponent::paint(CRenderContext* pContext)
{
    // Without geometry, nothing gets drawn
}

//-------------------------------------------------------------------------------------------------

/*!
    Called after the component has been painted.
*/
void CComponent::postPaint(CRenderContext* pContext)
{
    // Call the paint and postPaint method for each child component
    foreach (QSP<CComponent> pChild, m_vChildren)
    {
        pChild->paint(pContext);
        pChild->postPaint(pContext);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the local bounds of the component.
*/
CBoundingBox CComponent::bounds()
{
    // Without geometry, return an empty box

    return CBoundingBox();
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the world bounds of the component.
*/
CBoundingBox CComponent::worldBounds()
{
    // Without geometry, return an empty box

    return CBoundingBox();
}

//-------------------------------------------------------------------------------------------------

/*!
    Checks if \a ray intersects this component.
*/
RayTracingResult CComponent::intersect(Math::CRay3 ray)
{
    // Without geometry, return infinity

    return RayTracingResult(Q3D_INFINITY);
}

//-------------------------------------------------------------------------------------------------

/*!
    Flips all normals of the component. Meaningful on mesh objects.
*/
void CComponent::flipNormals()
{
    // Call the flipNormals method for each child component
    foreach (QSP<CComponent> pChild, m_vChildren)
    {
        pChild->flipNormals();
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Transform all vertices in the component using \a matrix. Meaningful on mesh objects.
*/
void CComponent::transformVertices(const Math::CMatrix4& matrix)
{
    // Call the transformVertices method for each child component
    foreach (QSP<CComponent> pChild, m_vChildren)
    {
        pChild->transformVertices(matrix);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Updates this component using \a dDeltaTimeS, which is the elapsed seconds since the last frame.
*/
void CComponent::update(double dDeltaTimeS)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Does post update work on the component using \a dDeltaTimeS, which is the elapsed seconds since the last frame.
*/
void CComponent::postUpdate(double dDeltaTimeS)
{
    // Compute the transform matrix
    computeWorldTransform();

    foreach (QSP<CComponent> pChild, m_vChildren)
    {
        pChild->update(dDeltaTimeS);
    }

    foreach (QSP<CComponent> pChild, m_vChildren)
    {
        pChild->postUpdate(dDeltaTimeS);
    }
}

//-------------------------------------------------------------------------------------------------

void CComponent::updateTexture(CTexture* pTexture, double dDeltaTime)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the world transform matrix of the component.
*/
CMatrix4 CComponent::worldTransform() const
{
    return m_mWorldTransform;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the inverse of the world transform matrix of the component.
*/
CMatrix4 CComponent::worldTransformInverse() const
{
    return m_mWorldTransformInverse;
}

//-------------------------------------------------------------------------------------------------

Math::CMatrix4 CComponent::previousWorldTransform() const
{
    return m_mPreviousWorldTransform;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the geocentric location of the component.
*/
CVector3 CComponent::worldPosition() const
{
    return m_mWorldTransform * CVector3(0.0, 0.0, 0.0);
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the ECEF (Earth-Centered Earth-Fixed) euler rotation of the component.
*/
CVector3 CComponent::worldRotation() const
{
    CVector3 vVec1(0.0, 0.0, 0.0);
    CVector3 vVec2(0.0, 0.0, 1.0);
    CVector3 vVec3(1.0, 0.0, 0.0);
    CVector3 vVec4(0.0, 1.0, 0.0);

    vVec1 = m_mWorldTransform * vVec1;
    vVec2 = m_mWorldTransform * vVec2;
    vVec3 = m_mWorldTransform * vVec3;
    vVec4 = m_mWorldTransform * vVec4;

    CAxis aAxis(vVec2 - vVec1, vVec4 - vVec1, vVec3 - vVec1);

    return aAxis.eulerAngles();
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns a vector that points to what is considered 'forward' of the component (+Z).
*/
CVector3 CComponent::worldDirection() const
{
    CVector3 vVec1(0.0, 0.0, 0.0);
    CVector3 vVec2(0.0, 0.0, 1.0);

    return ((m_mWorldTransform * vVec2) - (m_mWorldTransform * vVec1)).normalized();
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the world scale of the component.
*/
CVector3 CComponent::worldScale() const
{
    return m_vScale * m_vAnimScale;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the working status of the component. \br\br
    The status property can be used to emulate hardware that can is malfunctioning, thus producing breakdowns.
*/
double CComponent::status() const
{
    return m_dStatus;
}

//-------------------------------------------------------------------------------------------------

/*!
    Saves the current transform properties of the component.
*/
void CComponent::saveTransform()
{
    m_gSavedGeoloc      = m_gGeoloc;
    m_vSavedPosition    = m_vPosition;
    m_vSavedRotation    = m_vRotation;
    m_vSavedScale       = m_vScale;
}

//-------------------------------------------------------------------------------------------------

/*!
    Restores the saved transform properties of the component.
*/
void CComponent::loadTransform()
{
    m_gGeoloc       = m_gSavedGeoloc;
    m_vPosition     = m_vSavedPosition;
    m_vRotation     = m_vSavedRotation;
    m_vScale        = m_vSavedScale;

    if (isRootObject() && m_bInheritTransform)
    {
        m_vECEFRotation = toECEFRotation(m_vRotation);
    }
    else
    {
        m_vECEFRotation = m_vRotation;
    }

    computeWorldTransform();
}

//-------------------------------------------------------------------------------------------------

/*!
    Computes the world transform matrix of the component, taking into account parent transform.
*/
void CComponent::computeWorldTransform()
{
    CMatrix4 mRotation;
    CMatrix4 mAnimRotation;
    CMatrix4 mPosition;
    CMatrix4 mAnimPosition;

    // Create the original rotation matrix
    mRotation = CMatrix4::makeRotation(m_vECEFRotation);

    // Create the animated rotation matrix
    mAnimRotation = CMatrix4::makeRotation(m_vAnimRotation);

    // Create the original position matrix
    mPosition = CMatrix4::makeTranslation(m_vPosition);

    // Create the animated position matrix
    mAnimPosition = CMatrix4::makeTranslation(m_vAnimPosition);

    // Apply the transform matrices to m_mWorldTransform
    m_mWorldTransform.makeIdentity();

    m_mWorldTransform = m_mWorldTransform * mAnimRotation;
    m_mWorldTransform = m_mWorldTransform * mRotation;
    m_mWorldTransform = m_mWorldTransform * mAnimPosition;
    m_mWorldTransform = m_mWorldTransform * mPosition;

    if (isRootObject() || m_bInheritTransform == false)
    {
        CVector3 vPosition = m_gGeoloc.toVector3();
        mAnimPosition = CMatrix4::makeTranslation(vPosition);
        m_mWorldTransform = m_mWorldTransform * mAnimPosition;
    }

    if (m_pParent && m_bInheritTransform)
    {
        m_mWorldTransform = m_mWorldTransform * m_pParent->m_mWorldTransform;
    }

    // Compute the inverse of the transfom matrix
    m_mWorldTransformInverse = m_mWorldTransform.inverse();
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the root object of this component, meaning the start of the chain.
*/
QSP<CComponent> CComponent::root()
{
    QSP<CComponent> pReturnValue = QSP<CComponent>(this);

    while (pReturnValue->m_pParent)
    {
        pReturnValue = pReturnValue->m_pParent;
    }

    return pReturnValue;
}

//-------------------------------------------------------------------------------------------------

CGeoloc CComponent::geoloc() const
{
    if (isRootObject() || m_bInheritTransform == false)
    {
        return m_gGeoloc;
    }
    else
    {
        return CGeoloc(worldPosition());
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the rotation in the ECEF frame (Earth-centered earth-fixed).
*/
Math::CVector3 CComponent::ECEFRotation() const
{
    return m_vECEFRotation;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the position.
*/
CVector3 CComponent::position() const
{
    if (isRootObject() || m_bInheritTransform == false)
    {
        return m_gGeoloc.toVector3();
    }
    else
    {
        return m_vPosition;
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the rotation.
*/
Math::CVector3 CComponent::rotation() const
{
    return m_vRotation;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the scale.
*/
Math::CVector3 CComponent::scale() const
{
    return m_vScale;
}

//-------------------------------------------------------------------------------------------------

void CComponent::lookAt(CComponent* pTarget)
{
    CVector3 vPosition = pTarget->geoloc().toVector3(geoloc());

    double dY = vPosition.eulerYAngle();

    CMatrix4 mRotationCancelPan = CMatrix4::makeRotation(CVector3(0.0, dY * -1.0, 0.0));

    vPosition = mRotationCancelPan * vPosition;

    double dX = vPosition.eulerXAngle();

    setRotation(CVector3(dX, dY, 0.0));

    computeWorldTransform();
}

//-------------------------------------------------------------------------------------------------

void CComponent::copyTransform(const CComponent* pTarget)
{
    m_mWorldTransform = pTarget->m_mWorldTransform;
}

//-------------------------------------------------------------------------------------------------

/*!
    Dumps this component to \a stream using the indentation value in \a iIdent.
*/
void CComponent::dump(QTextStream& stream, int iIdent)
{
    dumpIdent(stream, iIdent, QString("[CComponent]"));
    dumpIdent(stream, iIdent, QString("Name : %1").arg(m_sName));
    dumpIdent(stream, iIdent, QString("Parent name : %1").arg(m_sParentName));
    dumpIdent(stream, iIdent, QString("Geoloc : %1").arg(m_gGeoloc.toString()));
    dumpIdent(stream, iIdent, QString("ECEF rotation : %1").arg(m_vECEFRotation.toString()));
    dumpIdent(stream, iIdent, QString("Position : %1").arg(m_vPosition.toString()));
    dumpIdent(stream, iIdent, QString("Rotation : %1").arg(m_vRotation.toString()));
    dumpIdent(stream, iIdent, QString("Scale : %1").arg(m_vScale.toString()));
    dumpIdent(stream, iIdent, QString("Animated position : %1").arg(m_vAnimPosition.toString()));
    dumpIdent(stream, iIdent, QString("Animated rotation : %1").arg(m_vAnimRotation.toString()));
    dumpIdent(stream, iIdent, QString("Animated scale : %1").arg(m_vAnimScale.toString()));
    dumpIdent(stream, iIdent, QString("World transform : %1").arg(m_mWorldTransform.toString()));
    dumpIdent(stream, iIdent, QString("Children :"));

    dumpOpenBlock(stream, iIdent);
    foreach (QSP<CComponent> pChild, m_vChildren)
    {
        pChild->dump(stream, iIdent + 1);
    }
    dumpCloseBlock(stream, iIdent);
}

//-------------------------------------------------------------------------------------------------

void CComponent::incComponentCounter(QString sClassName)
{
    if (m_mComponentCounter.contains(sClassName) == false)
    {
        m_mComponentCounter[sClassName] = 0;
    }

    m_mComponentCounter[sClassName]++;
}

//-------------------------------------------------------------------------------------------------

void CComponent::decComponentCounter(QString sClassName)
{
    if (m_mComponentCounter.contains(sClassName) == false)
    {
        m_mComponentCounter[sClassName] = 0;
    }

    m_mComponentCounter[sClassName]--;
}
