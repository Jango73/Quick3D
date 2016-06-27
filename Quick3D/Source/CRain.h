
#pragma once

#include "quick3d_global.h"

// Application
#include "CParticleSystem.h"

class C3DScene;

class QUICK3D_EXPORT CRain : public CParticleSystem
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Constructeur d'après une scène
    CRain(C3DScene* pScene);

    //! Destructeur
    virtual ~CRain();

    //-------------------------------------------------------------------------------------------------
    // Méthodes héritées
    //-------------------------------------------------------------------------------------------------

    //! Met l'objet à jour d'après le temps écoulé depuis la dernière image
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //!
    virtual void paint(CRenderContext* pContext);
};
