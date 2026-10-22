
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
#include "C3DSceneStatistics.h"

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

//! Outcome of one mesh geometry paint test during a layer pass, retained for engine snapshots
struct SMeshDrawInfo
{
    QString container;
    QString name;
    int layer;
    double distance;
    double centerX;
    double centerY;
    double centerZ;
    double radius;
    double boxMinX;
    double boxMinY;
    double boxMinZ;
    double boxMaxX;
    double boxMaxY;
    double boxMaxZ;
    double maxDistance;
    bool frustum;
    bool inRange;
    bool drawn;

    //! Returns the mesh draw outcome as a JSON object
    QString snapshotJson() const
    {
        return QString("{\"container\":\"%1\",\"name\":\"%2\",\"layer\":%3,\"distance\":%4,"
            "\"center\":[%5,%6,%7],\"radius\":%8,\"boxmin\":[%9,%10,%11],\"boxmax\":[%12,%13,%14],\"maxdistance\":%15,"
            "\"frustum\":%16,\"inrange\":%17,\"drawn\":%18}")
            .arg(container)
            .arg(name)
            .arg(layer)
            .arg(distance, 0, 'g', 10)
            .arg(centerX, 0, 'g', 10).arg(centerY, 0, 'g', 10).arg(centerZ, 0, 'g', 10)
            .arg(radius, 0, 'g', 10)
            .arg(boxMinX, 0, 'g', 10).arg(boxMinY, 0, 'g', 10).arg(boxMinZ, 0, 'g', 10)
            .arg(boxMaxX, 0, 'g', 10).arg(boxMaxY, 0, 'g', 10).arg(boxMaxZ, 0, 'g', 10)
            .arg(maxDistance, 0, 'g', 10)
            .arg(frustum == true ? "true" : "false")
            .arg(inRange == true ? "true" : "false")
            .arg(drawn == true ? "true" : "false");
    }
};

//-------------------------------------------------------------------------------------------------

//! Last painted frame info for one layer pass, retained for engine snapshots
struct SLayerFrameInfo
{
    int layer;
    double nearPlane;
    double farPlane;
    C3DSceneStatistics statistics;
    QVector<SMeshDrawInfo> draws;

    //! Returns the layer frame info as a JSON object
    QString snapshotJson() const
    {
        QString sDraws = "[";

        bool bFirst = true;

        foreach (const SMeshDrawInfo& tDraw, draws)
        {
            if (bFirst == false) sDraws += ",";
            bFirst = false;

            sDraws += tDraw.snapshotJson();
        }

        sDraws += "]";

        return QString("{\"layer\":%1,\"near\":%2,\"far\":%3,\"statistics\":%4,\"draws\":%5}")
            .arg(layer)
            .arg(nearPlane, 0, 'g', 10)
            .arg(farPlane, 0, 'g', 10)
            .arg(statistics.snapshotJson())
            .arg(sDraws);
    }
};

class QUICK3D_EXPORT CCamera : public CPhysicalComponent
{
public:

    //! Maximum camera pitch in degrees, prevents flipping upside down
    static constexpr double MaxPitchDegrees = 89.0;

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
    virtual ~CCamera() Q_DECL_OVERRIDE;

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

    //! Sets the rotation as euler angles, pitch is limited to +- MaxPitchDegrees
    virtual void setRotation(Math::CVector3 vRotation) Q_DECL_OVERRIDE;

    //! Returns the rotation limited to +- MaxPitchDegrees on the pitch (X) axis
    static Math::CVector3 limit(Math::CVector3 vRotation);

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

    //! Returns the layer painted by the current render pass (RENDER_LAYER_CLOSE/FAR), -1 outside a layer pass
    int currentPaintLayer() const { return m_iCurrentPaintLayer; }

    //! Returns the far plane of the close layer conditioned for the current frame
    double closeLayerFar() const { return m_dCloseLayerFar; }

    //! Returns the per-layer info retained from the last painted frame
    QVector<SLayerFrameInfo> lastLayerFrames() const { return m_vLastLayerFrames; }

    //! Records one mesh geometry paint outcome for the current layer pass, retained for snapshots
    void recordMeshDraw(const SMeshDrawInfo& tDraw)
    {
        if (m_vCurrentMeshDraws.count() < 2048)
        {
            m_vCurrentMeshDraws.append(tDraw);
        }
    }

    //-------------------------------------------------------------------------------------------------
    // Overridden methods
    //-------------------------------------------------------------------------------------------------

    //! Returns this object's class name
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CCamera; }

    //! Returns \c true if this object is a camera
    virtual bool isCamera() const Q_DECL_OVERRIDE { return true; }

    //! Loads this object's parameters using the provided XML node
    virtual void loadParameters(const QString& sBaseFile, const CXMLNode& xComponent) Q_DECL_OVERRIDE;

    //! Dumps contents to a stream
    virtual void dump(QTextStream& stream, int iIdent) Q_DECL_OVERRIDE;

    //! Returns a JSON document with the camera state and the per-layer info of the last painted frame
    virtual QString snapshotJson() Q_DECL_OVERRIDE;

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
    int                     m_iCurrentPaintLayer;
    double                  m_dMinDistance;
    double                  m_dMaxDistance;
    double                  m_dCloseLayerFar;
    QVector<Math::CPlane3>  m_pFrustumPlanes;
    QVector<SLayerFrameInfo> m_vLastLayerFrames;
    QVector<SMeshDrawInfo>  m_vCurrentMeshDraws; ///< Mesh outcomes of the running layer pass
};
