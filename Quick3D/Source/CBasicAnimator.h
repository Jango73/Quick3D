
#pragma once

#include "quick3d_global.h"

// Qt
#include <QVector>
#include <QDateTime>

// Application
#include "CQ3DConstants.h"
#include "CAnimator.h"
#include "CAnimatorFrame.h"

//-------------------------------------------------------------------------------------------------
// D�clarations avanc�es

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CBasicAnimator : public CAnimator
{

public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//! Retourne une nouvelle instance de cet objet
	static CComponent* instanciator(C3DScene* pScene);

	//! Constructeur d'apr�s une sc�ne
	CBasicAnimator(C3DScene* pScene);

	//! Destructeur
	virtual ~CBasicAnimator();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//! Retourne une r�f�rence sur les pas de translations
	CAnimatorFrame* getTranslationFrame() { return m_pTranslationFrame; }

	//! Retourne une r�f�rence sur les pas de rotations
	CAnimatorFrame* getRotationFrame() { return m_pRotationFrame; }

	//-------------------------------------------------------------------------------------------------
	// M�thodes de contr�le
	//-------------------------------------------------------------------------------------------------

	//! Ajoute un pas de translation
	void addTranslationStep(CBasicAnimationStep value);

	//! Ajoute un pas de rotation
	void addRotationStep(CBasicAnimationStep value);

	//-------------------------------------------------------------------------------------------------
	// M�thodes h�rit�es
	//-------------------------------------------------------------------------------------------------

	//! Retourne le nom de classe de cet objet
	virtual QString getClassName() const { return ClassName_CBasicAnimator; }

	//! Charge les param�tres de cet objet d'apr�s le noeud XML fourni
	virtual void loadParameters(CXMLNode xComponent);

	//! Met l'objet � jour d'apr�s le temps �coul� depuis la derni�re image
	virtual void update(double dDeltaTime);

	//-------------------------------------------------------------------------------------------------
	// Propri�t�s
	//-------------------------------------------------------------------------------------------------

protected:

	CAnimatorFrame*		m_pTranslationFrame;
	CAnimatorFrame*		m_pRotationFrame;
};
