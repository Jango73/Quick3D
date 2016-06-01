
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
// Déclarations avancées

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

	//! Constructeur d'après une scène
	CBasicAnimator(C3DScene* pScene);

	//! Destructeur
	virtual ~CBasicAnimator();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//! Retourne une référence sur les pas de translations
	CAnimatorFrame* getTranslationFrame() { return m_pTranslationFrame; }

	//! Retourne une référence sur les pas de rotations
	CAnimatorFrame* getRotationFrame() { return m_pRotationFrame; }

	//-------------------------------------------------------------------------------------------------
	// Méthodes de contrôle
	//-------------------------------------------------------------------------------------------------

	//! Ajoute un pas de translation
	void addTranslationStep(CBasicAnimationStep value);

	//! Ajoute un pas de rotation
	void addRotationStep(CBasicAnimationStep value);

	//-------------------------------------------------------------------------------------------------
	// Méthodes héritées
	//-------------------------------------------------------------------------------------------------

	//! Retourne le nom de classe de cet objet
	virtual QString getClassName() const { return ClassName_CBasicAnimator; }

	//! Charge les paramètres de cet objet d'après le noeud XML fourni
	virtual void loadParameters(CXMLNode xComponent);

	//! Met l'objet à jour d'après le temps écoulé depuis la dernière image
	virtual void update(double dDeltaTime);

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

protected:

	CAnimatorFrame*		m_pTranslationFrame;
	CAnimatorFrame*		m_pRotationFrame;
};
