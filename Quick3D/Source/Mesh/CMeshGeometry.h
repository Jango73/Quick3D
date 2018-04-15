
#pragma once

// Qt
#include <QImage>
#include <QMutex>
#include <QMutexLocker>
#include <QVector>

// qt-plus
#include "CMemoryMonitor.h"

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CBoundingBox.h"
#include "CBoundPartition.h"
#include "CComponent.h"
#include "CVertex.h"
#include "CFace.h"
#include "CVertexGroup.h"
#include "CMaterial.h"
#include "CGLMeshData.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CMeshGeometry : public QSharedData, public CBoundPartitioned<int>, public CDumpable
{
    DECLARE_MEMORY_MONITORED

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CMeshGeometry(C3DScene* pScene, double dMaxDistance = 10000.0, bool bUseSpacePartitionning = true);

    //!
    virtual ~CMeshGeometry();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setURL(const QString& sURL);

    //!
    void setMaterial(QSP<CMaterial> pMaterial);

    //!
    void setBounds(CBoundingBox bBox);

    //!
    void setGLType(int iGLType);

    //!
    void setGeometryDirty(bool bDirty);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    QString url() const { return m_sURL; }

    //!
    QVector<CVertex>& vertices() { return m_vVertices; }

    //!
    const QVector<CVertex>& vertices() const { return m_vVertices; }

    //!
    QVector<CFace>& faces() { return m_vFaces; }

    //!
    const QVector<CFace>& faces() const { return m_vFaces; }

    //!
    QVector<CVertexGroup>& vertexGroups() { return m_vVertexGroups; }

    //!
    const QVector<CVertexGroup>& vertexGroups() const { return m_vVertexGroups; }

    //!
    const QVector<QSP<CMaterial> >& materials() const { return m_vMaterials; }

    //!
    QVector<QSP<CMaterial> >& materials() { return m_vMaterials; }

    //!
    QMap<QString, QString>& dynTexUpdaters() { return m_mDynTexUpdaters; }

    //!
    const QMap<QString, QString>& dynTexUpdaters() const { return m_mDynTexUpdaters; }

    //!
    double maxDistance() const { return m_dMaxDistance; }

    //!
    QVector<CGLMeshData*>& glMeshData() { return m_vGLMeshData; }

    //! Return number of triangles needed for this mesh
    int triangleCount();

    //!
    int triangleCountForFaces(const QVector<int>& vFaceIndices);

    //!
    CBoundingBox bounds();

    //!
    CBoundingBox worldBounds(CComponent* pContainer);

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! Dumps contents to a stream
    virtual void dump(QTextStream& stream, int iIdent);

    //!
    virtual void addDataForPartition(CBoundPartition<int>& partition) Q_DECL_OVERRIDE;

    //! Ray intersection
    Math::RayTracingResult intersect(CComponent* pContainer, Math::CRay3 rGlobalRay);

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void clear();

    //!
    void update(double dDeltaTime);

    //! Met à jour les buffers de géométrie OpenGL
    void checkAndUpdateGeometry();

    //! Dessine l'objet
    void paint(CRenderContext* pContext, CComponent* pContainer);

    //! Inverse les vecteurs normaux des polygones
    void flipNormals();

    //! Transforme les sommets
    void transformVertices(const Math::CMatrix4& matrix);

    //!
    void deleteMaterials();

    //! Creates unique vertices for each polygon
    void isolateVertices();

    //!
    void splitVerticesBySmoothingGroup();

    //! Computes normal vectors for vertices and faces
    void computeNormals();

    //!
    void createBox(Math::CVector3 vMinimum, Math::CVector3 vMaximum);

    //! Creates a sphere
    void createSphere(int iNumSegments);

    //! Creates a portion of sphere
    void createSpherePart(
            int iPanSegments,
            int iTiltSegments,
            double dStartPanDegrees,
            double dEndPanDegrees,
            double dStartTiltDegrees,
            double dEndTiltDegrees
            );

    //!
    void createCone(int iNumSegments, int iNumHeightSegments, double dHeight, double dBaseRadius, double dApexRadius);

    //! Creates a triangle patch and subdivides it iIterations times
    void createAdaptiveTriPatch(Math::CVector3 vCenter, int iIterations);

    //! Creates a quad patch and subdivides it iIterations times
    void createAdaptiveQuadPatch(Math::CVector3 vCenter, int iNumIterations);

    //! Creates a quad patch
    void createQuadPatch(int iNumVerts, int iNumSkirtVerts = 0, bool bDoubleSided = false);

    //! Creates a circular quad patch
    void createCircularQuadPatch(Math::CVector3 vCenter, int iNumVerts);

    //! Creates a quad patch and deforms it using vFFDFrom and vFFDTo
    void createSurfaceFromFFD(const QVector<Math::CVector3>& vFFDFrom, const QVector<Math::CVector3>& vFFDTo, int iNumVerts);

    //! Subdivises polygons (assumed to be all quads)
    void subdivideQuads(int iNumIterations);

    //! Merges this mesh with another
    void merge(const CMeshGeometry& other);

    //! Translates all UV coords by vTranslate
    void translateUVs(Math::CVector2 vTranslate);

    //! Scales all UV coords by vScale
    void scaleUVs(Math::CVector2 vScale);

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    virtual Math::RayTracingResult intersectPartitionData(CComponent* pContainer, const CBoundPartition<int>& partition, Math::CRay3 rLocalray) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    // Owned data

    QMutex                          m_mMutex;                   // Data protection for multithreading
    C3DScene*                       m_pScene;
    QString                         m_sURL;
    CBoundingBox                    m_bBounds;                  // Mesh bounds
    QVector<CVertex>                m_vVertices;                // Vertices of the mesh
    QVector<CFace>                  m_vFaces;                   // Polygons of the mesh
    QVector<CVertexGroup>           m_vVertexGroups;            // Skinning and deforming vertex groupd
    QVector<CGLMeshData*>           m_vGLMeshData;
    QMap<QString, QString>          m_mDynTexUpdaters;          // Components that update dynamic textures
    double                          m_dMaxDistance;             // Maximum distance at which this mesh is visible
    int                             m_iGLType;
    bool                            m_bUseSpacePartitionning;   // If true, polygons are partitioned
    bool                            m_bAutomaticBounds;
    bool                            m_bGeometryDirty;           // If true, normals and partitions mush be computed

    // Shared data

    QVector<QSP<CMaterial> >        m_vMaterials;               // Materials of the mesh
};
