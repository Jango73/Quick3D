
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

    //! Constructeur d'apr�s une sc�ne
    CRain(C3DScene* pScene);

    //! Destructeur
    virtual ~CRain();

    //-------------------------------------------------------------------------------------------------
    // M�thodes h�rit�es
    //-------------------------------------------------------------------------------------------------

    //! Met l'objet � jour d'apr�s le temps �coul� depuis la derni�re image
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //!
    virtual void paint(CRenderContext* pContext);
};
