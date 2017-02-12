
#pragma once

#include "quick3d_global.h"

// Qt
#include <QImage>
#include <QSharedData>
#include <QExplicitlySharedDataPointer>
#include <QtOpenGL>
#include <QtOpenGL/QGLShaderProgram>
#include <QGLFramebufferObject>

// Application
#include "CVector4.h"
#include "CGeoloc.h"
#include "ILoadable.h"
#include "CQ3DConstants.h"
#include "CVertex.h"
#include "CTexture.h"

//-------------------------------------------------------------------------------------------------

#ifndef QSP
#define     QSP             QExplicitlySharedDataPointer
#define     QSP_CAST(T,O)   QSP<T>(dynamic_cast<T*>(O.data()))
#endif

//-------------------------------------------------------------------------------------------------

class C3DScene;
class CRenderContext;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CMaterial : public QObject, public QSharedData, public ILoadable
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    // Constructors and destructor
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

    //!
    void setHasAlpha(bool value) { m_bHasAlpha = value; }

    //! D�finit si ce mat�riau repr�sente le ciel
    void setUseSky(bool value) { m_bUseSky = value; }

    //!
    void setBillBoard(bool value) { m_bBillBoard = value; }

    //!
    void setLines(bool value) { m_bLines = value; }

    //! D�finit la valeur temps utilis�e pour animer les textures
    static void setTime(double value) { m_dTime = value; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Retourne le nom de la classe
    virtual QString getClassName() const { return ClassName_CMaterial; }

    //! Retourne le nom du mat�riau
    QString name() const { return m_sName; }

    //! Retourne la couleur ambiente
    Math::Vector4& ambient() { return m_cAmbient; }

    //! Retourne la couleur diffuse
    Math::Vector4& diffuse() { return m_cDiffuse; }

    //! Retourne la couleur sp�culaire
    Math::Vector4& specular() { return m_cSpecular; }

    //! Retourne la couleur sp�culaire
    Math::Vector4& subdermal() { return m_cSubdermal; }

    //! Retourne le taux de r�lection
    double reflection() const { return m_dReflection; }

    //! Retourne le taux de reflectance IR
    double IRFactor() const { return m_dIRFactor; }

    //! Est-ce un mat�riau de ciel?
    bool useSky() const { return m_bUseSky; }

    //!
    bool hasAlpha() const;

    //!
    QVector<CTexture*>& diffuseTextures() { return m_vDiffuseTextures; }

    //! Retourne des coordonn�es de texture pour une g�olocalisation donn�e
    virtual Math::CVector2 texCoords(const CGeoloc& gPosition, int iLevel);

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! Charge les propri�t�s de l'objet depuis un CXMLNode
    //! Loads the object's properties from a CXMLNode
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent);

    //! Recherche les liens de cet objet
    virtual void solveLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //! Efface les liens de cet objet
    virtual void clearLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual void update(double dDeltaTime);

    //! Ajoute l'image sp�cifi�e en texture diffuse
    void addDiffuseTexture(const QString& sBaseFile, const QString& sResourceName);

    //! Ajoute l'image sp�cifi�e en texture diffuse
    void addDiffuseTexture(const QString& sName, const QImage& imgTexture);

    //! Ajoute l'image sp�cifi�e en texture diffuse
    void addDynamicDiffuseTexture(const QString& sBaseFile, const QString& sResourceName);

    //!
    void addDynamicDiffuseTexture(const QString& sName, const QImage& imgTexture);

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
    virtual void activateShadow(CRenderContext* pContext);

    //! Applique des transformations � la g�olocalisation donn�e (ex: Mercator)
    virtual CGeoloc transformGeoloc(const CGeoloc& gPosition);

    //!
    static Math::Vector4 black()	{ return Math::Vector4(0.0, 0.0, 0.0, 1.0); }
    static Math::Vector4 white()	{ return Math::Vector4(1.0, 1.0, 1.0, 1.0); }
    static Math::Vector4 red()		{ return Math::Vector4(1.0, 0.0, 0.0, 1.0); }

    static Math::Vector4 white_x5()	{ return Math::Vector4(5.0, 5.0, 5.0, 1.0); }

    //-------------------------------------------------------------------------------------------------
    // Properties
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    C3DScene*               m_pScene;
    QString                 m_sName;
    Math::Vector4           m_cAmbient;
    Math::Vector4           m_cDiffuse;
    Math::Vector4           m_cSpecular;
    Math::Vector4           m_cSubdermal;
    double                  m_dSelfIllumination;
    double                  m_dShininess;
    double                  m_dReflection;
    double                  m_dReflectionSteepness;
    double                  m_dSSSFactor;
    double                  m_dSSSRadius;
    QVector<CTexture*>      m_vDiffuseTextures;
    QGLFramebufferObject*   m_pShadowBuffer;
    double                  m_dIRFactor;
    bool                    m_bHasAlpha;
    bool                    m_bUseSky;
    bool                    m_bUseWaves;
    bool                    m_bBillBoard;
    bool                    m_bLines;

    static double           m_dTime;
};
