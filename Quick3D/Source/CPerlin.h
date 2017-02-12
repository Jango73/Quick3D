
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CVector3.h"
#include "CMatrix4.h"
#include "CAxis.h"
#include "CSingleton.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CPerlin : public CSingleton<CPerlin>
{
    friend class CSingleton<CPerlin>;

public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //!
    CPerlin();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Retourne une valeur de bruit pour une position donnée
    double noise(Math::CVector3 pos);

    //! Retourne une valeur de bruit normalisée pour une position donnée
    double noise_0_1(Math::CVector3 pos);

    //! Retourne une valeur de turbulence pour une position donnée
    double turbulence(Math::CVector3 pos);

    //! Retourne une valeur d'érosion pour une position donnée
    double erosion(Math::CVector3 pos, Math::CAxis reference, double dDisplace);

    //! Retourne une valeur du pattern de Voronoi pour une position donnée
    double voronoi(Math::CVector3 pos, Math::CAxis reference, double dDisplace);

    //! Retourne une valeur de déplacement pour une position donnée
    Math::CVector3 displace(Math::CVector3 pos, double scale);

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    Math::CMatrix4  m_mNoiseRotation;
};
