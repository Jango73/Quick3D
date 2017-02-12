
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Application
#include "CQ3DConstants.h"
#include "CComponent.h"

//-------------------------------------------------------------------------------------------------

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
        m_eType			= target.m_eType;
        m_dVoltage		= target.m_dVoltage;
        m_dAmperage		= target.m_dAmperage;
        m_dFrequency	= target.m_dFrequency;

        return *this;
    }

    ECurrentType	m_eType;
    double			m_dVoltage;
    double			m_dAmperage;
    double			m_dFrequency;
};

class QUICK3D_EXPORT CElectricalComponent : public CComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instanciator(C3DScene* pScene);

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
    virtual QString getClassName() const { return ClassName_CElectricalComponent; }

    //! Charge les paramètres de cet objet
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) Q_DECL_OVERRIDE;

    //! Recherche les liens de cet objet
    virtual void solveLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //! Efface les liens de cet objet
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
