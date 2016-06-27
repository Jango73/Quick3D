
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

    //! Constructeur d'apr�s une sc�ne
    CForceController(C3DScene* pScene);

    //! Destructeur
    virtual ~CForceController();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // M�thodes de contr�le
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // M�thodes h�rit�es
    //-------------------------------------------------------------------------------------------------

    //! Retourne le nom de classe de cet objet
    virtual QString getClassName() const { return ClassName_CForceController; }

    //! Met l'objet � jour d'apr�s le temps �coul� depuis la derni�re image
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Propri�t�s
    //-------------------------------------------------------------------------------------------------

protected:

};
