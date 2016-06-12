
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
    // M�thodes h�rit�es
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CArmature; }

    //!
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent);

    //! M�thode de mise � jour avec temps delta depuis la derni�re it�ration
    virtual void update(double dDeltaTime);

    //!
    virtual void paint(CRenderContext* pContext);

    //! Calcul d'intersection avec un rayon
    virtual Math::RayTracingResult intersect(Math::CRay3 ray);

    //! Dump du contenu dans un flux
    virtual void dump(QTextStream& stream, int iIdent);

    //-------------------------------------------------------------------------------------------------
    // M�thodes prot�g�s
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    void updateSingleMesh(CMesh* pMesh, double dDeltaTime);

    //-------------------------------------------------------------------------------------------------
    // Propri�t�s
    //-------------------------------------------------------------------------------------------------
};
