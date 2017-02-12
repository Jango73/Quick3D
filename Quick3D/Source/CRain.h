
#pragma once

#include "quick3d_global.h"

// Application
#include "CParticleSystem.h"

class C3DScene;

class QUICK3D_EXPORT CRain : public CParticleSystem
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructor using a scene
    CRain(C3DScene* pScene);

    //! Destructor
    virtual ~CRain();

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! Met l'objet � jour d'apr�s le temps �coul� depuis la derni�re image
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //!
    virtual void paint(CRenderContext* pContext) Q_DECL_OVERRIDE;
};
