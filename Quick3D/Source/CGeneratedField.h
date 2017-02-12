
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CVector3.h"
#include "CGeoloc.h"
#include "CXMLNode.h"

#include "CHeightField.h"
#include "CGenerateFunction.h"

class QUICK3D_EXPORT CGeneratedField : public CHeightField
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CGeneratedField(CXMLNode xParameters);

    //!
    virtual ~CGeneratedField();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    virtual double getHeightAt(const CGeoloc& gPosition, double* pRigidness = NULL);

    //!
    virtual double getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, double* pRigidness = NULL);

    //!
    virtual double getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, bool bForPhysics = true);

    //!
    virtual bool isGenerated();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual void flatten(const CGeoloc& gPosition, double dRadius);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    CXMLNode            m_xParameters;
    CGenerateFunction*  m_pFunction;
};
