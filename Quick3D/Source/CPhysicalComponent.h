
#pragma once

#include "quick3d_global.h"

// Application
#include "CQ3DConstants.h"
#include "CComponent.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class CComponent;
class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CPhysicalComponent : public CComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Enum�rateurs
    //-------------------------------------------------------------------------------------------------

    enum ECollisionType
    {
        ctSphere,
        ctBox,
        ctMesh
    };

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Retourne une nouvelle instance de cet objet
    static CComponent* instanciator(C3DScene* pScene);

    //! Constructor using a scene
    CPhysicalComponent(C3DScene* pScene);

    //! Destructor
    virtual ~CPhysicalComponent();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! D�finit la train�e
    void setDrag_norm(double value) { m_dDrag_norm = value; }

    //! D�finit la train�e angulaire
    void setAngularDrag_norm(double value) { m_dAngularDrag_norm = value; }

    //! D�finit la masse en kilogrammes
    void setMass_kg(double value) { m_dMass_kg = value; }

    //! D�finit la v�locit� en m�tres/seconde
    void setVelocity_ms(Math::CVector3 value) { m_vVelocity_ms = value; }

    //! D�finit la v�locit� angulaire en radians/seconde
    void setAngularVelocity_rs(Math::CVector3 value) { m_vAngularVelocity_rs = value; }

    //! D�finit l'accel�ration en m�tres/seconde/seconde
    void setAcceleration_mss(Math::CVector3 value) { m_vSummedForces_mss = value; }

    //! D�finit l'accel�ration angulaire en m�tres/seconde/seconde
    void setAngularAcceleration_rss(Math::CVector3 value) { m_vSummedTorques_rss = value; }

    //! D�finit si les collisions sont activ�es
    void setCollisions(bool bEnabled);

    //! D�finit le type d'objet de collision � utiliser
    void setCollisionType (ECollisionType eType);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Retourne le nom de cette classe
    virtual QString getClassName() const { return ClassName_CPhysicalComponent; }

    //! Retourne la train�e normalis�e
    double drag_norm() const { return m_dDrag_norm; }

    //! Retourne la train�e angulaire normalis�e
    double angularDrag_norm() const { return m_dAngularDrag_norm; }

    //! Retourne la masse en kilogrammes
    double mass_kg() const { return m_dMass_kg; }

    //! Retourne la masse totale (enfants compris) en kilogrammes
    double totalMass_kg() const;

    //! Retourne le vecteur v�locit� en m�tres/seconde
    Math::CVector3 velocity_ms() const { return m_vVelocity_ms; }

    //! Retourne la v�locit� angulaire en radians/seconde
    Math::CVector3 angularVelocity_rs() const { return m_vAngularVelocity_rs; }

    //! Retourne toutes les forces en m�tres/seconde/seconde
    Math::CVector3 summedForces_mss()  const { return m_vSummedForces_mss; }

    //! Retourne tous les couples en radians/seconde/seconde
    Math::CVector3 summedTorques_rss() const { return m_vSummedTorques_rss; }

    //! Retourne vrai si les collisions sont activ�es
    bool collisionsActive() const { return m_bCollisionsActive; }

    //! Retourne le type d'objet utilis� pour les collisions
    ECollisionType collisionType() const { return m_eCollisionType; }

    //!
    Math::CVector3 velocityVectorAngles() const;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Op�rateur d'assignation
    CPhysicalComponent& operator = (const CPhysicalComponent& target);

    //! Loads this object's parameters
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) Q_DECL_OVERRIDE;

    //! M�thode de mise � jour avec temps delta depuis la derni�re it�ration
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //! Appel de la m�thode update des enfants
    virtual void postUpdate(double dDeltaTimeS) Q_DECL_OVERRIDE;

    //! Dump du contenu dans un flux
    virtual void dump(QTextStream& stream, int iIdent);

    //! Ajoute un champ de hauteurs pour cet objet
    void addField(CHeightField* value);

    //! Arr�te les calculs de physique
    void sleep();

    //! Reprend les calculs de physique
    void wakeUp();

    //! Ajoute une force en kilogrammes dans le rep�re local de l'objet, de mani�re excentr�e
    //! Elle peut donc produire un couple
    void addUncenteredLocalForce_kg(Math::CVector3 vPosition, Math::CVector3 vForce_kg);

    //! Ajoute une force en kilogrammes dans le rep�re local de l'objet
    void addLocalForce_kg(Math::CVector3 vForce_kg);

    //! Ajoute une force en kilogrammes dans le rep�re NOLL de l'objet
    void addForce_kg(Math::CVector3 vForce_kg);

    //! Ajoute un couple en kilogrammes dans le rep�re local de l'objet
    void addLocalTorque_kg(Math::CVector3 vForce_kg);

    //! Ajoute un couple en kilogrammes dans le rep�re NOLL de l'objet
    void addTorque_kg(Math::CVector3 vForce_kg);

    //-------------------------------------------------------------------------------------------------
    // Static methods
    //-------------------------------------------------------------------------------------------------

    //!
    static void computeCollisions(QVector<QSP<CComponent> >& vComponents, double dDeltaTimeS);

    //!
    static void computeCollisionsForComponent(QSP<CPhysicalComponent> pComponent, QVector<QSP<CComponent> >& vOtherComponents, double dDeltaTimeS);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QVector<CHeightField*>  m_pFields;                      // Les champs de hauteurs dont d�pend cet objet

    bool                    m_bPhysicsActive;               // Calculs de physique activ�s
    bool                    m_bCollisionsActive;            // Calculs de collisions activ�s
    bool                    m_bOnGround;                    // Indicateur si v�hicule au sol
    double                  m_dDrag_norm;                   // Facteur de train�e normalis� (0..1)
    double                  m_dAngularDrag_norm;            // Facteur de train�e angulaire normalis� (0..1)
    double                  m_dFriction_norm;               // Facteur de friction normalis� (0..1)
    double                  m_dMass_kg;                     // Masse en kilogrammes
    double                  m_dStickToNOLL;                 //
    double                  m_dRotationLatency;             //
    Math::CVector3          m_vCenterOfMass;                // Centre de masse
    Math::CVector3          m_vVelocity_ms;                 // V�locit� en m�tres/seconde, rep�re NOLL
    Math::CVector3          m_vAngularVelocity_rs;          // V�locit� angulaire en degr�s/seconde, rep�re NOLL
    Math::CVector3          m_vSummedForces_mss;            // Accel�ration en m�tres/seconde/seconde, rep�re NOLL
    Math::CVector3          m_vSummedTorques_rss;           // Accel�ration angulaire en m�tres/seconde/seconde, rep�re NOLL
    ECollisionType          m_eCollisionType;               // Type de collisions � utiliser
};
