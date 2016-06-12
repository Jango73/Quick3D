
#pragma once

// Qt
#include <QImage>
#include <QDateTime>

// Application
#include "CQ3DConstants.h"
#include "CComponent.h"
#include "CWorldChunk.h"
#include "CHeightField.h"
#include "CGenerateFunction.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CVegetation
{
public:

    enum EVegetationType
    {
        evtNone,
        evtTree,
        evtBush
    };

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    CVegetation()
        : m_eType(evtNone)
        , m_dSpread(0.0)
        , m_pFunction(NULL)
        , m_pMesh(NULL)
        , m_pMaterial(NULL)
    {
    }

    CVegetation(EVegetationType eType, double dSpread, CGenerateFunction* pFunction, CMeshInstance* pMesh, CMaterial* pMaterial)
        : m_eType(eType)
        , m_dSpread(dSpread)
        , m_pFunction(pFunction)
        , m_pMesh(pMesh)
        , m_pMaterial(QSharedPointer<CMaterial>(pMaterial))
    {
    }

    virtual ~CVegetation()
    {
        if (m_pFunction != NULL) delete m_pFunction;
        if (m_pMesh != NULL) delete m_pMesh;
    }

    EVegetationType             m_eType;
    double                      m_dSpread;
    CGenerateFunction*          m_pFunction;
    CMeshInstance*              m_pMesh;
    QSharedPointer<CMaterial>   m_pMaterial;
};

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CBuildings
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    CBuildings()
        : m_dSpread(0.0)
        , m_pFunction(NULL)
    {
    }

    CBuildings(double dSpread, CGenerateFunction* pFunction, QVector<CMeshInstance*> vMeshes)
        : m_dSpread(dSpread)
        , m_pFunction(pFunction)
        , m_vMeshes(vMeshes)
    {
    }

    virtual ~CBuildings()
    {
        delete m_pFunction;

        foreach (CMeshInstance* pMesh, m_vMeshes)
        {
            delete pMesh;
        }
    }

    double                      m_dSpread;
    CGenerateFunction*          m_pFunction;
    QVector<CMeshInstance*>     m_vMeshes;
};

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CAutoTerrain : public CComponent, public CHeightField
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instanciator(C3DScene* pScene);

    //!
    CAutoTerrain(C3DScene* pScene, CGeoloc gCameraPosition = CGeoloc(), CHeightField* pHeights = NULL, bool bGenerateNow = false);

    //!
    virtual ~CAutoTerrain();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setTerrainResolution(int value);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    CHeightField* getHeights() { return m_pHeights; }

    //!
    QVector<CVegetation*>& getVegetation() { return m_vVegetation; }

    //!
    CMaterial* getMaterial() { return m_pMaterial.data(); }

    //-------------------------------------------------------------------------------------------------
    // Méthodes héritées
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CAutoTerrain; }

    //!
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent);

    //!
    virtual void paint(CRenderContext* pContext);

    //! Méthode de mise à jour avec temps delta depuis la dernière itération
    virtual void update(double dDeltaTime);

    //! Méthode de mise à jour avec temps delta depuis la dernière itération
    virtual void postUpdate(double dDeltaTime);

    //!
    virtual double getHeightAt(const CGeoloc& gPosition, double* pRigidness = NULL);

    //!
    virtual void flatten(const CGeoloc& gPosition, double dRadius);

    //! Calcul d'intersection avec un rayon
    virtual Math::RayTracingResult intersect(Math::CRay3 ray);

    //! Dump du contenu dans un flux
    virtual void dump(QTextStream& stream, int iIdent);

    //-------------------------------------------------------------------------------------------------
    // Méthodes protégés
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    void buildRoot();

    //!
    void paintRecurse(QVector<CWorldChunk*>& vChunkCollect, CRenderContext* pContext, CWorldChunk* pChunk, int iLevel, bool bForcePaint);

    //!
    void buildRecurse(CWorldChunk* pChunk, CRenderContext* pContext, int iLevel);

    //!
    double getHeightAtRecurse(const CGeoloc& gPosition, CWorldChunk* pChunk, double* pRigidness = NULL);

    //!
    void collectGarbage();

    //!
    void collectGarbageRecurse(CWorldChunk* pChunk);

    //!
    void readVegetationParameters(const QString& sBaseFile);

    //!
    void readBuildingParameters(const QString& sBaseFile);

    //! Calcul d'intersection avec un rayon
    Math::RayTracingResult intersectRecurse(CWorldChunk* pChunk, const Math::CRay3& ray) const;

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    bool                        m_bGenerateNow;
    CWorldChunk*                m_pRoot;
    CHeightField*               m_pHeights;
    int                         m_iLevels;
    int                         m_iTerrainResolution;
    CXMLNode                    m_xParameters;
    QSharedPointer<CMaterial>   m_pMaterial;
    QVector<CVegetation*>       m_vVegetation;
};
