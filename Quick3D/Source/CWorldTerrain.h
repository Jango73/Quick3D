
#pragma once

// Qt
#include <QImage>
#include <QDateTime>

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CComponent.h"
#include "CWorldChunk.h"
#include "CHeightField.h"
#include "CGeometryGenerator.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CBuildings
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    CBuildings()
        : m_dSpread(0.0)
        , m_pFunction(nullptr)
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

class QUICK3D_EXPORT CWorldTerrain : public CComponent, public CHeightField
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instantiator(C3DScene* pScene);

    //!
    CWorldTerrain(C3DScene* pScene, CGeoloc gCameraPosition = CGeoloc(), CHeightField* pHeights = nullptr, bool bGenerateNow = false);

    //!
    virtual ~CWorldTerrain();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setTerrainResolution(int value);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    CHeightField* heights() { return m_pHeights; }

    //!
    int levels() const { return m_iLevels; }

    //!
    CMaterial* material() { return m_pMaterial.data(); }

    //!
    QVector<QSP<CComponent> >& generators() { return m_vGenerators; }

    //-------------------------------------------------------------------------------------------------
    // Overridden methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CWorldTerrain; }

    //!
    virtual void loadParameters(const QString& sBaseFile, const CXMLNode& xComponent) Q_DECL_OVERRIDE;

    //! Solves the links of this object
    virtual void solveLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //! Deletes this object's links
    virtual void clearLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //!
    virtual void paint(CRenderContext* pContext) Q_DECL_OVERRIDE;

    //! Updates this object using the elapsed time since last update
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //! Updates this object using the elapsed time since last update
    virtual void postUpdate(double dDeltaTime) Q_DECL_OVERRIDE;

    //!
    virtual double getHeightAt(const CGeoloc& gPosition, double* pRigidness = nullptr);

    //!
    virtual void flatten(const CGeoloc& gPosition, double dRadius_m) Q_DECL_OVERRIDE;

    //! Ray intersection
    virtual Math::RayTracingResult intersect(Math::CRay3 ray) Q_DECL_OVERRIDE;

    //! Dumps contents to a stream
    virtual void dump(QTextStream& stream, int iIdent);

    //-------------------------------------------------------------------------------------------------
    // Méthodes protégés
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    void buildRoot(CRenderContext* pContext);

    //!
    bool enoughDetail(QSP<CWorldChunk> pChunk, CRenderContext* pContext, int iLevel);

    //!
    void paintRecurse(QVector<QSP<CWorldChunk> >& vChunkCollect, CRenderContext* pContext, QSP<CWorldChunk> pChunk, int iLevel, bool bForcePaint);

    //!
    void buildRecurse(QSP<CWorldChunk> pChunk, CRenderContext* pContext, int iLevel);

    //!
    double getHeightAtRecurse(const CGeoloc& gPosition, QSP<CWorldChunk> pChunk, double* pRigidness = nullptr);

    //!
    void collectGarbage();

    //!
    void collectGarbageRecurse(QSP<CWorldChunk> pChunk);

    //! Ray intersection
    Math::RayTracingResult intersectRecurse(QSP<CWorldChunk> pChunk, const Math::CRay3& ray) const;

    //!
    void flattenRecurse(QSP<CWorldChunk> pChunk, const CGeoloc& gPosition, double dRadius_m);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    bool                        m_bGenerateNow;
    CHeightField*               m_pHeights;
    int                         m_iLevels;
    int                         m_iTerrainResolution;
    CXMLNode                    m_xParameters;

    // Shared data

    QSP<CWorldChunk>            m_pRoot;
    QSP<CMaterial>              m_pMaterial;
    QVector<QSP<CComponent> >   m_vGenerators;
};
