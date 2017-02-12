
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
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Retourne une nouvelle instance de cet objet
    static CComponent* instanciator(C3DScene* pScene);

    //! Constructor using a scene
    CBasicAnimator(C3DScene* pScene);

    //! Destructor
    virtual ~CBasicAnimator();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Retourne une r�f�rence sur les pas de translations
    CAnimatorFrame* translationFrame() { return m_pTranslationFrame; }

    //! Retourne une r�f�rence sur les pas de rotations
    CAnimatorFrame* rotationFrame() { return m_pRotationFrame; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Ajoute un pas de translation
    void addTranslationStep(CBasicAnimationStep value);

    //! Ajoute un pas de rotation
    void addRotationStep(CBasicAnimationStep value);

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! Returns this object's class name
    virtual QString getClassName() const { return ClassName_CBasicAnimator; }

    //! Loads this object's parameters d'apr�s le noeud XML fourni
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) Q_DECL_OVERRIDE;

    //! Met l'objet � jour d'apr�s le temps �coul� depuis la derni�re image
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    CAnimatorFrame*     m_pTranslationFrame;
    CAnimatorFrame*     m_pRotationFrame;
};
