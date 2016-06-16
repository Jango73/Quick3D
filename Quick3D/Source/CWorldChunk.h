
#pragma once

#include "quick3d_global.h"

// Qt
#include <QObject>
#include <QImage>
#include <QMutex>
#include <QMutexLocker>

// Application
#include "CWorker.h"
#include "CComponent.h"
#include "CBoundedMeshInstances.h"
#include "CMaterial.h"
#include "CHeightField.h"
#include "CTerrain.h"
#include "CBox.h"

//-------------------------------------------------------------------------------------------------
// Déclarations avancées

class C3DScene;
class CAutoTerrain;

//-------------------------------------------------------------------------------------------------

enum ETerrainType
{
    ttGround,
    ttWater,
    ttVegetation
};

class QUICK3D_EXPORT CWorldChunk : public CWorker, public CComponent, public CHeightField
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //!
    CWorldChunk(C3DScene* pScene, CAutoTerrain* pAutoTerrain, CHeightField* pContainer);

    //!
    virtual ~CWorldChunk();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    virtual void setGeoloc(CGeoloc gGeoloc);

    //!
    virtual void setSize(CGeoloc value);

    //!
    void setTerrain(CTerrain* value, bool bGenerateNow = false);

    //!
    void setWater(CTerrain* value);

    //!
    void setDistance(double value) { m_dDistance = value; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    CGeoloc getOriginalGeoloc() const { return m_gOriginalGeoloc; }

    //!
    CGeoloc getOriginalSize() const { return m_gOriginalSize; }

    //!
    CGeoloc getSize() const { return m_gSize; }

    //!
    CTerrain* getTerrain() const { return m_pTerrain; }

    //!
    CTerrain* getWater() const { return m_pWater; }

    //!
    bool isOK() const { return m_bOK; }

    //!
    static int getNumWorldChunks() { return m_iNumWorldChunks; }

    //-------------------------------------------------------------------------------------------------
    // Méthodes héritées
    //-------------------------------------------------------------------------------------------------

    //!
    virtual CBoundingBox getBounds();

    //!
    virtual CBoundingBox getWorldBounds();

    //!
    virtual CBoundingBox getBuildWorldBounds();

    //!
    virtual void update(double dDeltaTime);

    //!
    virtual double getHeightAt(const CGeoloc& gPosition, double* pRigidness = NULL);

    //!
    virtual void flatten(const CGeoloc& gPosition, double dRadius);

    //! Calcul d'intersection avec un rayon
    virtual Math::RayTracingResult intersect(Math::CRay3 ray);

    //! Dump du contenu dans un flux
    virtual void dump(QTextStream& stream, int iIdent);

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    //!
    bool operator < (const CWorldChunk& other);

    //!
    void paint(CRenderContext* pContext, ETerrainType eType);

    //!
    bool drawable();

    //!
    bool isEmpty();

    //!
    CBoundingBox createBoundsForTerrain(CGeoloc gPosition, CGeoloc gSize, double dAltitudeFactor = 1.0);

    //!
    void build();

    //!
    void clearTerrain();

    //!
    virtual void work();

    //!
    void placeTree(CGeoloc gPosition, double dRadius, int iVegetIndex);

    //!
    void placeBush(CGeoloc gPosition, double dRadius, int iVegetIndex);

    //!
    void placeBuilding(CGeoloc gPosition, double dRadius, int iBuildingIndex);

    //!
    bool checkPositionFree(CGeoloc gPosition, double dRadius);

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    CAutoTerrain*                   m_pAutoTerrain;
    CHeightField*                   m_pContainer;
    CGeoloc                         m_gOriginalGeoloc;
    CGeoloc                         m_gOriginalSize;
    CGeoloc                         m_gSize;
    CBoundingBox                    m_bWorldBounds;
    CTerrain*                       m_pTerrain;
    CTerrain*                       m_pWater;
    QVector<CBoundedMeshInstances*> m_vMeshes;
    QMap<QString, CMeshGeometry*>   m_vBushMeshes;
    QMutex                          m_mMutex;
    double                          m_dDistance;
    bool                            m_bOK;

    static int                      m_iNumWorldChunks;
};
