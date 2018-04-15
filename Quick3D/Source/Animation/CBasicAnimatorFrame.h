
#pragma once

#include "quick3d_global.h"

// Qt
#include <QVector>

// Application
#include "CQ3DConstants.h"
#include "CAnimatorFrame.h"

class QUICK3D_EXPORT CBasicAnimatorFrame : public CAnimatorFrame
{

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Default constructor
    CBasicAnimatorFrame();

    //! Destructor
    virtual ~CBasicAnimatorFrame();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Computes a target value given a current value and a delta time
    virtual Math::CVector3 compute(double dDeltaTime, Math::CVector3 vCurrentValue) Q_DECL_OVERRIDE;

    //! Returns the current value
    virtual Math::CVector3 currentValue() const Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //! Calcule une valeur cible selon une valeur courante
    double computeSingleAxis(double dDeltaTime, double dCurrentValue, double dTargetValue, double dSpeed);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    Math::CVector3      m_vCurrentValue;
};
