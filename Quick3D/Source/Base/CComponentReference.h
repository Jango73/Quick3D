
#pragma once

// Application
#include "quick3d_global.h"
#include "CComponent.h"
#include "C3DScene.h"

//-------------------------------------------------------------------------------------------------

template <class T>
class CComponentReference
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CComponentReference()
        : m_pComponent(nullptr)
    {
    }

    //!
    CComponentReference(QString sName)
        : m_pComponent(nullptr)
        , m_sName(sName)
    {
    }

    //!
    virtual ~CComponentReference()
    {
    }

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setName(const QString& sName) { m_sName = sName; }

    //!
    void setComponent(QSP<CComponent> pComponent) { m_pComponent = pComponent; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    const QString& name() const { return m_sName; }

    //!
    QSP<CComponent> component() { return m_pComponent; }

    //!
    const QSP<CComponent> component() const { return m_pComponent; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void clear()
    {
        m_pComponent.reset();
    }

    //!
    void solve(C3DScene* pScene, QSP<CComponent> pCaller)
    {
        foreach (QSP<CComponent> pComponent, pScene->components())
        {
            QSP<CComponent> pFound = pComponent->findComponent(m_sName, pCaller);

            if (pFound != nullptr)
            {
                m_pComponent = pFound;
                break;
            }
        }
    }

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QString             m_sName;
    QSP<CComponent>     m_pComponent;
};
