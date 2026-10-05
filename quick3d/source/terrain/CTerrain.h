
#pragma once

// qt-plus
#include "CXMLNode.h"
#include "CInterpolator.h"

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CWorker.h"
#include "CComponent.h"
#include "CMeshGeometry.h"
#include "CMaterial.h"
#include "CHeightField.h"
#include "CPerlin.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

/*

    Tile reference system

    m_gSize.Longitude
    <-------------->

    ----------------  ^
    |              |  |
    |              |  |
    |  m_gGeoloc   |  |
    |      .       |  | m_gSize.Latitude
    |              |  |
    |              |  |
    |              |  |
    ----------------  v

*/

//! Classe de stockage d'un mesh de terrain
class QUICK3D_EXPORT CTerrain : public CWorker, public CComponent, public CHeightField
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CTerrain(
            C3DScene* pScene,
            CHeightField* pHeights,
            CGeoloc gOriginalGeoloc,
            CGeoloc gOriginalSize,
            CGeoloc gGeoloc,
            CGeoloc gSize,
            int iPoints,
            int iLevel,
            int iMaxLevel,
            bool bIsWater,
            bool bGenerateNow = false
            );

    //!
    virtual ~CTerrain();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    bool isOK() const { return m_bOK; }

    //!
    bool allHeightsOverSea() const { return m_bAllHeightsOverSea; }

    //!
    int level() { return m_iLevel; }

    //!
    CMeshGeometry* mesh () { return m_pMesh; }

    //!
    const CMeshGeometry* mesh () const { return m_pMesh; }

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CTerrain; }

    //!
    virtual CBoundingBox bounds();

    //!
    virtual CBoundingBox worldBounds();

    //!
    virtual void paint(CRenderContext* pContext) Q_DECL_OVERRIDE;

    //!
    virtual double getHeightAt(const CGeoloc& gPosition, double* pRigidness = nullptr);

    //!
    virtual void flatten(const CGeoloc& gPosition, double dRadius);

    //! Ray intersection
    virtual Math::RayTracingResult intersect(Math::CRay3 ray) Q_DECL_OVERRIDE;

    //!
    virtual void work();

    //! Dumps contents to a stream
    virtual void dump(QTextStream& stream, int iIdent);

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void setMaterial(QSP<CMaterial> pMaterial);

    //!
    int getPointIndexForXZ(int X, int Z) const;

    //!
    int getFaceIndexForVertices(int v1, int v2, int v3, int v4) const;

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    void buildVerticesToFaceMap();

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    CHeightField*                       m_pHeights;
    CGeoloc                             m_gOriginalGeoloc;
    CGeoloc                             m_gOriginalSize;
    CGeoloc                             m_gSize;
    CMeshGeometry*                      m_pMesh;
    QVector<CMeshGeometry*>             m_vSeams;
    QMap<QString, int>                  m_mVerticesToFace;
    int                                 m_iNumPoints;
    int                                 m_iLevel;
    int                                 m_iMaxLevel;
    bool                                m_bAllHeightsOverSea;
    bool                                m_bIsWater;
    bool                                m_bOK;

    static CInterpolator<double>        m_iAltitudes_Sand;
    static CInterpolator<double>        m_iAltitudes_Dirt;
    static CInterpolator<double>        m_iAltitudes_Grass;
    static CInterpolator<double>        m_iAltitudes_Snow;
};
