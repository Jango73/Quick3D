
#pragma once

#include "quick3d_global.h"

// Application
#include "CQ3DConstants.h"
#include "CElectricalComponent.h"

class C3DScene;

class QUICK3D_EXPORT CElectricalContactor : public CElectricalComponent
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//!
	static CComponent* instanciator(C3DScene* pScene);

	//!
	CElectricalContactor(C3DScene* pScene);

	//!
	virtual ~CElectricalContactor();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//!
	void setClosed(bool bValue) { m_bClosed = bValue; }

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//!
	bool getClosed() const { return m_bClosed; }

	//-------------------------------------------------------------------------------------------------
	// M�thodes h�rit�es
	//-------------------------------------------------------------------------------------------------

	//!
	virtual QString getClassName() const { return ClassName_CElectricalContactor; }

	//! Charge les param�tres de cet objet
	virtual void loadParameters(CXMLNode xComponent);

	//!
	virtual void update(double dDeltaTime);

	//!
	virtual void push(const CElectricalLoad& tLoad, double dDeltaTime);

	//!
	virtual CElectricalLoad pull(double dAmperage, double dDeltaTime);

	//-------------------------------------------------------------------------------------------------
	// M�thodes de contr�le
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// Propri�t�s
	//-------------------------------------------------------------------------------------------------

protected:

	bool	m_bClosed;
};
