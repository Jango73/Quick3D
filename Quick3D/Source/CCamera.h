
#pragma once

#include "quick3d_global.h"

// Qt
#include <QMatrix4x4>

// Fondations
#include "CVector3.h"
#include "CMatrix4.h"
#include "CPlane3.h"

// Application
#include "CQ3DConstants.h"
#include "CBoundingBox.h"
#include "CPhysicalComponent.h"
#include "CGeoZone.h"
#include "IProgressListener.h"

//-------------------------------------------------------------------------------------------------
// D�clarations avanc�es

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

    QVector<CGeoZone>&          m_vZones;               // Les zones de d�tection
    QVector<double>&            m_vDepth;               // La matrice de profondeur
    QVector<char>&              m_vDetection;           // La matrice de d�tection (zones)
    QVector<char>&              m_vEdges;               // La matrice de contours (angles d'incidence)
    QImage&                     m_imgDepthImage;        // L'image de profondeur
    QImage&                     m_imgDetectionImage;    // L'image de d�tection (zones)
    QImage&                     m_imgContourImage;      // L'image de contours (angles d'incidence)

    Math::CVector3              m_vAttitude;            // Vecteur permetant le ressampling
};

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CCamera : public CPhysicalComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Retourne une nouvelle instance de cet objet
    static CComponent* instanciator(C3DScene* pScene);

    //! Constructeur d'apr�s une sc�ne
    CCamera(C3DScene* pScene);

    //! Constructeur de copie
    CCamera(const CCamera& target);

    //! Destructeur
    virtual ~CCamera();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! R�gle le champ de vision
    virtual void setFOV(double dValue) { m_dFOV = dValue; }

    //! R�gle le niveau de zoom normalis� (0.0 -> 1.0)
    virtual void setZoom(double dValue) { m_dZoom = dValue; }

    //! R�gle le niveau de focalisation normalis� (0.0 -> 1.0)
    virtual void setFocus(double dValue) { m_dFocus = dValue; }

    //! R�gle le niveau de gain normalis� (0.0 -> 1.0)
    virtual void setGain(double dValue) { m_dGain = dValue; }

    //! R�gle la distance minimum de visibilit� des objets (sert � la matrice de projection)
    virtual void setMinDistance(double dValue) { m_dMinDistance = dValue; }

    //! R�gle la distance maximum de visibilit� des objets (sert � la matrice de projection)
    virtual void setMaxDistance(double dValue) { m_dMaxDistance = dValue; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Retourne le champ de vision
    virtual double getFOV() const { return m_dFOV; }

    //! Retourne le niveau de zoom
    virtual double getZoom() const { return m_dZoom; }

    //! Retourne le niveau de focalisation normalis� (0.0 -> 1.0)
    virtual double getFocus() const { return m_dFocus; }

    //! Retourne le niveau de gain normalis� (0.0 -> 1.0)
    virtual double getGain() const { return m_dGain; }

    //! Retourne la distance minimum de visibilit� des objets (sert � la matrice de projection)
    virtual double getMinDistance() const { return m_dMinDistance; }

    //! Retourne la distance maximum de visibilit� des objets (sert � la matrice de projection)
    virtual double getMaxDistance() const { return m_dMaxDistance; }

    //-------------------------------------------------------------------------------------------------
    // M�thodes h�rit�es
    //-------------------------------------------------------------------------------------------------

    //! Retourne le nom de classe de cet objet
    virtual QString getClassName() const { return ClassName_CCamera; }

    //! Retourne vrai si cet objet est une cam�ra
    virtual bool isCamera() const { return true; }

    //! Charge les param�tres de cet objet d'apr�s le noeud XML fourni
    virtual void loadParameters(CXMLNode xComponent);

    //-------------------------------------------------------------------------------------------------
    // M�thodes de contr�le
    //-------------------------------------------------------------------------------------------------

    //! Op�rateur d'assignation
    CCamera& operator = (const CCamera& target);

    //! G�n�re un rendu de la sc�ne (pScene) � travers le viewport (pViewport)
    //! Le FOV peut �tre forc� � une grande valeur (bForceWideFOV) ou une petite (bForceSmallFOV)
    virtual void render(C3DScene* pScene, CViewport* pViewport, bool bForceWideFOV, bool bForceSmallFOV, bool bForceIR);

    //! G�n�re une matrice de profondeur en utilisant la technique du ray-tracing
    virtual void renderDepth_RayTraced(
            C3DScene* pScene,
            double dMaxDistance,
            CPanoramicMatrixParams& tParams,
            IProgressListener* pProgressListener
            );

    //! G�n�re une matrice de profondeur en utilisant la technique du cube-mapping
    virtual void renderDepth_CubeMapped(
            C3DScene* pScene,
            double dMaxDistance,
            CPanoramicMatrixParams& tParams,
            IProgressListener* pProgressListener
            );

    //! Calcul le frustum (pyramide de visualisation) de la cam�ra
    void computeFrustum(double dFOV, double dAspectRatio, double dMinDistance, double dMaxDistance);

    //! Retourne vrai si le frustum contient partiellement la sph�re d�finie par vPosition et dRadius
    bool contains(const Math::CVector3& vPosition, double dRadius) const;

    //! Retourne vrai si le frustum contient partiellement la bo�te englobante d�finie par bounds
    bool contains(const CBoundingBox& bounds) const;

    //! Retourne une matrice de projection au format Qt
    static QMatrix4x4 getQtProjectionMatrix(double dFOV, double dAspectRatio, double dMinDistance, double dMaxDistance);

    //! Retourne une matrice de transformation au format Qt
    static QMatrix4x4 getQtMatrix(Math::CVector3 vPosition, Math::CVector3 vRotation);

    //! Retourne une matrice de transformation utilis�e par les m�thodes internes
    static Math::CMatrix4 getInternalMatrix(Math::CVector3 vPosition, Math::CVector3 vRotation);

    //! Retourne une cat�gorie de d�tection pour le point vPoint en utilisant les zones localPolygons
    CGeoZone::EGeoZoneFlag categorizePointFromZones(const QVector<CGeoZone>& vZones, Math::CVector2 vPoint);

    //-------------------------------------------------------------------------------------------------
    // Propri�t�s
    //-------------------------------------------------------------------------------------------------

protected:

    double                  m_dFOV;
    double                  m_dZoom;
    double                  m_dFocus;
    double                  m_dGain;
    double                  m_dMinDistance;
    double                  m_dMaxDistance;
    QVector<Math::CPlane3>  m_pFrustumPlanes;
};
