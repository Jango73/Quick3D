
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
    // Constructeurs et destructeur
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
    // M�thodes h�rit�es
    //-------------------------------------------------------------------------------------------------

    //! Active ce mat�riau pour le rendu
    virtual QGLShaderProgram* activate(CRenderContext* pContext);

    //! Retourne l'altitude � la g�olocalisation donn�e
    virtual double getHeightAt(const CGeoloc& gPosition, double* pRigidness = NULL);

    //! Retourne l'altitude � la g�olocalisation donn�e
    virtual double getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, double* pRigidness = NULL);

    //! Retourne l'altitude � la g�olocalisation donn�e
    virtual double getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, bool bForPhysics = true);

    //-------------------------------------------------------------------------------------------------
    // M�thodes de contr�le
    //-------------------------------------------------------------------------------------------------

    //! Retourne l'altitude de vague � la g�olocalisation donn�e
    double WaveHeight(CGeoloc gPosition);

    //-------------------------------------------------------------------------------------------------
    // M�thodes prot�g�es
    //-------------------------------------------------------------------------------------------------

protected:

    //-------------------------------------------------------------------------------------------------
    // Propri�t�s
    //-------------------------------------------------------------------------------------------------

protected:

};
