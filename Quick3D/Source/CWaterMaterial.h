
#pragma once

#include "quick3d_global.h"

// Qt
#include <QVector2D>

// Fondations
#include "CGeoloc.h"

// Application
#include "CQ3DConstants.h"
#include "CMaterial.h"
#include "CHeightField.h"

class C3DScene;

class QUICK3D_EXPORT CWaterMaterial : public CMaterial, public CHeightField
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CWaterMaterial(C3DScene* pScene);

    //!
    virtual ~CWaterMaterial();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Retourne le nom de la classe
    virtual QString getClassName() const { return ClassName_CWaterMaterial; }

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! Active ce matériau pour le rendu
    virtual QGLShaderProgram* activate(CRenderContext* pContext);

    //! Retourne l'altitude à la géolocalisation donnée
    virtual double getHeightAt(const CGeoloc& gPosition, double* pRigidness = NULL);

    //! Retourne l'altitude à la géolocalisation donnée
    virtual double getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, double* pRigidness = NULL);

    //! Retourne l'altitude à la géolocalisation donnée
    virtual double getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, bool bForPhysics = true);

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Retourne l'altitude de vague à la géolocalisation donnée
    double WaveHeight(CGeoloc gPosition);

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

};
