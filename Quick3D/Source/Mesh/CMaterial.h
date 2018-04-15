
#pragma once

// Qt
#include <QImage>
#include <QSharedData>
#include <QExplicitlySharedDataPointer>
#include <QtOpenGL>
#include <QtOpenGL/QGLShaderProgram>
#include <QGLFramebufferObject>

// qt-plus
#include "CMemoryMonitor.h"

// Application
#include "quick3d_global.h"
#include "CNamed.h"
#include "CVector4.h"
#include "CGeoloc.h"
#include "ILoadable.h"
#include "CQ3DConstants.h"
#include "CVertex.h"
#include "CTexture.h"
#include "CDumpable.h"

//-------------------------------------------------------------------------------------------------

#ifndef QSP
#define     QSP             QExplicitlySharedDataPointer
#define     QSP_CAST(T,O)   QSP<T>(dynamic_cast<T*>(O.data()))
#endif

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;
class CRenderContext;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CMaterial : public QObject, public QSharedData, public CNamed, public CDumpable, public ILoadable
{
    Q_OBJECT
    DECLARE_MEMORY_MONITORED

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CMaterial(C3DScene* pScene, QString sName = "");

    //!
    virtual ~CMaterial();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! Sets shininess factor
    void setShininess(double value) { m_dShininess = value; }

    //! Sets metalness factor
    void setMetalness(double value) { m_dMetalness = value; }

    //! Sets SSS factor (Sub-surface scattering)
    void setSSSFactor(double value) { m_dSSSFactor = value; }

    //! Sets SSS radius
    void setSSSRadius(double value) { m_dSSSRadius = value; }

    //! Sets IR factor
    void setIRFactor(double value) { m_dIRFactor = value; }

    //! Sets if this material has alpha
    void setHasAlpha(bool value) { m_bHasAlpha = value; }

    //! Sets the "use sky" flag
    void setUseSky(bool value) { m_bUseSky = value; }

    //! Sets the "bill board" flag
    void setBillBoard(bool value) { m_bBillBoard = value; }

    //! Sets the "lines" flag
    void setLines(bool value) { m_bLines = value; }

    //! Sets the time value for animated textures
    static void setTime(double value) { m_dTime = value; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns this object's class name
    virtual QString getClassName() const { return ClassName_CMaterial; }

    //! Returns a reference to the ambient color
    Math::CVector4& ambient() { return m_cAmbient; }

    //! Returns a reference to the diffuse color
    Math::CVector4& diffuse() { return m_cDiffuse; }

    //! Returns a reference to the specular color
    Math::CVector4& specular() { return m_cSpecular; }

    //! Returns a reference to the subdermal color
    Math::CVector4& subdermal() { return m_cSubdermal; }

    //! Returns the IR factor
    double IRFactor() const { return m_dIRFactor; }

    //! Returns true if this material has alpha
    bool hasAlpha() const;

    //! Returns the "use sky" flag
    bool useSky() const { return m_bUseSky; }

    //! Returns a reference to the list of diffuse textures
    QVector<CTexture*>& diffuseTextures() { return m_vDiffuseTextures; }

    //! Returns a reference to the list of normal textures
    QVector<CTexture*>& normalTextures() { return m_vNormalTextures; }

    //! Returns texture coordinates for a given geo loc
    virtual Math::CVector2 texCoords(const CGeoloc& gPosition, int iLevel);

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! Charge les propriétés de l'objet depuis un CXMLNode
    //! Loads the object's properties from a CXMLNode
    virtual void loadParameters(const QString& sBaseFile, const CXMLNode& xComponent);

    //! Solves the links of this object
    virtual void solveLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //! Deletes this object's links
    virtual void clearLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual void update(double dDeltaTime);

    //! Ajoute l'image spécifiée en texture diffuse
    void addDiffuseTexture(const QString& sBaseFile, const QString& sResourceName);

    //! Ajoute l'image spécifiée en texture diffuse
    void addDiffuseTexture(const QString& sName, const QImage& imgTexture);

    //! Ajoute l'image spécifiée en texture diffuse
    void addDynamicDiffuseTexture(const QString& sBaseFile, const QString& sResourceName);

    //!
    void addDynamicDiffuseTexture(const QString& sName, const QImage& imgTexture);

    //! Ajoute l'image spécifiée en texture normale
    void addNormalTexture(const QString& sBaseFile, const QString& sResourceName);

    //! Ajoute l'image spécifiée en texture normale
    void addNormalTexture(const QString& sName, const QImage& imgTexture);

    //! Créé une texture d'ombre portée
    void createShadowTexture();

    //! Détruit toutes les textures
    void clearTextures();

    //!
    void enableFrameBuffer();

    //!
    void disableFrameBuffer();

    //! Active ce matériau pour le rendu
    virtual QGLShaderProgram* activate(CRenderContext* pContext);

    //! Active les ombres portées
    virtual void activateShadow(CRenderContext* pContext);

    //! Applique des transformations à la géolocalisation donnée (ex: Mercator)
    virtual CGeoloc transformGeoloc(const CGeoloc& gPosition);

    //! Dumps contents to a stream
    virtual void dump(QTextStream& stream, int iIdent) Q_DECL_OVERRIDE;

    //!
    static Math::CVector4 black()	{ return Math::CVector4(0.0, 0.0, 0.0, 1.0); }
    static Math::CVector4 white()	{ return Math::CVector4(1.0, 1.0, 1.0, 1.0); }
    static Math::CVector4 red()		{ return Math::CVector4(1.0, 0.0, 0.0, 1.0); }

    static Math::CVector4 white_x5()	{ return Math::CVector4(5.0, 5.0, 5.0, 1.0); }

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    C3DScene*               m_pScene;
    Math::CVector4          m_cAmbient;
    Math::CVector4          m_cDiffuse;
    Math::CVector4          m_cSpecular;
    Math::CVector4          m_cSubdermal;
    double                  m_dSelfIllumination;
    double                  m_dShininess;
    double                  m_dMetalness;
    double                  m_dSSSFactor;
    double                  m_dSSSRadius;
    QVector<CTexture*>      m_vDiffuseTextures;
    QVector<CTexture*>      m_vNormalTextures;
    QGLFramebufferObject*   m_pShadowBuffer;
    double                  m_dIRFactor;
    bool                    m_bHasAlpha;
    bool                    m_bUseSky;
    bool                    m_bUseWaves;
    bool                    m_bBillBoard;
    bool                    m_bLines;

    static double           m_dTime;
};
