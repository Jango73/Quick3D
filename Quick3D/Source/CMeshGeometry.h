
#pragma once

// Qt
#include <QImage>
#include <QMutex>
#include <QMutexLocker>
#include <QVector>

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CBoundingBox.h"
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

class QUICK3D_EXPORT CMeshPartition
{
public:

    CMeshPartition()
    {
    }

    CMeshPartition(CBoundingBox bBounds)
        : m_bBounds(bBounds)
    {
    }

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Retourne le délimiteur de la partition
    CBoundingBox& getBounds() { return m_bBounds; }

    //! Retourne le vecteur de partitions enfant
    QVector<CMeshPartition>& getChildren() { return m_vChildren; }

    //! Retourne le vecteur d'indices de facettes
    QVector<int>& getFaceIndices() { return m_vFaceIndices; }

    //! Retourne le vecteur d'indices de facettes
    const QVector<int>& getFaceIndices() const { return m_vFaceIndices; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    CMeshPartition& operator = (const CMeshPartition& target)
    {
        m_bBounds = target.m_bBounds;
        m_vChildren = target.m_vChildren;
        m_vFaceIndices = target.m_vFaceIndices;

        return *this;
    }

    //!
    void clear()
    {
        m_bBounds = CBoundingBox();
        m_vChildren.clear();
        m_vFaceIndices.clear();
    }

    //!
    void addChild(CMeshPartition mpChild)
    {
        m_vChildren.append(mpChild);
    }

protected:

    CBoundingBox                m_bBounds;
    QVector<CMeshPartition>     m_vChildren;
    QVector<int>                m_vFaceIndices;
};

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CMeshGeometry : public QSharedData, public CDumpable
{
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

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! Dumps contents to a stream
    virtual void dump(QTextStream& stream, int iIdent);

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void clear();

    //!
    void update(double dDeltaTime);

    //! Met à jour les buffers de géométrie OpenGL
    void checkAndUpdateGeometry();

    //!
    CBoundingBox getBounds();

    //!
    CBoundingBox getWorldBounds(CComponent* pContainer);

    //! Dessine l'objet
    void paint(CRenderContext* pContext, CComponent* pContainer);

    //! Inverse les vecteurs normaux des polygones
    void flipNormals();

    //! Transforme les sommets
    void transformVertices(const Math::CMatrix4& matrix);

    //!
    void deleteMaterials();

    //!
    void createPartition(CMeshPartition& mpCurrentPartition, int iLevel);

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

    //! Ray intersection
    virtual Math::RayTracingResult intersect(CComponent* pContainer, Math::CRay3 ray);

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    Math::RayTracingResult intersectRecurse(CComponent* pContainer, CMeshPartition& mpPartition, Math::CRay3 ray);

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
    CMeshPartition                  m_mpPartitions;             // Ray-tracing acceleration partitions
    double                          m_dMaxDistance;             // Maximum distance at which this mesh is visible
    int                             m_iGLType;
    bool                            m_bUseSpacePartitionning;   // If true, polygons are partitioned
    bool                            m_bAutomaticBounds;
    bool                            m_bGeometryDirty;           // If true, normals and partitions mush be computed

    // Shared data

    QVector<QSP<CMaterial> >        m_vMaterials;               // Materials of the mesh
};
