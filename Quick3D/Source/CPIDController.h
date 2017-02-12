
#ifndef CPIDCONTROLLER_H
#define CPIDCONTROLLER_H

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CPIDController
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Default constructor
    CPIDController(double dNewProportional, double dNewIntegral, double dNewDerivative);

    //! Destructor
    virtual ~CPIDController();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! Définit la valeur cible à atteindre
    void setSetPoint(double value) { m_dSetPoint = value; }

    //! Définit le facteur proportionnel
    void setProportionalConstant(double value) { m_dProportionalConstant = value; }

    //! Définit le facteur intégral
    void setIntegralConstant(double value) { m_dIntegralConstant = value; }

    //! Définit le facteur dérivé
    void setDerivativeConstant(double value) { m_dDerivativeConstant = value; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Retourne la valeur cible à atteindre
    double getSetPoint() { return m_dSetPoint; }

    //! Retourne le facteur proportionnel
    double getProportionalConstant() { return m_dProportionalConstant; }

    //! Retourne le facteur intégral
    double getIntegralConstant() { return m_dIntegralConstant; }

    //! Retourne le facteur dérivé
    double getDerivativeConstant() { return m_dDerivativeConstant; }

    //! Retourne la valeur de sortie
    double getOutput() { return m_dOutput; }

    //-------------------------------------------------------------------------------------------------
    // Méthode de contrôle
    //-------------------------------------------------------------------------------------------------

    //! Réinitialise le contrôleur
    void reset();

    //! Met à jour le contrôleur
    void update(double dCurrentValue, double DeltaTimeMillis);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    double m_dSetPoint;
    double m_dError;
    double m_dPreviousError;
    double m_dIntegral;
    double m_dDerivative;
    double m_dProportionalConstant;
    double m_dIntegralConstant;
    double m_dDerivativeConstant;
    double m_dOutput;
};

#endif // CPIDCONTROLLER_H
