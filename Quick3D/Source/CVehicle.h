
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CAverager.h"
#include "CLogManager.h"

// Application
#include "CQ3DConstants.h"
#include "CComponent.h"
#include "CTrajectorable.h"
#include "CMeshInstance.h"

class C3DScene;

class QUICK3D_EXPORT CVehicle : public CTrajectorable
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instanciator(C3DScene* pScene);

    //! Constructeur
    CVehicle(C3DScene* pScene);

    //!
    virtual ~CVehicle();

    //-------------------------------------------------------------------------------------------------
    // Méthodes héritées
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CVehicle; }

    //!
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent);

    //!
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //!
    virtual void paint(CRenderContext* pContext) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    QVector<CMeshInstance*>     m_vAxisMeshes;
};
