
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
	void setComponent(CComponent* pComponent) { m_pComponent = pComponent; }

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//!
	const QString& getName() const { return m_sName; }

	//!
	T* component() { return dynamic_cast<T*>(m_pComponent); }

	//!
	const CComponent* component() const { return dynamic_cast<T*>(m_pComponent); }

	//-------------------------------------------------------------------------------------------------
	// Méthodes de contrôle
	//-------------------------------------------------------------------------------------------------

	//!
	void solve(C3DScene* pScene, CComponent* pCaller)
	{
		foreach (QSharedPointer<CComponent> pComponent, pScene->getComponents())
		{
			CComponent* pFound = pComponent->findComponent(m_sName, pCaller);

			if (pFound != NULL)
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

	QString			m_sName;
	CComponent*		m_pComponent;
};
