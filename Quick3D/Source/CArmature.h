
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
    // Constructeurs et destructeur
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
    // Méthodes héritées
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CArmature; }

    //!
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent);

    //! Méthode de mise à jour avec temps delta depuis la dernière itération
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //!
    virtual void paint(CRenderContext* pContext) Q_DECL_OVERRIDE;

    //! Calcul d'intersection avec un rayon
    virtual Math::RayTracingResult intersect(Math::CRay3 ray) Q_DECL_OVERRIDE;

    //! Dump du contenu dans un flux
    virtual void dump(QTextStream& stream, int iIdent);

    //-------------------------------------------------------------------------------------------------
    // Méthodes protégés
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    void updateSingleMesh(QSP<CMesh> pMesh, double dDeltaTime);

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------
};
