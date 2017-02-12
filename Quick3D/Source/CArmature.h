
#pragma once

// Qt
#include <QImage>
#include <QDateTime>

// Application
#include "CQ3DConstants.h"
#include "CPhysicalComponent.h"
#include "CBone.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;
class CMesh;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CArmature : public CPhysicalComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instanciator(C3DScene* pScene);

    //!
    CArmature(C3DScene* pScene);

    //!
    virtual ~CArmature();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CArmature; }

    //!
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) Q_DECL_OVERRIDE;

    //! M�thode de mise � jour avec temps delta depuis la derni�re it�ration
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //!
    virtual void paint(CRenderContext* pContext) Q_DECL_OVERRIDE;

    //! Calcul d'intersection avec un rayon
    virtual Math::RayTracingResult intersect(Math::CRay3 ray) Q_DECL_OVERRIDE;

    //! Dump du contenu dans un flux
    virtual void dump(QTextStream& stream, int iIdent);

    //-------------------------------------------------------------------------------------------------
    // M�thodes prot�g�s
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    void updateSingleMesh(QSP<CMesh> pMesh, double dDeltaTime);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------
};
