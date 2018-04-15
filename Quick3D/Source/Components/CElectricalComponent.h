
#pragma once

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CComponent.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

enum ECurrentType
{
    ctNone,
    ctAC,
    ctDC
};

class CElectricalLoad
{
public:

    //!
    CElectricalLoad()
        : m_eType(ctAC)
        , m_dVoltage(200.0)
        , m_dAmperage(0.0)
        , m_dFrequency(400.0)
    {
    }

    //!
    CElectricalLoad(ECurrentType eType, double dVoltage, double dAmperage, double dFrequency)
        : m_eType(eType)
        , m_dVoltage(dVoltage)
        , m_dAmperage(dAmperage)
        , m_dFrequency(dFrequency)
    {
    }

    //!
    CElectricalLoad(const CElectricalLoad& target)
    {
        *this = target;
    }

    //!
    CElectricalLoad& operator = (const CElectricalLoad& target)
    {
        m_eType         = target.m_eType;
        m_dVoltage      = target.m_dVoltage;
        m_dAmperage     = target.m_dAmperage;
        m_dFrequency    = target.m_dFrequency;

        return *this;
    }

    static CElectricalLoad noPower()
    {
        return CElectricalLoad(ctNone, 0.0, 0.0, 0.0);
    }

    ECurrentType    m_eType;
    double          m_dVoltage;
    double          m_dAmperage;
    double          m_dFrequency;
};

class QUICK3D_EXPORT CElectricalComponent : public CComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instantiator(C3DScene* pScene);

    //!
    CElectricalComponent(C3DScene* pScene);

    //!
    virtual ~CElectricalComponent();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    const CElectricalLoad& load() const { return m_tLoad; }

    //!
    bool isShortCircuited() const { return m_bShortCircuited; }

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CElectricalComponent; }

    //! Loads this object's parameters
    virtual void loadParameters(const QString& sBaseFile, const CXMLNode& xComponent) Q_DECL_OVERRIDE;

    //! Solves the links of this object
    virtual void solveLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //! Deletes this object's links
    virtual void clearLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //!
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual void push(const CElectricalLoad& tLoad, double dDeltaTime);

    //!
    virtual CElectricalLoad pull(double dAmperage, double dDeltaTime);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    double                          m_dMaxAmperage;
    bool                            m_bShortCircuited;
    CElectricalLoad                 m_tLoad;
    QVector<QString>                m_vPowerInputNames;
    QVector<CElectricalComponent*>  m_vPowerInputs;
    QVector<CElectricalComponent*>  m_vPowerOutputs;
};
