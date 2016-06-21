
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

#include "CComponent.h"
#include "C3DScene.h"

//-------------------------------------------------------------------------------------------------

template <class T>
class CComponentReference
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //!
    CComponentReference()
        : m_pComponent(NULL)
    {
    }

    //!
    CComponentReference(QString sName)
        : m_pComponent(NULL)
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
    const QString& getName() const { return m_sName; }

    //!
    QSP<CComponent> component() { return m_pComponent; }

    //!
    const QSP<CComponent> component() const { return m_pComponent; }

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    //!
    void clear()
    {
        m_pComponent.reset();
    }

    //!
    void solve(C3DScene* pScene, QSP<CComponent> pCaller)
    {
        foreach (QSP<CComponent> pComponent, pScene->getComponents())
        {
            QSP<CComponent> pFound = pComponent->findComponent(m_sName, pCaller);

            if (pFound)
            {
                m_pComponent = pFound;
                break;
            }
        }
    }

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    QString             m_sName;
    QSP<CComponent>     m_pComponent;
};
