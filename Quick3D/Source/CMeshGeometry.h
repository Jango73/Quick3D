
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
#include "CBoundingBox.h"
#include "CComponent.h"
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

    //! Retourne le vecteur de partitions enfant
    QVector<CMeshPartition>& getChildren() { return m_vChildren; }

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

class QUICK3D_EXPORT CMeshGeometry : public CDumpable
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
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
    void setMaterial(QSharedPointer<CMaterial> pMaterial);

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
    QString getURL() const { return m_sURL; }

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
    QMap<QString, QString>& getDynTexUpdaters() { return m_mDynTexUpdaters; }

    //!
    const QMap<QString, QString>& getDynTexUpdaters() const { return m_mDynTexUpdaters; }

    //!
    double getMaxDistance() const { return m_dMaxDistance; }

    //!
    QVector<CGLMeshData*>& getGLMeshData() { return m_vGLMeshData; }

    //! Return number of triangles needed for this mesh
    int getNumTriangleCount();

    //!
    int getNumTriangleCountForFaces(const QVector<int>& vFaceIndices);

    //-------------------------------------------------------------------------------------------------
    // Méthodes héritées
    //-------------------------------------------------------------------------------------------------

    //! Dump du contenu dans un flux
    virtual void dump(QTextStream& stream, int iIdent);

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
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

    //! Crée un vertex unique pour chaque polygone
    void isolateVertices();

    //!
    void splitVerticesBySmoothingGroup();

    //! Calcule les vecteurs normaux
    void computeNormals();

    //!
    void createBox(Math::CVector3 vMinimum, Math::CVector3 vMaximum);

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

    //!
    void createCone(int iNumSegments, int iNumHeightSegments, double dHeight, double dBaseRadius, double dApexRadius);

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
    void merge(const CMeshGeometry& other);

    //! Déplace les coordonnées de texture
    void translateUVs(Math::CVector2 vTranslate);

    //! Met à l'échelle les coordonnées de texture
    void scaleUVs(Math::CVector2 vScale);

    //! Calcul d'intersection avec un rayon
    virtual Math::RayTracingResult intersect(CComponent* pContainer, Math::CRay3 ray);

    //-------------------------------------------------------------------------------------------------
    // Méthodes protégées
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    Math::RayTracingResult intersectRecurse(CComponent* pContainer, CMeshPartition& mpPartition, Math::CRay3 ray);

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    // Owned data

    QMutex                                  m_mMutex;                   // Mutex de protection des données entre threads
    C3DScene*                               m_pScene;
    QString                                 m_sURL;
    CBoundingBox                            m_bBounds;                  // Boite englobante du maillage
    QVector<CVertex>                        m_vVertices;                // Sommets du maillage
    QVector<CFace>                          m_vFaces;                   // Polygones du maillage
    QVector<CVertexGroup>                   m_vVertexGroups;            // Groupes de sommets pour skinning
    QVector<CGLMeshData*>                   m_vGLMeshData;
    QMap<QString, QString>                  m_mDynTexUpdaters;
    CMeshPartition                          m_mpPartitions;             // Partitions pour accélerer le ray-tracing
    double                                  m_dMaxDistance;             // Distance maximale à laquelle ce maillage est visible
    int                                     m_iGLType;
    bool                                    m_bUseSpacePartitionning;   // Si vrai, le partionnement des polygones est utilisé via m_mpPartitions
    bool                                    m_bAutomaticBounds;
    bool                                    m_bGeometryDirty;

    // Shared data

    QVector<QSharedPointer<CMaterial> >     m_vMaterials;               // Matériaux du maillage
};
