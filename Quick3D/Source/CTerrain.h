
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Fondations
#include "CXMLNode.h"
#include "Interpolator.h"

// Application
#include "CQ3DConstants.h"
#include "CWorker.h"
#include "CComponent.h"
#include "CMesh.h"
#include "CMaterial.h"
#include "CHeightField.h"
#include "CPerlin.h"

//-------------------------------------------------------------------------------------------------
// Déclarations avancées

class C3DScene;

//-------------------------------------------------------------------------------------------------

/*

    Description du référentiel de la tuile

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
    // Constructeurs et destructeur
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
    bool getAllHeightsOverSea() const { return m_bAllHeightsOverSea; }

    //!
    int getLevel() { return m_iLevel; }

    //!
    CMesh* getMesh () { return m_pMesh; }

    //!
    const CMesh* getMesh () const { return m_pMesh; }

    //!
    static int getNumTerrains() { return m_iNumTerrains; }

    //-------------------------------------------------------------------------------------------------
    // Méthodes héritées
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CTerrain; }

    //!
    virtual CBoundingBox getBounds();

    //!
    virtual CBoundingBox getWorldBounds();

    //!
    virtual void paint(CRenderContext* pContext);

    //!
    virtual double getHeightAt(const CGeoloc& gPosition, double* pRigidness = NULL);

    //!
    virtual void flatten(const CGeoloc& gPosition, double dRadius);

    //! Calcul d'intersection avec un rayon
    virtual Math::RayTracingResult intersect(Math::CRay3 ray);

    //!
    virtual void work();

    //! Dump du contenu dans un flux
    virtual void dump(QTextStream& stream, int iIdent);

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    //!
    void setMaterial(QSharedPointer<CMaterial> pMaterial);

    //!
    int getPointIndexForXZ(int X, int Z) const;

    //!
    int getFaceIndexForVertices(int v1, int v2, int v3, int v4) const;

    //-------------------------------------------------------------------------------------------------
    // Méthodes protégées
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    void buildVerticesToFaceMap();

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    CHeightField*                       m_pHeights;
    CGeoloc                             m_gOriginalGeoloc;
    CGeoloc                             m_gOriginalSize;
    CGeoloc                             m_gSize;
    CMesh*                              m_pMesh;
    QVector<CMesh*>                     m_vSeams;
    QMap<QString, int>                  m_mVerticesToFace;
    int                                 m_iNumPoints;
    int                                 m_iLevel;
    int                                 m_iMaxLevel;
    bool                                m_bAllHeightsOverSea;
    bool                                m_bIsWater;
    bool                                m_bOK;

    static int                          m_iNumTerrains;

    static Math::Interpolator<double>   m_iAltitudes_Sand;
    static Math::Interpolator<double>   m_iAltitudes_Dirt;
    static Math::Interpolator<double>   m_iAltitudes_Grass;
    static Math::Interpolator<double>   m_iAltitudes_Snow;
};
