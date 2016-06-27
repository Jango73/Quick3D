
#pragma once

#include "quick3d_global.h"

// Application
#include "CQ3DConstants.h"
#include "CStandardController.h"

class C3DScene;

class QUICK3D_EXPORT CForceController : public CStandardController
{

public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Retourne une nouvelle instance de cet objet
    static CComponent* instanciator(C3DScene* pScene);

    //! Constructeur d'après une scène
    CForceController(C3DScene* pScene);

    //! Destructeur
    virtual ~CForceController();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Méthodes héritées
    //-------------------------------------------------------------------------------------------------

    //! Retourne le nom de classe de cet objet
    virtual QString getClassName() const { return ClassName_CForceController; }

    //! Met l'objet à jour d'après le temps écoulé depuis la dernière image
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

};
