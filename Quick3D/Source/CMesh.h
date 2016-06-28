
#pragma once

#include "quick3d_global.h"

// Qt
#include <QImage>
#include <QMutex>
#include <QMutexLocker>
#include <QVector>

// Application
#include "CQ3DConstants.h"
#include "CPhysicalComponent.h"
#include "CMeshGeometry.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;

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
    CMesh(C3DScene* pScene, double dMaxDistance = 10000.0, bool bUseSpacePartitionning = true);

    //!
    virtual ~CMesh();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setGeometry(QSP<CMeshGeometry> pGeometry);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    QSP<CMeshGeometry> geometry() { return m_pGeometry; }

    //-------------------------------------------------------------------------------------------------
    // Méthodes héritées
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CMesh; }

    //!
    virtual CBoundingBox getBounds();

    //!
    virtual CBoundingBox getWorldBounds();

    //!
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //! Charge les paramètres de l'objet
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) Q_DECL_OVERRIDE;

    //! Recherche les liens de cet objet
    virtual void solveLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //! Efface les liens de cet objet
    virtual void clearLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //! Dessine l'objet
    virtual void paint(CRenderContext* pContext) Q_DECL_OVERRIDE;

    //! Calcul d'intersection avec un rayon
    virtual Math::RayTracingResult intersect(Math::CRay3 ray) Q_DECL_OVERRIDE;

    //! Inverse les vecteurs normaux des polygones
    virtual void flipNormals();

    //! Transforme les sommets
    virtual void transformVertices(const Math::CMatrix4& matrix);

    //! Dump du contenu dans un flux
    virtual void dump(QTextStream& stream, int iIdent);

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
    Math::RayTracingResult intersectRecurse(CMeshPartition& mpPartition, Math::CRay3 ray);

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    QSP<CMeshGeometry>      m_pGeometry;
};
