
#pragma once

// Qt
#include <QImage>
#include <QDateTime>
#include <QString>
#include <QMutex>
#include <QMutexLocker>

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
struct COLLECTED_CHUNK_INFO
{
    QString m_sAddress;
    double  m_dLatitude;
    double  m_dLongitude;
    double  m_dSizeLatitude;
    double  m_dSizeLongitude;

    //! Returns the chunk identity as a node, JSON through CXMLNode
    CXMLNode snapshotNode() const;
};

//-------------------------------------------------------------------------------------------------

//! Readiness of one quadtree level on the camera path, for engine snapshots
struct CHUNK_PATH_INFO
{
    int     m_iLevel;
    double  m_dLatitude;
    double  m_dLongitude;
    double  m_dSizeLatitude;
    double  m_dSizeLongitude;
    bool    m_bSelfReady;
    int     m_iChildCount;
    int     m_iDrawableChildren;
    bool    m_bStay;

    //! Returns the level readiness as a node, JSON through CXMLNode
    CXMLNode snapshotNode() const;
};

//-------------------------------------------------------------------------------------------------

//! Identity of one height field registered in the physics world, for engine snapshots
struct REGISTERED_HEIGHTFIELD_INFO
{
    int     m_iHeightfield;
    double  m_dLatitude;
    double  m_dLongitude;
    double  m_dSizeLatitude;
    double  m_dSizeLongitude;

    //! Returns the height field identity as a node, JSON through CXMLNode
    CXMLNode snapshotNode() const;
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

    //! Returns the per-layer chunk identities retained from the last paint, copy under lock
    QMap<QString, QVector<COLLECTED_CHUNK_INFO> > lastCollectedChunks() const;

    //! Returns the quadtree readiness retained from the last paint, copy under lock
    QVector<CHUNK_PATH_INFO> lastCameraPath() const;

    //! Returns the camera position retained from the last paint, copy under lock
    CGeoloc lastCameraGeoloc() const;

    //! Returns the physics height field identities retained from the last sync, copy under lock
    QVector<REGISTERED_HEIGHTFIELD_INFO> lastHeightfields() const;

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

    //! Returns the last collect counts and the quadtree readiness on the camera path as a node, JSON through CXMLNode
    virtual CXMLNode snapshotNode() Q_DECL_OVERRIDE;

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

    //! Makes \a pChunk drawable and queues its detail work, the single owner of parent-cover creation for build and garbage passes
    void ensureChunkCover(QSP<CWorldChunk> pChunk, int iLevel);

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

    //! Applies the LOD policy bottom-up with the camera retained from the last paint
    void collectGarbageRecurse(QSP<CWorldChunk> pChunk, int iLevel, const CGeoloc& gCamera);

    //! Registers ready chunk grids as physics height fields, drops stale ones
    void syncPhysicsHeightfields();

    //! Registers the finest ready grid below \a pChunk included, true when every quadrant below holds a grid or is empty
    bool syncPhysicsHeightfieldsRecurse(QSP<CWorldChunk> pChunk);

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
    // Constants
    //-------------------------------------------------------------------------------------------------

    //! Water patch resolution relative to the terrain resolution
    static constexpr double WaterResolutionFactor = 0.75;

    //! Child quadrant origin offset as a fraction of the parent extent
    static constexpr double ChildQuarterOffset = 0.25;

    //! Opposite child quadrant origin offset as a fraction of the parent extent
    static constexpr double ChildThreeQuarterOffset = 0.75;

    //! Child extent and subdivision origin as a fraction of the parent extent
    static constexpr double ChildHalfFraction = 0.50;

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
    QMap<QString, QVector<COLLECTED_CHUNK_INFO> > m_mLastCollectedChunks; ///< Layer pass to identities of its last painted chunks
    QVector<CHUNK_PATH_INFO>     m_vLastCameraPath; ///< Quadtree readiness on the camera path, recorded by the last paint
    QVector<REGISTERED_HEIGHTFIELD_INFO> m_vLastHeightfields; ///< Physics height field identities, recorded by the last sync
    CGeoloc                     m_gLastCameraGeoloc; ///< Camera position retained from the last paint, for snapshots
    mutable QMutex              m_mSnapshotMutex; ///< Guards the retained snapshot state across render and snapshot threads
};
