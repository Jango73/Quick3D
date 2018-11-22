
#pragma once

#include "quick3d_global.h"

// Qt
#include <QVector>

// Application
#include "CQ3DConstants.h"
#include "CBasicAnimationStep.h"

//! Defines a frame for an animator
class QUICK3D_EXPORT CAnimatorFrame
{

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Default constructor
    CAnimatorFrame();

    //! Destructor
    virtual ~CAnimatorFrame();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns the list of steps as a reference
    QVector<CBasicAnimationStep>& steps() { return m_vSteps; }

    //! Returns the list of steps as a constant reference
    const QVector<CBasicAnimationStep>& steps() const { return m_vSteps; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Adds an animation step
    void addStep(CBasicAnimationStep value);

    //! Computes a target value given a current value and a delta time
    virtual Math::CVector3 compute(double dDeltaTime, Math::CVector3 vCurrentValue);

    //! Returns the current value
    virtual Math::CVector3 currentValue() const;

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QVector<CBasicAnimationStep>    m_vSteps;
    int                             m_iCurrentStep;
    double                          m_dTime;
};
