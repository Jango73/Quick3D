
#pragma once

#include "quick3d_global.h"

// Qt
#include <QSharedPointer>
#include <QImage>
#include <QMutex>
#include <QMutexLocker>
#include <QVector>

// Application
#include "CQ3DConstants.h"
#include "CPhysicalComponent.h"
#include "CVertex.h"
#include "CFace.h"
#include "CVertexGroup.h"
#include "CMaterial.h"
#include "CGLMeshData.h"

//-------------------------------------------------------------------------------------------------

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

    //! Retourne le délimiteur de la partition
    const CBoundingBox& getBounds() const { return m_bBounds; }

    //! Retourne le vecteur de partitions enfant
    QVector<CMeshPartition>& getChildren() { return m_vChildren; }

    //! Retourne le vecteur de partitions enfant
    const QVector<CMeshPartition>& getChildren() const { return m_vChildren; }

    //! Retourne le vecteur d'indices de facettes
    QVector<int>& getFaceIndices() { return m_vFaceIndices; }

    //! Retourne le vecteur d'indices de facettes
    const QVector<int>& getFaceIndices() const { return m_vFaceIndices; }

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
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

class QUICK3D_EXPORT CMesh : public CPhysicalComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Retourne une nouvelle instance de cet objet
    static CComponent* instanciator(C3DScene* pScene);

    //!
    CMesh(C3DScene* pScene, double dMaxDistance = 10000.0, bool bUseSpacePartitionning = false);

    //!
    virtual ~CMesh();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setMaterial(QSharedPointer<CMaterial> pMaterial);

    //!
    void setBounds(CBoundingBox bBox);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    QVector<CVertex>& getVertices() { return m_vVertices; }

    //!
    const QVector<CVertex>& getVertices() const { return m_vVertices; }

    //!
    QVector<CFace>& getFaces() { return m_vFaces; }

    //!
    const QVector<CFace>& getFaces() const { return m_vFaces; }

    //!
    QVector<CVertexGroup>& getVertexGroups() { return m_vVertexGroups; }

    //!
    const QVector<CVertexGroup>& getVertexGroups() const { return m_vVertexGroups; }

    //!
    const QVector<QSharedPointer<CMaterial> >& getMaterials() const { return m_vMaterials; }

    //!
    QVector<QSharedPointer<CMaterial> >& getMaterials() { return m_vMaterials; }

    //!
    double getMaxDistance() const { return m_dMaxDistance; }

    //! Return number of triangles needed for this mesh
    int getNumTriangleCount();

    //!
    int getNumTriangleCountForFaces(const QVector<int>& vFaceIndices);

    //-------------------------------------------------------------------------------------------------
    // Méthodes héritées
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CMesh; }

    //!
    virtual CBoundingBox getBounds() const;

    //!
    virtual CBoundingBox getWorldBounds() const;

    //!
    virtual void update(double dDeltaTime);

    //! Charge les paramètres de l'objet
    virtual void loadParameters(CXMLNode xComponent);

    //! Recherche les liens de cet objet
    virtual void solveLinks(C3DScene* pScene);

    //! Dessine l'objet
    virtual void paint(CRenderContext* pContext);

    //! Inverse les vecteurs normaux des polygones
    virtual void flipNormals();

    //! Transforme les sommets
    virtual void transformVertices(const Math::CMatrix4& matrix);

    //! Dump du contenu dans un flux
    virtual void dump(QTextStream& stream, int iIdent);

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    //!
    void deleteMaterials();

    //! Met à jour les buffers de géométrie OpenGL
    virtual void updateGeometry(bool bComputeNormals = true);

    //!
    void createPartition(CMeshPartition& mpCurrentPartition, int iLevel);

    //! Crée un vertex unique pour chaque polygone
    void isolateVertices();

    //!
    void splitVerticesBySmoothingGroup();

    //! Calcule les vecteurs normaux
    void computeNormals();

    //! Crée une sphère
    void createSphere(int iNumSegments);

    //! Crée une portion de sphère
    void createSpherePart(
            int iPanSegments,
            int iTiltSegments,
            double dStartPanDegrees,
            double dEndPanDegrees,
            double dStartTiltDegrees,
            double dEndTiltDegrees
            );

    //! Crée un patch de triangles subdivisés en fonction de la distance
    void createAdaptiveTriPatch(Math::CVector3 vCenter, int iIterations);

    //! Crée un patch de quads subdivisés en fonction de la distance
    void createAdaptiveQuadPatch(Math::CVector3 vCenter, int iNumIterations);

    //! Crée un patch de quads
    void createQuadPatch(int iNumVerts, int iNumSkirtVerts = 0, bool bDoubleSided = false);

    //! Crée un patch circulaire de quads
    void createCircularQuadPatch(Math::CVector3 vCenter, int iNumVerts);

    //!
    void createSurfaceFromFFD(const QVector<Math::CVector3>& vFFDFrom, const QVector<Math::CVector3>& vFFDTo, int iNumVerts);

    //! Subdivise les polygones (supposés tous quads)
    void subdivideQuads(int iNumIterations);

    //! Fusionne un mesh dans ce mesh
    void merge(const CMesh& other, bool bUpdateGeometry = true);

    //! Déplace les coordonnées de texture
    void translateUVs(Math::CVector2 vTranslate);

    //! Met à l'échelle les coordonnées de texture
    void scaleUVs(Math::CVector2 vScale);

    //! Calcul d'intersection avec un rayon
    virtual Math::RayTracingResult intersect(Math::CRay3 ray) const;

    //-------------------------------------------------------------------------------------------------
    // Méthodes statiques
    //-------------------------------------------------------------------------------------------------

    //! Créé une sphère "explosée" pour supporter une grille de textures
    static CComponent* createMultiTextureSphere(
            C3DScene* pScene,
            int iNumSegments,
            int m_iPanCount,
            int m_iTiltCount,
            double dMaxDistance = 10000.0
            );

    //-------------------------------------------------------------------------------------------------
    // Méthodes protégées
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    Math::RayTracingResult intersectRecurse(const CMeshPartition& mpPartition, Math::CRay3 ray) const;

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    QMutex                                  m_mMutex;					// Mutex de protection des données entre threads
    CBoundingBox                            m_bBounds;					// Boite englobante du maillage
    QVector<CVertex>                        m_vVertices;				// Sommets du maillage
    QVector<CFace>                          m_vFaces;					// Polygones du maillage
    QVector<CVertexGroup>                   m_vVertexGroups;			// Groupes de sommets pour skinning
    QVector<QSharedPointer<CMaterial> >     m_vMaterials;				// Matériau du maillage
    QVector<CGLMeshData*>                   m_vGLMeshData;
    QMap<QString, QString>                  m_mDynTexUpdaters;
    CMeshPartition                          m_mpPartitions;				// Partitions pour accélerer le ray-tracing
    double                                  m_dMaxDistance;				// Distance maximale à laquelle ce maillage est visible
    bool                                    m_bPointCloud;
    bool                                    m_bAllQuads;				// Si vrai, tous les polygones ont quatre côtés
    bool                                    m_bUseSpacePartitionning;	// Si vrai, le partionnement des polygones est utilisé via m_mpPartitions
};
