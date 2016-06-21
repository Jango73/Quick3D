
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Application
#include "CQ3DConstants.h"
#include "CComponent.h"
#include "CComponentReference.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CHydraulicComponent : public CComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instanciator(C3DScene* pScene);

    //!
    CHydraulicComponent(C3DScene* pScene);

    //!
    virtual ~CHydraulicComponent();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    double getPressure() const { return m_dPressure; }

    //-------------------------------------------------------------------------------------------------
    // Méthodes héritées
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CHydraulicComponent; }

    //! Charge les paramètres de cet objet
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent);

    //! Recherche les liens de cet objet
    virtual void solveLinks(C3DScene* pScene);

    //! Efface les liens de cet objet
    virtual void clearLinks(C3DScene* pScene);

    //!
    virtual void update(double dDeltaTime);

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    //!
    virtual void push(double dPressure, double dDeltaTime);

    //!
    virtual double pull(double dPressure, double dDeltaTime);

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    double                                              m_dPressure;
    QVector<CComponentReference<CHydraulicComponent> >  m_vInputs;
    QVector<CComponentReference<CHydraulicComponent> >  m_vOutputs;
};
