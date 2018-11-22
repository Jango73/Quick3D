
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

//! Defines a basic animator
class QUICK3D_EXPORT CBasicAnimator : public CAnimator
{

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Retourne une nouvelle instance de cet objet
    static CComponent* instantiator(C3DScene* pScene);

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

    //! Returns a pointer to the translation frame
    CAnimatorFrame* translationFrame() { return m_pTranslationFrame; }

    //! Returns a pointer to the rotation frame
    CAnimatorFrame* rotationFrame() { return m_pRotationFrame; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Adds a translation step
    void addTranslationStep(CBasicAnimationStep value);

    //! Adds a rotation step
    void addRotationStep(CBasicAnimationStep value);

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! Returns this object's class name
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CBasicAnimator; }

    //! Loads this object's parameters d'après le noeud XML fourni
    virtual void loadParameters(const QString& sBaseFile, const CXMLNode& xComponent) Q_DECL_OVERRIDE;

    //! Updates this object using the elapsed time since last update
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    CAnimatorFrame*     m_pTranslationFrame;
    CAnimatorFrame*     m_pRotationFrame;
};
