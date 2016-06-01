
#pragma once

#include "quick3d_global.h"

// Qt
#include <QImage>

// Fondations
#include "CVector2.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CSphericalTextureSet
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//! Constructeur par défaut
	CSphericalTextureSet();

	//! Constructeur avec paramètres
	CSphericalTextureSet(
		Math::CVector2 vStartAngles,
		Math::CVector2 vEndAngles,
		int iPanCount,
		int iTiltCount,
		QSize sTextureSize
		);

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//!
	void setStartAngles(const Math::CVector2& value) { m_vStartAngles = value; }

	//!
	void setEndAngles(const Math::CVector2& value) { m_vEndAngles = value; }

	//!
	void setPanCount(int value) { m_iPanCount = value; }

	//!
	void setTiltCount(int value) { m_iTiltCount = value; }

	//!
	void setTextureSize(QSize value) { m_sTextureSize = value; }

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//!
	Math::CVector2 getStartAngles() const {return m_vStartAngles; }

	//!
	Math::CVector2 getEndAngles() const {return m_vEndAngles; }

	//!
	int getPanCount() const { return m_iPanCount; }

	//!
	int getTiltCount() const { return m_iTiltCount; }

	//!
	QSize getTextureSize() { return m_sTextureSize; }

	//! Retourne la liste d'images
	QVector<QVector<QImage> >& getImages() { return m_vImages; }

	//! Retourne la liste d'images
	const QVector<QVector<QImage> >& getImages() const { return m_vImages; }

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

protected:

	Math::CVector2				m_vStartAngles;
	Math::CVector2				m_vEndAngles;
	int							m_iPanCount;
	int							m_iTiltCount;
	QSize						m_sTextureSize;
	QVector<QVector<QImage> >	m_vImages;		// List d'images rangées par tilt puis pan
};

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CImageUtilities
{
public:

	//-------------------------------------------------------------------------------------------------
	// Méthodes statiques
	//-------------------------------------------------------------------------------------------------

	//! Créé un jeu de textures à mapper sur une sphère d'après une image panoramique
	static void createPanoramicTextureSet(
		const QImage& imgSourceImage,
		CSphericalTextureSet& tParams
		);

	//!
	static void blitPart(QImage& imgDest, QPoint pDestPosition, const QImage& imgSource, QPoint pSourcePosition, QSize sSize);

	//!
	static void blitWhole(QImage& imgDest, QPoint pDestPosition, const QImage& imgSource);
};
