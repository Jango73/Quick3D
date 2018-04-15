
#pragma once

// Qt
#include <QImage>
#include <QMutex>
#include <QMutexLocker>
#include <QVector>

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CPhysicalComponent.h"
#include "CMeshGeometry.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CMesh : public CPhysicalComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Retourne une nouvelle instance de cet objet
    static CComponent* instantiator(C3DScene* pScene);

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
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CMesh; }

    //!
    virtual CBoundingBox bounds();

    //!
    virtual CBoundingBox worldBounds();

    //!
    virtual void update(double dDeltaTimeS) Q_DECL_OVERRIDE;

    //! Charge les paramètres de l'objet
    virtual void loadParameters(const QString& sBaseFile, const CXMLNode& xComponent) Q_DECL_OVERRIDE;

    //! Solves the links of this object
    virtual void solveLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //! Deletes this object's links
    virtual void clearLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //! Dessine l'objet
    virtual void paint(CRenderContext* pContext) Q_DECL_OVERRIDE;

    //! Ray intersection
    virtual Math::RayTracingResult intersect(Math::CRay3 ray) Q_DECL_OVERRIDE;

    //! Inverse les vecteurs normaux des polygones
    virtual void flipNormals();

    //! Transforme les sommets
    virtual void transformVertices(const Math::CMatrix4& matrix);

    //! Dumps contents to a stream
    virtual void dump(QTextStream& stream, int iIdent);

    //-------------------------------------------------------------------------------------------------
    // Static methods
    //-------------------------------------------------------------------------------------------------

    //! Creates an exploded sphere to allow for multiple image maps
    static CComponent* createMultiTextureSphere(
            C3DScene* pScene,
            int iNumSegments,
            int m_iPanCount,
            int m_iTiltCount,
            double dMaxDistance = 10000.0
            );

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QSP<CMeshGeometry>      m_pGeometry;
};
