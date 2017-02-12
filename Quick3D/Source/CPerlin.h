
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

    //! Retourne une valeur de bruit pour une position donn�e
    double noise(Math::CVector3 pos);

    //! Retourne une valeur de bruit normalis�e pour une position donn�e
    double noise_0_1(Math::CVector3 pos);

    //! Retourne une valeur de turbulence pour une position donn�e
    double turbulence(Math::CVector3 pos);

    //! Retourne une valeur d'�rosion pour une position donn�e
    double erosion(Math::CVector3 pos, Math::CAxis reference, double dDisplace);

    //! Retourne une valeur du pattern de Voronoi pour une position donn�e
    double voronoi(Math::CVector3 pos, Math::CAxis reference, double dDisplace);

    //! Retourne une valeur de d�placement pour une position donn�e
    Math::CVector3 displace(Math::CVector3 pos, double scale);

    //-------------------------------------------------------------------------------------------------
    // Propri�t�s
    //-------------------------------------------------------------------------------------------------

protected:

    Math::CMatrix4  m_mNoiseRotation;
};
