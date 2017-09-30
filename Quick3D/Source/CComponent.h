
#pragma once

// Qt
#include <QString>
#include <QVector>
#include <QGraphicsScene>
#include <QPainter>
#include <QImage>
#include <QSharedData>
#include <QtOpenGL>

// qt-plus
#include "CMemoryMonitor.h"

// Application
#include "quick3d_global.h"
#include "ILoadable.h"
#include "CQ3DConstants.h"
#include "CVector3.h"
#include "CMatrix4.h"
#include "CRay3.h"
#include "CGeolocalized.h"
#include "CXMLNode.h"
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

/* DataType */
#define GL_BYTE                         0x1400
#define GL_UNSIGNED_BYTE                0x1401
#define GL_SHORT                        0x1402
#define GL_UNSIGNED_SHORT               0x1403
#define GL_INT                          0x1404
#define GL_UNSIGNED_INT                 0x1405
#define GL_FLOAT                        0x1406
#define GL_2_BYTES                      0x1407
#define GL_3_BYTES                      0x1408
#define GL_4_BYTES                      0x1409
#define GL_DOUBLE                       0x140A

/* PixelFormat */
#define GL_COLOR_INDEX                  0x1900
#define GL_STENCIL_INDEX                0x1901
#define GL_DEPTH_COMPONENT              0x1902
#define GL_RED                          0x1903
#define GL_GREEN                        0x1904
#define GL_BLUE                         0x1905
#define GL_ALPHA                        0x1906
#define GL_RGB                          0x1907
#define GL_RGBA                         0x1908
#define GL_LUMINANCE                    0x1909
#define GL_LUMINANCE_ALPHA              0x190A
#define GL_TEXTURE_RECTANGLE_ARB        0x84F5
#define GL_CLAMP_TO_BORDER              0x812D
#define GL_UNSIGNED_INT_8_8_8_8         0x8035

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;
class CController;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CComponent
        : public QSharedData
        , public CNamed
        , public CGeolocalized
        , public CParented
        , public CExpendable
        , public CDumpable
        , public ILoadable
{
    DECLARE_MEMORY_MONITORED

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Returns a new instance of this object
    static CComponent* instantiator(C3DScene* pScene);

    //! Constructor using a scene
    CComponent(C3DScene* pScene);

    //! Destructor
    virtual ~CComponent();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! Sets the user defined tag
    void setTag(const QString& sValue);

    //! Sets the object's controller
    void setController(CController* pController);

    //! Définit si l'objet est visible à la caméra
    void setVisible(bool bValue);

    //! Définit si l'objet produit des ombres portées
    void setCastShadows(bool bValue);

    //! Définit si l'objet est affecté par les ombres portées
    void setReceiveShadows(bool bValue);

    //! Définit si l'objet est inclut dans le ray-tracing
    void setRaytracable(bool bValue);

    //! Définit si l'objet est sélectionné
    void setSelected(bool bValue);

    //! Définit le parent
    void setParent(QSP<CComponent> pParent);

    //! Définit la géolocalisation
    virtual void setGeoloc(CGeoloc gGeoloc) Q_DECL_OVERRIDE;

    //! Définit la position d'origine
    void setPosition(Math::CVector3 vPosition);

    //! Définit la rotation d'origine
    void setRotation(Math::CVector3 vRotation);

    //! Définit l'échelle d'origine
    void setScale(Math::CVector3 vScale);

    //! Définit la position animée
    void setAnimPosition(Math::CVector3 vPosition);

    //! Définit la rotation animée
    void setAnimRotation(Math::CVector3 vRotation);

    //! Définit l'échelle animée
    void setAnimScale(Math::CVector3 vScale);

    //! Définit si l'objet hérite des transformations de son parent
    void setInheritTransform(bool bValue);

    //!
    void setPreviousWorldTransform(const Math::CMatrix4& mTransform);

    //!
    void setStatus(double dValue);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns this object's class name
    virtual QString getClassName() const { return ClassName_CComponent; }

    //! Returns the user defined tag
    QString tag() const { return m_sTag; }

    //! Returns the object's qualified name
    QString qualifiedName();

    //! Returns the object's controller
    CController* controller();

    //! Is the object visible to the camera?
    bool isVisible() const;

    //! Est-ce que l'objet projette des ombres?
    bool castsShadows() const;

    //! Est-ce que l'objet peut recevoir des ombres portées?
    bool receivesShadows() const;

    //! Est-ce que l'objet est visible au ray-tracing?
    bool isRaytracable() const { return m_bRaytracable; }

    //! Est-ce que l'objet est sélectionné?
    bool isSelected() const { return m_bSelected; }

    //! Returns the scene to which this object belongs
    C3DScene* scene() const { return m_pScene; }

    //! Is the object a camera?
    virtual bool isCamera() const { return false; }

    //! Is the object a light?
    virtual bool isLight() const { return false; }

    //! Est-ce que l'objet est racine (n'a pas de parent)?
    virtual bool isRootObject() const { return m_sParentName == ""; }

    //! Est-ce que l'objet peut avoir une trajectoire?
    virtual bool isTrajectorable() const { return false; }

    //! Returns this object's parent
    virtual QSP<CComponent> parentComponent() const { return m_pParent; }

    //! Retourne les enfants
    QVector<QSP<CComponent> >& childComponents() { return m_vChildren; }

    //! Returns the object's root object
    QSP<CComponent> root();

    //! Returns the object's geo-location
    virtual CGeoloc geoloc() const Q_DECL_OVERRIDE;

    //! Returns the rotation in the ECEF frame (Earth-centered earth-fixed)
    virtual Math::CVector3 ECEFRotation() const;

    //! Returns the position
    virtual Math::CVector3 position() const;

    //! Returns the rotation
    virtual Math::CVector3 rotation() const;

    //! Returns the scale
    virtual Math::CVector3 scale() const;

    //! Returns the animated position
    virtual Math::CVector3 animPosition() const { return m_vAnimPosition; }

    //! Returns the animated rotation
    virtual Math::CVector3 animRotation() const { return m_vAnimRotation; }

    //! Returns the animated scale
    virtual Math::CVector3 animScale() const { return m_vAnimScale; }

    //! Retourne la matrice de transformation monde
    Math::CMatrix4 worldTransform() const;

    //! Retourne l'inverse de la matrice de transformation monde
    Math::CMatrix4 worldTransformInverse() const;

    //!
    Math::CMatrix4 previousWorldTransform() const;

    //! Retourne la position monde
    Math::CVector3 worldPosition() const;

    //! Retourne la rotation monde
    Math::CVector3 worldRotation() const;

    //! Retourne la direction monde (vers où l'objet pointe)
    Math::CVector3 worldDirection() const;

    //! Retourne le facteur d'échelle monde
    Math::CVector3 worldScale() const;

    //!
    double status() const;

    //!
    static void incComponentCounter(QString sClassName);

    //!
    static void decComponentCounter(QString sClassName);

    //! Retourne le nombre d'instances de cette classe dans l'application
    static int getNumComponents() { return m_iNumComponents; }

    //-------------------------------------------------------------------------------------------------
    // Operators
    //-------------------------------------------------------------------------------------------------

    //! Assign operator
    CComponent& operator = (const CComponent& target);

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Loads this object's parameters
    virtual void loadParameters(const QString& sBaseFile, const CXMLNode& xComponent);

    //! Solves the links of this object
    virtual void solveLinks(C3DScene* pScene);

    //! Deletes this object's links
    virtual void clearLinks(C3DScene* pScene);

    //! Recherche un composant dans la hiérarchie de cet objet
    virtual QSP<CComponent> findComponent(QString sName, QSP<CComponent> pCaller = QSP<CComponent>(nullptr));

    //! Cette méthode permet à l'objet de rajouter des items dans la scène qui le contient
    virtual void addItems(C3DScene* pScene);

    //! Permet à l'objet de mettre à jour le contexte de rendu
    virtual void updateContext(CRenderContext* pContext);

    //! Permet à l'objet de mettre à jour les items qu'il a crée dans la scène qui le contient
    virtual void updateItems(C3DScene* pScene);

    //! Dans cette méthode, l'objet doit faire son rendu
    virtual void paint(CRenderContext* pContext);

    //! Appel de la méthode paint des enfants
    virtual void postPaint(CRenderContext* pContext);

    //! Updates this object using the elapsed time since last update
    virtual void update(double dDeltaTime);

    //! Calls the update method for child components
    virtual void postUpdate(double dDeltaTime);

    //!
    virtual void updateTexture(CTexture* pTexture, double dDeltaTime);

    //! Returns the local bounding box
    virtual CBoundingBox bounds();

    //! Returns the world bounding box
    virtual CBoundingBox worldBounds();

    //! Ray intersection
    virtual Math::RayTracingResult intersect(Math::CRay3 ray);

    //! Flips all normals of this component (does something if component is a mesh)
    virtual void flipNormals();

    //! Explicitely set world transform (which is otherwise computed automatically)
    virtual void setWorldTransform(const Math::CMatrix4& value);

    //! Transforms all vertices of this component (does something if component is a mesh)
    void transformVertices(const Math::CMatrix4& matrix);

    //! Compute the world transform matrix
    void computeWorldTransform();

    //! Saves the world transform matrix
    void saveTransform();

    //! Restores the world transform matrix
    void loadTransform();

    //! Converts a local euler rotation to the equivalent in the ECEF frame
    Math::CVector3 toECEFRotation(Math::CVector3 vRotation) const;

    //!
    Math::CVector3 fromFrame(CComponent* pFrom, Math::CVector3 vPosition) const;

    //! Makes this component look at a target, using Z axis as forward axis
    void lookAt(CComponent* pTarget);

    //! Copie la matrice de l'objet cible dans la matrice de cet objet
    void copyTransform(const CComponent* pTarget);

    //! Dumps contents to a stream
    virtual void dump(QTextStream& stream, int iIdent) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Static methods
    //-------------------------------------------------------------------------------------------------

    static QMap<QString, int> componentCounter() { return m_mComponentCounter; }

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

private:

    Math::CVector3              m_vECEFRotation;                // Object's rotation in the ECEF frame (Earth-centered earth-fixed)
    Math::CVector3              m_vPosition;                    // Object's position in the NOLL frame (North-oriented local-level)
    Math::CVector3              m_vRotation;                    // Object's rotation (euler) in the NOLL frame
    Math::CVector3              m_vScale;                       // Object's scale in the NOLL frame
    Math::CVector3              m_vAnimPosition;                // Object's animated position in the local frame
    Math::CVector3              m_vAnimRotation;                // Object's animated rotation (euler) in the local frame
    Math::CVector3              m_vAnimScale;                   // Object's animated scale in the local frame
    Math::CVector3              m_vRotationFactor;              // Rotation factors (allows axis locking)
    Math::CVector3              m_vRotationMinimum;             // Minimum rotation
    Math::CVector3              m_vRotationMaximum;             // Maximum rotation

protected:

    QString                     m_sTag;                         // User defined tag
    C3DScene*                   m_pScene;                       // The scene to which the object belongs
    CController*                m_pController;
    Math::CMatrix4              m_mWorldTransform;              // World transform of the object
    Math::CMatrix4              m_mWorldTransformInverse;       // World inverse transform of the object
    Math::CMatrix4              m_mPreviousWorldTransform;
    QVector<CHeightField*>      m_pFields;                      // The height fields of the object
    bool                        m_bVisible;                     // Is the object visible?
    bool                        m_bCastShadows;                 // Does the object cast shadows?
    bool                        m_bReceiveShadows;              // Does the object receive shadows?
    bool                        m_bRaytracable;                 // Should the object be considered in ray-tracing methods?
    bool                        m_bInheritTransform;            // Should the object inherit its parent's transform?
    bool                        m_bSelected;                    // Is the object selected?

    double                      m_dStatus;                      // Status of the object (0.0 = Out of service, 1.0 = Functional)

    // Used by saveTransform() and loadTransform()

    CGeoloc                     m_gSavedGeoloc;
    Math::CVector3              m_vSavedPosition;
    Math::CVector3              m_vSavedRotation;
    Math::CVector3              m_vSavedScale;

    // Shared data

    QSP<CComponent>             m_pParent;                      // Object's parent
    QVector<QSP<CComponent> >   m_vChildren;                    // Object's children

    static int                  m_iNumComponents;
    static QMap<QString, int>   m_mComponentCounter;
};
