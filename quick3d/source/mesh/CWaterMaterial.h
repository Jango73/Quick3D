
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
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CWaterMaterial; }

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! Active ce matériau pour le rendu
    virtual QOpenGLShaderProgram* activate(CRenderContext* pContext);

    //! Returns the altitude at the given geolocation
    virtual double getHeightAt(const CGeoloc& gPosition, double* pRigidness = nullptr);

    //! Returns the altitude at the given geolocation
    virtual double getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, double* pRigidness = nullptr);

    //! Returns the altitude at the given geolocation
    virtual double getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, bool bForPhysics = true);

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Returns the wave altitude at the given geolocation
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
