
#pragma once

// Qt
#include <QMatrix4x4>

// Application
#include "quick3d_global.h"
#include "CVector3.h"
#include "CMatrix4.h"
#include "CPlane3.h"
#include "CQ3DConstants.h"
#include "CBoundingBox.h"
#include "CPhysicalComponent.h"
#include "CGeoZone.h"
#include "IProgressListener.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;
class CViewport;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CPanoramicMatrixParams
{
public:

    CPanoramicMatrixParams(
            QVector<CGeoZone>& vZones,
            QVector<double>& vDepth,
            QVector<char>& vDetection,
            QVector<char>& vEdges,
            QImage& imgDepthImage,
            QImage& imgDetectionImage,
            QImage& imgContourImage,
            Math::CVector3 vAttitude = Math::CVector3(0.0,0.0,0.0)
            )
        : m_sResolution(600, 200)
        , m_dCameraTrueHeadingDegrees(0.0)
        , m_vZones(vZones)
        , m_vDepth(vDepth)
        , m_vDetection(vDetection)
        , m_vEdges(vEdges)
        , m_imgDepthImage(imgDepthImage)
        , m_imgDetectionImage(imgDetectionImage)
        , m_imgContourImage(imgContourImage)
        , m_vAttitude(vAttitude)
    {
    }

    QSize                       m_sResolution;
    double                      m_dCameraTrueHeadingDegrees;
    CGeoloc                     m_gCameraPosition;
    Math::CVector2              m_vStartPanTiltDegrees;
    Math::CVector2              m_vEndPanTiltDegrees;
    Math::CVector2              m_vPanTiltOffsetDegrees;
    QString                     m_sPathToSTRMData;
    QString                     m_sPathToBILData;

    QVector<CGeoZone>&          m_vZones;               // The detection zones
    QVector<double>&            m_vDepth;               // The depth matrix
    QVector<char>&              m_vDetection;           // The zone matrix
    QVector<char>&              m_vEdges;               // The edge matrix (angles of incidence between camera and terrain)
    QImage&                     m_imgDepthImage;        // The depth image
    QImage&                     m_imgDetectionImage;    // The zone image
    QImage&                     m_imgContourImage;      // The edge image

    Math::CVector3              m_vAttitude;            // Used for resampling
};

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CCamera : public CPhysicalComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Returns a new instance of this class
    static CComponent* instantiator(C3DScene* pScene);

    //! Constructor using a scene
    CCamera(C3DScene* pScene);

    //! Copy constructor
    CCamera(const CCamera& target);

    //! Destructor
    virtual ~CCamera();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! Sets the field of view in degrees
    virtual void setVerticalFOV(double dValue) { m_dFOV = dValue; }

    //! Sets the normalized focus (0.0 -> 1.0)
    virtual void setFocus(double dValue) { m_dFocus = dValue; }

    //! Sets the normalized gain (0.0 -> 1.0)
    virtual void setGain(double dValue) { m_dGain = dValue; }

    //! Sets the minimum visibility distance of objects (used by projection matrix)
    void setMinDistance(double dValue) { m_dMinDistance = dValue; }

    //! Sets the maximum visibility distance of objects (used by projection matrix)
    void setMaxDistance(double dValue) { m_dMaxDistance = dValue; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns the field of view in degrees
    virtual double verticalFOV() const { return m_dFOV; }

    //! Returns the normalized focus (0.0 -> 1.0)
    virtual double focus() const { return m_dFocus; }

    //! Returns the normalized gain (0.0 -> 1.0)
    virtual double gain() const { return m_dGain; }

    //! Returns the minimum visibility distance of objects (used by projection matrix)
    double minDistance() const { return m_dMinDistance; }

    //! Returns the maximum visibility distance of objects (used by projection matrix)
    double maxDistance() const { return m_dMaxDistance; }

    //-------------------------------------------------------------------------------------------------
    // Overridden methods
    //-------------------------------------------------------------------------------------------------

    //! Returns this object's class name
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CCamera; }

    //! Returns \c true if this object is a camera
    virtual bool isCamera() const { return true; }

    //! Loads this object's parameters using the provided XML node
    virtual void loadParameters(const QString& sBaseFile, const CXMLNode& xComponent) Q_DECL_OVERRIDE;

    //! Dumps contents to a stream
    virtual void dump(QTextStream& stream, int iIdent) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Assign operator
    CCamera& operator = (const CCamera& target);

    //! Renders the scene in the provided viewport
    //! The FOV may be overridden using bForceWideFOV or bForceSmallFOV
    virtual void render(C3DScene* pScene, CViewport* pViewport, bool bForceWideFOV, bool bForceSmallFOV, bool bForceIR, bool bOverlook);

    //! Generates a depth matrix using ray tracing technique
    virtual void renderDepth_RayTraced(
            C3DScene* pScene,
            double dMaxDistance,
            CPanoramicMatrixParams& tParams,
            IProgressListener* pProgressListener
            );

    //! Generates a depth matrix using cube mapping technique
    virtual void renderDepth_CubeMapped(
            C3DScene* pScene,
            double dMaxDistance,
            CPanoramicMatrixParams& tParams,
            IProgressListener* pProgressListener
            );

    //! Computes the frustum (visualization pyramid) of the camera
    void computeFrustum(double dVerticalFOV, double dAspectRatio, double dMinDistance, double dMaxDistance);

    //! Returns \c true if the frustum partially contains the sphere defined by \a vPosition and \a dRadius
    bool contains(const Math::CVector3& vPosition, double dRadius) const;

    //! Returns \c true if the frustum partially contains the box defined by \a bounds
    bool contains(const CBoundingBox& bounds) const;

    //! Returns a projection Qt matrix
    static QMatrix4x4 getQtProjectionMatrix(double dVerticalFOV, double dAspectRatio, double dMinDistance, double dMaxDistance);

    //! Returns a model view Qt matrix
    static QMatrix4x4 getQtCameraMatrix(Math::CVector3 vPosition, Math::CVector3 vRotation);

    //! Returns a projection matrix used internally
    static Math::CMatrix4 getInternalProjectionMatrix(double dVerticalFOV, double dAspectRatio, double dMinDistance, double dMaxDistance);

    //! Returns a model view matrix used internally
    static Math::CMatrix4 getInternalCameraMatrix(Math::CVector3 vPosition, Math::CVector3 vRotation);

    //! Returns a detection category for \a vPoint using polygons in \a vZones
    CGeoZone::EGeoZoneFlag categorizePointFromZones(const QVector<CGeoZone>& vZones, Math::CVector2 vPoint);

    //!
    Math::CRay3 screenPointToWorldRay(CViewport* pViewport, Math::CVector2 vPoint);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    double                  m_dFOV;
    double                  m_dFocus;
    double                  m_dGain;
    double                  m_dMinDistance;
    double                  m_dMaxDistance;
    QVector<Math::CPlane3>  m_pFrustumPlanes;
};
