
#pragma once

#include "quick3d_global.h"

// Qt
#include <QVector>

// Application
#include "CQ3DConstants.h"
#include "CController.h"

//-------------------------------------------------------------------------------------------------
// Déclarations avancées

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CAnimator : public CController
{

public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Retourne l'instanciateur de cet objet
    static CComponent* instanciator(C3DScene* pScene);

    //! Constructeur d'après une scène
    CAnimator(C3DScene* pScene);

    //! Destructeur
    virtual ~CAnimator();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    bool isPlaying() const { return m_bIsPlaying; }

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    //!
    virtual void play();

    //!
    virtual void stop();

    //-------------------------------------------------------------------------------------------------
    // Méthodes héritées
    //-------------------------------------------------------------------------------------------------

    //! Retourne le nom de classe de cet objet
    virtual QString getClassName() const { return ClassName_CAnimator; }

    //! Charge les paramètres de cet objet d'après le noeud XML fourni
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) Q_DECL_OVERRIDE;

    //!
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    bool	m_bIsPlaying;
};
