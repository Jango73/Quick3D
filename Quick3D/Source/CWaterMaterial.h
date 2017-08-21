
#pragma once

// Qt
#include <QVector2D>

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CGeoloc.h"
#include "CMaterial.h"
#include "CHeightField.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

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

    //! Returns this object's class name
    virtual QString getClassName() const { return ClassName_CWaterMaterial; }

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! Active ce mat�riau pour le rendu
    virtual QGLShaderProgram* activate(CRenderContext* pContext);

    //! Retourne l'altitude � la g�olocalisation donn�e
    virtual double getHeightAt(const CGeoloc& gPosition, double* pRigidness = nullptr);

    //! Retourne l'altitude � la g�olocalisation donn�e
    virtual double getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, double* pRigidness = nullptr);

    //! Retourne l'altitude � la g�olocalisation donn�e
    virtual double getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, bool bForPhysics = true);

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Retourne l'altitude de vague � la g�olocalisation donn�e
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
