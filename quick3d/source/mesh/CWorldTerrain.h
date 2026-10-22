
#pragma once

// Qt
#include <QImage>
#include <QDateTime>
#include <QString>

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

//! Identity of one chunk collected by the last paint of a layer pass, for engine snapshots
struct SCollectedChunkInfo
{
    QString address;
    double latitude;
    double longitude;
    double sizeLatitude;
    double sizeLongitude;

    //! Returns the chunk identity as a JSON object
    QString snapshotJson() const
    {
        return QString("{\"address\":\"%1\",\"latitude\":%2,\"longitude\":%3,\"sizelatitude\":%4,\"sizelongitude\":%5}")
            .arg(address)
            .arg(latitude, 0, 'g', 10)
            .arg(longitude, 0, 'g', 10)
            .arg(sizeLatitude, 0, 'g', 10)
            .arg(sizeLongitude, 0, 'g', 10);
    }
};

//-------------------------------------------------------------------------------------------------

//! Readiness of one quadtree level on the camera path, for engine snapshots
struct SChunkPathInfo
{
    int level;
    double latitude;
    double longitude;
    double sizeLatitude;
    double sizeLongitude;
    bool selfReady;
    int childCount;
    int drawableChildren;
    bool stay;

    //! Returns the level readiness as a JSON object
    QString snapshotJson() const
    {
        return QString("{\"level\":%1,\"latitude\":%2,\"longitude\":%3,\"sizelatitude\":%4,\"sizelongitude\":%5,"
            "\"selfready\":%6,\"children\":%7,\"drawable\":%8,\"stay\":%9}")
            .arg(level)
            .arg(latitude, 0, 'g', 10)
            .arg(longitude, 0, 'g', 10)
            .arg(sizeLatitude, 0, 'g', 10)
            .arg(sizeLongitude, 0, 'g', 10)
            .arg(selfReady == true ? "true" : "false")
            .arg(childCount)
            .arg(drawableChildren)
            .arg(stay == true ? "true" : "false");
    }
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

    //! Returns a JSON document with the last collect counts and the quadtree readiness on the camera path
    virtual QString snapshotJson() Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    void buildRoot(CRenderContext* pContext);

    //!
    bool enoughDetail(QSP<CWorldChunk> pChunk, CRenderContext* pContext, int iLevel);

    //! Returns \c true when \a iLevel is enough detail for \a pChunk seen from \a gCamera
    bool enoughDetail(QSP<CWorldChunk> pChunk, const CGeoloc& gCamera, int iLevel);

    //! Records the quadtree readiness on the path from the root to \a gCamera, retained for snapshots
    void recordCameraPath(const CGeoloc& gCamera);

    //! Collects the chunks to draw, one level per zone following the LOD policy
    void paintRecurse(QVector<QSP<CWorldChunk> >& vChunkCollect, CRenderContext* pContext, QSP<CWorldChunk> pChunk, int iLevel);

    //! Returns true when \a pChunk lies fully outside the range of the layer pass in \a pContext and contributes nothing to it
    bool skipChunkForLayerPass(QSP<CWorldChunk> pChunk, CRenderContext* pContext);

    //! Queues chunk detail work once its terrain is ready
    void requestChunkDetailWork(QSP<CWorldChunk> pChunk);

    //!
    void buildRecurse(QSP<CWorldChunk> pChunk, CRenderContext* pContext, int iLevel);

    //! Creates the terrain and water patches of \a pChunk when missing
    void ensureChunkTerrain(QSP<CWorldChunk> pChunk, int iLevel);

    //! Tells whether \a gCamera stands inside the ground extent of \a pChunk
    bool isCoveringChunk(QSP<CWorldChunk> pChunk, const CGeoloc& gCamera) const;

    //!
    double getHeightAtRecurse(const CGeoloc& gPosition, QSP<CWorldChunk> pChunk, double* pRigidness = nullptr);

    //! Destroys the terrain, water and height field of a chunk and every node below it
    void destroyChunkSubtree(QSP<CWorldChunk> pChunk);

    //!
    void collectGarbage();

    //!
    void collectGarbageRecurse(QSP<CWorldChunk> pChunk, int iLevel);

    //! Registers ready chunk grids as physics height fields, drops stale ones
    void syncPhysicsHeightfields();

    //!
    void syncPhysicsHeightfieldsRecurse(QSP<CWorldChunk> pChunk);

    //! Removes the physics height field of \a pChunk when registered
    void removeChunkHeightfield(QSP<CWorldChunk> pChunk);

    //! Returns the registered height field id of \a pChunk, -1 when none
    int chunkHeightfieldIndex(QSP<CWorldChunk> pChunk) const;

    //! Records \a iIndex as the height field of \a pChunk, -1 unregisters without removal
    void setChunkHeightfieldIndex(QSP<CWorldChunk> pChunk, int iIndex);

    //! Removes every registered chunk height field from the physics world
    void clearPhysicsHeightfields();

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
    QMap<const CWorldChunk*, int> m_mPhysicsHeightfields; ///< Chunk to height field ids, owned here not in the mesh
    QMap<int, QVector<SCollectedChunkInfo> > m_mLastCollectedChunks; ///< Layer pass to identities of its last painted chunks
    QMap<int, int> m_mLastOverlapCount; ///< Layer pass to same-pass ground overlaps in its last painted chunks
    QVector<SChunkPathInfo>     m_vLastCameraPath; ///< Quadtree readiness on the camera path, recorded by the last paint
    CGeoloc                     m_gLastCameraGeoloc; ///< Camera position retained from the last paint, for snapshots
};
