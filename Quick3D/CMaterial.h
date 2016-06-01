
#pragma once

#include "quick3d_global.h"

// Qt
#include <QImage>
#include <QtOpenGL>
#include <QtOpenGL/QGLShaderProgram>
#include <QGLFramebufferObject>

// Fondations
#include "CVector4.h"
#include "CGeoloc.h"

// Application
#include "CQ3DConstants.h"
#include "CVertex.h"
#include "CRenderContext.h"
#include "CTexture.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CMaterial : public QObject
{
	Q_OBJECT

public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//!
	CMaterial(C3DScene* pScene, QString sName = "");

	//!
	virtual ~CMaterial();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//! D�finit le nom du mat�riau
	void setName(const QString& sName) { m_sName = sName; }

	//! D�finit le taux de brillance
	void setShininess(double value) { m_dShininess = value; }

	//! D�finit le taux de r�flection
	void setReflection(double value) { m_dReflection = value; }

	//!
	void setSSSFactor(double value) { m_dSSSFactor = value; }

	//!
	void setSSSRadius(double value) { m_dSSSRadius = value; }

	//! D�finit le taux de reflectance IR
	void setIRFactor(double value) { m_dIRFactor = value; }

	//! D�finit si ce mat�riau repr�sente le ciel
	void setUseSky(bool value) { m_bUseSky = value; }

	//! D�finit la valeur temps utilis�e pour animer les textures
	static void setTime(double value) { m_dTime = value; }

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//! Retourne le nom de la classe
	virtual QString getClassName() const { return ClassName_CMaterial; }

	//! Retourne le nom du mat�riau
	QString getName() const { return m_sName; }

	//! Retourne la couleur ambiente
	Math::Vector4& getAmbient() { return m_cAmbient; }

	//! Retourne la couleur diffuse
	Math::Vector4& getDiffuse() { return m_cDiffuse; }

	//! Retourne la couleur sp�culaire
	Math::Vector4& getSpecular() { return m_cSpecular; }

	//! Retourne la couleur sp�culaire
	Math::Vector4& getSubdermal() { return m_cSubdermal; }

	//! Retourne le taux de r�lection
	double getReflection() { return m_dReflection; }

	//! Retourne le taux de reflectance IR
	double getIRFactor() { return m_dIRFactor; }

	//! Est-ce un mat�riau de ciel?
	bool getUseSky() { return m_bUseSky; }

	//!
	QVector<CTexture*>& getDiffuseTextures() { return m_vDiffuseTextures; }

	//! Retourne des coordonn�es de texture pour une g�olocalisation donn�e
	virtual Math::CVector2 getTexCoords(const Math::CGeoloc& gPosition, int iLevel);

	//-------------------------------------------------------------------------------------------------
	// M�thodes de contr�le
	//-------------------------------------------------------------------------------------------------

	//!
	virtual void update(double dDeltaTime);

	//! Ajoute l'image sp�cifi�e en texture diffuse
	void addDiffuseTexture(QString sResourceName);

	//! Ajoute l'image sp�cifi�e en texture diffuse
	void addDiffuseTexture(QString sName, const QImage& imgTexture);

	//! Ajoute l'image sp�cifi�e en texture diffuse
	void addDynamicDiffuseTexture(QString sResourceName);

	//!
	void addDynamicDiffuseTexture(QString sName, const QImage& imgTexture);

	//! Cr�� une texture d'ombre port�e
	void createShadowTexture();

	//! D�truit toutes les textures
	void clearTextures();

	//!
	void enableFrameBuffer();

	//!
	void disableFrameBuffer();

	//! Active ce mat�riau pour le rendu
	virtual QGLShaderProgram* activate(CRenderContext* pContext);

	//! Active les ombres port�es
	virtual void activateShadow(CRenderContext* pContext, QGLShaderProgram* pProgram);

	//! Applique des transformations � la g�olocalisation donn�e (ex: Mercator)
	virtual Math::CGeoloc transformGeoloc(const Math::CGeoloc& gPosition);

	//!
	static Math::Vector4 black()	{ return Math::Vector4(0.0, 0.0, 0.0, 1.0); }
	static Math::Vector4 white()	{ return Math::Vector4(1.0, 1.0, 1.0, 1.0); }
	static Math::Vector4 red()		{ return Math::Vector4(1.0, 0.0, 0.0, 1.0); }

	static Math::Vector4 white_x5()	{ return Math::Vector4(5.0, 5.0, 5.0, 1.0); }

	//-------------------------------------------------------------------------------------------------
	// Propri�t�s
	//-------------------------------------------------------------------------------------------------

protected:

	C3DScene*				m_pScene;
	QString					m_sName;
	Math::Vector4			m_cAmbient;
	Math::Vector4			m_cDiffuse;
	Math::Vector4			m_cSpecular;
	Math::Vector4			m_cSubdermal;
	double					m_dSelfIllumination;
	double					m_dShininess;
	double					m_dReflection;
	double					m_dReflectionSteepness;
	double					m_dSSSFactor;
	double					m_dSSSRadius;
	QVector<CTexture*>		m_vDiffuseTextures;
	QGLFramebufferObject*	m_pShadowBuffer;
	double					m_dIRFactor;
	bool					m_bUseSky;
	bool					m_bUseWaves;

	static double			m_dTime;
};
