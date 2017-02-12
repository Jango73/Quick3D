
#pragma once

#include "quick3d_global.h"

// Application
#include "CQ3DConstants.h"
#include "CMesh.h"
#include "CHeightField.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CWater : public CMesh, public CHeightField
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CWater(C3DScene* pScene, double dSize, int iIterations);

    //!
    virtual ~CWater();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! Retourne le nom de la classe
    virtual QString getClassName() const { return ClassName_CWater; }

    //! M�thode de mise � jour avec temps delta depuis la derni�re it�ration
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //! Dans cette m�thode, l'objet doit faire son rendu
    virtual void paint(CRenderContext* pContext) Q_DECL_OVERRIDE;

    //!
    virtual double getHeightAt(const CGeoloc& gPosition, double* pRigidness = NULL);

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------
};
