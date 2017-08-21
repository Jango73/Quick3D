
#pragma once

#include "quick3d_global.h"

// Qt
#include <QVector>

// Application
#include "CMesh.h"
#include "CParticle.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CParticleSystem : public CMesh
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructeur
    CParticleSystem(C3DScene* pScene);

    //! Destructor
    virtual ~CParticleSystem();

    //!
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //!
    virtual void paint(CRenderContext* pContext) Q_DECL_OVERRIDE;

    //!
    QVector<CParticle>& getParticles() { return m_vParticles; }

protected:

    QVector<CParticle>  m_vParticles;
};
