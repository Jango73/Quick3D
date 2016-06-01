
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Application
#include "CQ3DConstants.h"
#include "CController.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CStandardController : public CController
{

public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//! Retourne une nouvelle instance de cet objet
	static CComponent* instanciator(C3DScene* pScene);

	//! Constructeur d'apr�s une sc�ne
	CStandardController(C3DScene* pScene);

	//! Destructeur
	virtual ~CStandardController();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// M�thodes de contr�le
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// M�thodes h�rit�es
	//-------------------------------------------------------------------------------------------------

	//! Retourne le nom de classe de cet objet
	virtual QString getClassName() const { return ClassName_CStandardController; }

	//! Met l'objet � jour d'apr�s le temps �coul� depuis la derni�re image
	virtual void update(double dDeltaTime);

	//!
	virtual void keyPressEvent(QKeyEvent* event);

	//!
	virtual void keyReleaseEvent(QKeyEvent *event);

	//!
	virtual void mouseMoveEvent(QMouseEvent* event);

	//!
	virtual void q3dEvent(CQ3DEvent* event);

	//-------------------------------------------------------------------------------------------------
	// Propri�t�s
	//-------------------------------------------------------------------------------------------------

protected:

	bool		m_bLookUp;
	bool		m_bLookDown;
	bool		m_bGoForward;
	bool		m_bGoBackward;
	bool		m_bGoUp;
	bool		m_bGoDown;
	bool		m_bTurnRight;
	bool		m_bTurnLeft;
	bool		m_bStrafeRight;
	bool		m_bStrafeLeft;
	bool		m_bAltitudeFastUp;
	bool		m_bAltitudeFastDown;
};
