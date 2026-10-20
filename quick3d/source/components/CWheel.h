
#pragma once

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CPhysicalComponent.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CWheel : public CPhysicalComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Returns a new instance of this class
    static CComponent* instantiator(C3DScene* pScene);

    //! Constructor using a scene
    CWheel(C3DScene* pScene);

    //! Destructor
    virtual ~CWheel();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! Sets the tire radius in meters, used as ground offset for the parent vehicle
    void setRadiusM(double value) { m_dRadiusM = value; }

    //! Sets the tire width in meters
    void setWidthM(double value) { m_dWidthM = value; }

    //! Sets true when engine torque reaches this wheel
    void setDriven(bool value) { m_bDriven = value; }

    //! Sets true when this wheel steers the vehicle
    void setSteered(bool value) { m_bSteered = value; }

    //! Sets the maximum steer angle in degrees, 0 for a fixed wheel
    void setMaxSteerAngleDeg(double value) { m_dMaxSteerAngleDeg = value; }

    //! Sets the suspension length at full bump in meters
    void setSuspensionMinLengthM(double value) { m_dSuspensionMinLengthM = value; }

    //! Sets the suspension length at full droop in meters
    void setSuspensionMaxLengthM(double value) { m_dSuspensionMaxLengthM = value; }

    //! Sets the suspension spring frequency in hertz
    void setSuspensionFrequencyHz(double value) { m_dSuspensionFrequencyHz = value; }

    //! Sets the suspension spring damping, 0 to 1
    void setSuspensionDampingNorm(double value) { m_dSuspensionDampingNorm = value; }

    //! Sets the tire friction level along rolling direction
    void setLongitudinalFrictionNorm(double value) { m_dLongitudinalFrictionNorm = value; }

    //! Sets the tire friction level along sideways direction
    void setLateralFrictionNorm(double value) { m_dLateralFrictionNorm = value; }

    //! Sets true when the hand brake torque reaches this wheel
    void setHandBrake(bool value) { m_bHandBrake = value; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns the tire radius in meters
    double radiusM() const { return m_dRadiusM; }

    //! Returns the tire width in meters
    double widthM() const { return m_dWidthM; }

    //! Returns true when engine torque reaches this wheel
    bool driven() const { return m_bDriven; }

    //! Returns true when this wheel steers the vehicle
    bool steered() const { return m_bSteered; }

    //! Returns the maximum steer angle in degrees
    double maxSteerAngleDeg() const { return m_dMaxSteerAngleDeg; }

    //! Returns the suspension length at full bump in meters
    double suspensionMinLengthM() const { return m_dSuspensionMinLengthM; }

    //! Returns the suspension length at full droop in meters
    double suspensionMaxLengthM() const { return m_dSuspensionMaxLengthM; }

    //! Returns the suspension spring frequency in hertz
    double suspensionFrequencyHz() const { return m_dSuspensionFrequencyHz; }

    //! Returns the suspension spring damping, 0 to 1
    double suspensionDampingNorm() const { return m_dSuspensionDampingNorm; }

    //! Returns the tire friction level along rolling direction
    double longitudinalFrictionNorm() const { return m_dLongitudinalFrictionNorm; }

    //! Returns the tire friction level along sideways direction
    double lateralFrictionNorm() const { return m_dLateralFrictionNorm; }

    //! Returns true when the hand brake torque reaches this wheel
    bool handBrake() const { return m_bHandBrake; }

    //-------------------------------------------------------------------------------------------------
    // Overridden methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CWheel; }

    //!
    virtual void loadParameters(const QString& sBaseFile, const CXMLNode& xComponent) Q_DECL_OVERRIDE;

    //! Updates this object using the elapsed time since last update
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    double                  m_dRadiusM;
    double                  m_dWidthM;
    double                  m_dMaxSteerAngleDeg;
    double                  m_dSuspensionMinLengthM;
    double                  m_dSuspensionMaxLengthM;
    double                  m_dSuspensionFrequencyHz;
    double                  m_dSuspensionDampingNorm;
    double                  m_dLongitudinalFrictionNorm;
    double                  m_dLateralFrictionNorm;
    bool                    m_bDriven;
    bool                    m_bSteered;
    bool                    m_bHandBrake;
};
