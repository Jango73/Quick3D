
#pragma once

#include "quick3d_global.h"

// Qt
#include <QString>
#include <QVector>
#include <QGraphicsScene>
#include <QPainter>
#include <QImage>
#include <QSharedData>
#include <QExplicitlySharedDataPointer>
#include <QtOpenGL>

// Application
#include "CVector3.h"
#include "CMatrix4.h"
#include "CRay3.h"
#include "CGeoloc.h"
#include "CXMLNode.h"
#include "ILoadable.h"
#include "CQ3DConstants.h"
#include "CGLExtension.h"
#include "CRenderContext.h"
#include "CNamed.h"
#include "CParented.h"
#include "CExpendable.h"
#include "CDumpable.h"
#include "CBoundingBox.h"
#include "CHeightField.h"
#include "CTexture.h"

//-------------------------------------------------------------------------------------------------

#define     QSP             QExplicitlySharedDataPointer
#define     QSP_CAST(T,O)   QSP<T>(dynamic_cast<T*>(O.data()))

//-------------------------------------------------------------------------------------------------

/* DataType */
#define GL_BYTE                           0x1400
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_SHORT                          0x1402
#define GL_UNSIGNED_SHORT                 0x1403
#define GL_INT                            0x1404
#define GL_UNSIGNED_INT                   0x1405
#define GL_FLOAT                          0x1406
#define GL_2_BYTES                        0x1407
#define GL_3_BYTES                        0x1408
#define GL_4_BYTES                        0x1409
#define GL_DOUBLE                         0x140A

/* PixelFormat */
#define GL_COLOR_INDEX					0x1900
#define GL_STENCIL_INDEX				0x1901
#define GL_DEPTH_COMPONENT				0x1902
#define GL_RED							0x1903
#define GL_GREEN						0x1904
#define GL_BLUE							0x1905
#define GL_ALPHA						0x1906
#define GL_RGB							0x1907
#define GL_RGBA							0x1908
#define GL_LUMINANCE					0x1909
#define GL_LUMINANCE_ALPHA				0x190A
#define GL_TEXTURE_RECTANGLE_ARB		0x84F5
#define GL_CLAMP_TO_BORDER				0x812D
#define GL_UNSIGNED_INT_8_8_8_8			0x8035

//-------------------------------------------------------------------------------------------------

class C3DScene;
class CController;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CComponent : public QSharedData, public CNamed, public CParented, public CExpendable, public CDumpable, public ILoadable
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Retourne une nouvelle instance de cet objet
    static CComponent* instanciator(C3DScene* pScene);

    //! Constructeur d'apr�s une sc�ne
    CComponent(C3DScene* pScene);

    //! Destructor
    virtual ~CComponent();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! D�finit le tag de l'objet
    void setTag(const QString& sValue);

    //!
    void setController(CController* pController);

    //! D�finit si l'objet est visible � la cam�ra
    void setVisible(bool bValue);

    //! D�finit si l'objet produit des ombres port�es
    void setCastShadows(bool bValue);

    //! D�finit si l'objet est affect� par les ombres port�es
    void setReceiveShadows(bool bValue);

    //! D�finit si l'objet est inclut dans le ray-tracing
    void setRaytracable(bool bValue);

    //! D�finit si l'objet est s�lectionn�
    void setSelected(bool bValue);

    //! D�finit le parent
    void setParent(QSP<CComponent> pParent);

    //! D�finit la g�olocalisation
    virtual void setGeoloc(CGeoloc gGeoloc);

    //! D�finit la position d'origine
    void setOriginPosition(Math::CVector3 Position);

    //! D�finit la rotation d'origine
    void setOriginRotation(Math::CVector3 Rotation);

    //! D�finit l'�chelle d'origine
    void setOriginScale(Math::CVector3 Scale);

    //! D�finit la position anim�e
    void setPosition(Math::CVector3 Position);

    //! D�finit la rotation anim�e
    void setRotation(Math::CVector3 Rotation);

    //! D�finit l'�chelle anim�e
    void setScale(Math::CVector3 Scale);

    //! D�finit si l'objet h�rite des transformations de son parent
    void setInheritTransform(bool bValue);

    //!
    void setPreviousWorldTransform(const Math::CMatrix4& mTransform);

    //!
    void setStatus(double dValue);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    QString getTag() const { return m_sTag; }

    //!
    QString getQualifiedName();

    //!
    CController* getController();

    //! Est-ce que l'objet est visible (doit �tre rendu dans la sc�ne)?
    bool isVisible() const;

    //! Est-ce que l'objet projette des ombres?
    bool castsShadows() const;

    //! Est-ce que l'objet peut recevoir des ombres port�es?
    bool receivesShadows() const;

    //! Est-ce que l'objet est visible au ray-tracing?
    bool isRaytracable() const { return m_bRaytracable; }

    //! Est-ce que l'objet est s�lectionn�?
    bool isSelected() const { return m_bSelected; }

    //! Retourne le nom de cette classe
    virtual QString getClassName() const { return ClassName_CComponent; }

    //! Retourne la sc�ne � laquelle l'objet est attach�
    C3DScene* getScene() const { return m_pScene; }

    //! Est-ce que l'objet est une cam�ra?
    virtual bool isCamera() const { return false; }

    //! Est-ce que l'objet est une lumi�re?
    virtual bool isLight() const { return false; }

    //! Est-ce que l'objet est racine (n'a pas de parent)?
    virtual bool isRootObject() const { return m_sParentName == ""; }

    //! Est-ce que l'objet peut avoir une trajectoire?
    virtual bool isTrajectorable() const { return false; }

    //! Retourne le parent
    virtual QSP<CComponent> getParent() const { return m_pParent; }

    //! Retourne les enfants
    QVector<QSP<CComponent> >& getChildren() { return m_vChildren; }

    //! Retourne l'objet racine
    QSP<CComponent> getRoot();

    //! Retourne la g�olocalisation
    virtual CGeoloc getGeoloc() const;

    //! Retourne la rotation dans le rep�re ECEF (Earth-centered earth-fixed)
    virtual Math::CVector3 getECEFRotation() const { return m_vECEFRotation; }

    //! Retourne la position cart�sienne d'origine
    virtual Math::CVector3 getOriginPosition() const;

    //! Retourne la rotation d'origine
    virtual Math::CVector3 getOriginRotation() const;

    //! Retourne la facteur d'�chelle d'origine
    virtual Math::CVector3 getOriginScale() const { return m_vOriginScale; }

    //! Retourne la position cart�sienne anim�e
    virtual Math::CVector3 getPosition() const { return m_vPosition; }

    //! Retourne la rotation anim�e
    virtual Math::CVector3 getRotation() const { return m_vRotation; }

    //! Retourne la facteur d'�chelle
    virtual Math::CVector3 getScale() const { return m_vScale; }

    //! Retourne la matrice de transformation monde
    Math::CMatrix4 getWorldTransform() const;

    //! Retourne l'inverse de la matrice de transformation monde
    Math::CMatrix4 getWorldTransformInverse() const;

    //!
    Math::CMatrix4 getPreviousWorldTransform() const;

    //! Retourne la position monde
    Math::CVector3 getWorldPosition() const;

    //! Retourne la rotation monde
    Math::CVector3 getWorldRotation() const;

    //! Retourne la direction monde (vers o� l'objet pointe)
    Math::CVector3 getWorldDirection() const;

    //! Retourne le facteur d'�chelle monde
    Math::CVector3 getWorldScale() const;

    //!
    double getStatus() const;

    //! Retourne le nombre d'instances de cette classe dans l'application
    static int getNumComponents() { return m_iNumComponents; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Op�rateur d'assignation
    CComponent& operator = (const CComponent& target);

    //! Charge les param�tres de cet objet
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent);

    //! Recherche les liens de cet objet
    virtual void solveLinks(C3DScene* pScene);

    //! Efface les liens de cet objet
    virtual void clearLinks(C3DScene* pScene);

    //! Recherche un composant dans la hi�rarchie de cet objet
    virtual QSP<CComponent> findComponent(QString sName, QSP<CComponent> pCaller = QSP<CComponent>(NULL));

    //! Cette m�thode permet � l'objet de rajouter des items dans la sc�ne qui le contient
    virtual void addItems(C3DScene* pScene);

    //! Permet � l'objet de mettre � jour le contexte de rendu
    virtual void updateContext(CRenderContext* pContext);

    //! Permet � l'objet de mettre � jour les items qu'il a cr�e dans la sc�ne qui le contient
    virtual void updateItems(C3DScene* pScene);

    //! Dans cette m�thode, l'objet doit faire son rendu
    virtual void paint(CRenderContext* pContext);

    //! Appel de la m�thode paint des enfants
    virtual void postPaint(CRenderContext* pContext);

    //! M�thode de mise � jour avec temps delta depuis la derni�re it�ration
    virtual void update(double dDeltaTime);

    //! Appel de la m�thode update des enfants
    virtual void postUpdate(double dDeltaTime);

    //!
    virtual void updateTexture(CTexture* pTexture, double dDeltaTime);

    //! Retourne la boite englobante locale
    virtual CBoundingBox bounds();

    //! Retourne la boite englobante "monde"
    virtual CBoundingBox worldBounds();

    //! Calcul d'intersection avec un rayon
    virtual Math::RayTracingResult intersect(Math::CRay3 ray);

    //! Inverse les vecteurs normaux des polygones
    virtual void flipNormals();

    //! D�finit les tranformations monde (normalement calcul�es automatiquement)
    virtual void setWorldTransform(const Math::CMatrix4& value);

    //! Transforme les sommets
    void transformVertices(const Math::CMatrix4& matrix);

    //! Dump du contenu dans un flux
    virtual void dump(QTextStream& stream, int iIdent);

    //! Calcule la matrice de transformation en coordonn�es "monde"
    void computeWorldTransform();

    //! Sauvegarde la matrice de transformation en coordonn�es "monde"
    void saveTransform();

    //! Restaure la matrice de transformation en coordonn�es "monde"
    void loadTransform();

    //!
    Math::CVector3 toECEFRotation(Math::CVector3 vRotation) const;

    //! Oriente cet objet pour qu'il "regarde" vers l'objet cible (axe Z align� vers l'objet cible)
    void lookAt(CComponent* pTarget);

    //! Copie la matrice de l'objet cible dans la matrice de cet objet
    void copyTransform(const CComponent* pTarget);

    //-------------------------------------------------------------------------------------------------
    // Static methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

private:

    CGeoloc                     m_gGeoloc;						// G�olocalisation de l'objet
    Math::CVector3              m_vECEFRotation;				// Rotation de l'objet dans le rep�re ECEF (Earth-centered earth-fixed)
    Math::CVector3              m_vOriginPosition;				// Position de l'objet dans le rep�re NOLL (North-oriented local-level)
    Math::CVector3              m_vOriginRotation;				// Rotation de l'objet (euleur) dans le rep�re NOLL
    Math::CVector3              m_vOriginScale;					// Echelle de l'objet dans le rep�re NOLL
    Math::CVector3              m_vPosition;					// Position anim�e de l'objet dans son rep�re local
    Math::CVector3              m_vRotation;					// Rotation anim�e de l'objet (euleur) dans son rep�re local
    Math::CVector3              m_vScale;						// Echelle anim�e de l'objet dans son rep�re local
    Math::CVector3              m_vRotationFactor;				// Verrouillages de rotation
    Math::CVector3              m_vRotationMinimum;				// Limites minimum de rotation
    Math::CVector3              m_vRotationMaximum;				// Limites maximum de rotation

protected:

    QString                     m_sTag;
    C3DScene*                   m_pScene;						// La sc�ne � laquelle l'objet appartient
    CController*                m_pController;
    Math::CMatrix4              m_mWorldTransform;				// Transformation "monde" de l'objet
    Math::CMatrix4              m_mWorldTransformInverse;		// Transformation "monde" inverse de l'objet
    Math::CMatrix4              m_mPreviousWorldTransform;
    CGeoloc                     m_gSavedGeoloc;
    Math::CVector3              m_vSavedOriginPosition;
    Math::CVector3              m_vSavedOriginRotation;
    Math::CVector3              m_vSavedOriginScale;
    QVector<CHeightField*>      m_pFields;						// Les champs de hauteurs dont d�pend cet objet
    bool                        m_bVisible;						// Est visible?
    bool                        m_bCastShadows;					// G�n�re des ombres port�es
    bool                        m_bReceiveShadows;				// Recoit des ombres port�es
    bool                        m_bRaytracable;					// Ce composant doit-il �tre trait� en ray-tracing?
    bool                        m_bInheritTransform;			// L'objet h�rite des transformations de son parent si vrai
    bool                        m_bSelected;					// Est-ce que l'objet est s�lectionn�

    double                      m_dStatus;						// Etat de l'objet (0.0 = HS, 1.0 = Fonctionnel)

    // Shared data

    QSP<CComponent>             m_pParent;						// Parent de l'objet
    QVector<QSP<CComponent> >   m_vChildren;					// Liste d'objets enfants

    static int                  m_iNumComponents;
};
