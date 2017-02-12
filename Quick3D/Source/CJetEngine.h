
#pragma once

#include "quick3d_global.h"

// Application
#include "CLogManager.h"
#include "CQ3DConstants.h"
#include "CEngine.h"
#include "CServoPosition.h"

class C3DScene;

class QUICK3D_EXPORT CJetEngine : public CEngine
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instanciator(C3DScene* pScene);

    //!
    CJetEngine(C3DScene* pScene);

    //!
    virtual ~CJetEngine();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    virtual double currentThrust_kg() const;

    //!
    virtual double currentFuelCons_ls() const;

    //!
    virtual double n1_norm() const;

    //!
    virtual double n2_norm() const;

    //-------------------------------------------------------------------------------------------------
    // Méthodes héritées
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CJetEngine; }

    //!
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    CServoPosition              m_dN1;
    double                      m_dN2_norm;
};
