
#pragma once

#include "quick3d_global.h"

// Qt
#include <QVector>

// Application
#include "CQ3DConstants.h"
#include "CController.h"

//-------------------------------------------------------------------------------------------------
// D�clarations avanc�es

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CAnimator : public CController
{

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Retourne l'instanciateur de cet objet
    static CComponent* instanciator(C3DScene* pScene);

    //! Constructeur d'apr�s une sc�ne
    CAnimator(C3DScene* pScene);

    //! Destructor
    virtual ~CAnimator();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    bool isPlaying() const { return m_bIsPlaying; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual void play();

    //!
    virtual void stop();

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! Returns this object's class name
    virtual QString getClassName() const { return ClassName_CAnimator; }

    //! Charge les param�tres de cet objet d'apr�s le noeud XML fourni
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) Q_DECL_OVERRIDE;

    //!
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    bool	m_bIsPlaying;
};
