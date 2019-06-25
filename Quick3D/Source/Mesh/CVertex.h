
#pragma once

// Application
#include "quick3d_global.h"
#include "CVector2.h"
#include "CVector3.h"

//-------------------------------------------------------------------------------------------------
// Macros

#define VTX_OFFSET_OF(c,m)	( static_cast<unsigned int> ( reinterpret_cast<uintptr_t>( &(((c*) 0)->m) ) ) )

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CVertex
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Default constructor
    CVertex();

    //! Constructeur avec coordonnées de position et de texture mapping
    CVertex(Math::CVector3 NewPosition, Math::CVector3 NewTexCoord = Math::CVector3());

    //! Constructeur de copie
    CVertex(const CVertex&);

    //! Destructor
    virtual ~CVertex();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! Définit un poids pour une texture
    void setDiffuseTextureWeight(int iTexture, double dWeight);

    //!
    void setNormal(const Math::CVector3& value) { m_vNormal = value; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    Math::CVector3& position() { return m_vPosition; }

    //!
    Math::CVector3& normal() { return m_vNormal; }

    //!
    Math::CVector3& tangent() { return m_vTangent; }

    //!
    Math::CVector3& gravity() { return m_vGravity; }

    //!
    Math::CVector3& texCoord() { return m_vTexCoord; }

    //!
    double& altitude() { return m_dAltitude; }

    //!
    double& normalDivider() { return m_dNormalDivider; }

    //!
    Math::CVector3 position() const { return m_vPosition; }

    //!
    Math::CVector3 normal() const { return m_vNormal; }

    //!
    Math::CVector3 tangent() const { return m_vTangent; }

    //!
    Math::CVector3 texCoord() const { return m_vTexCoord; }

    //!
    double altitude() const { return m_dAltitude; }

    //-------------------------------------------------------------------------------------------------
    // Static methods
    //-------------------------------------------------------------------------------------------------

    //! Retourne l'offset mémoire de la propriété m_vPosition
    static unsigned int positionOffset() { return VTX_OFFSET_OF(CVertex, m_vPosition); }

    //! Retourne l'offset mémoire de la propriété m_vTexCoord
    static unsigned int texCoordOffset() { return VTX_OFFSET_OF(CVertex, m_vTexCoord); }

    //! Retourne l'offset mémoire de la propriété m_vNormal
    static unsigned int normalOffset() { return VTX_OFFSET_OF(CVertex, m_vNormal); }

    //! Retourne l'offset mémoire de la propriété m_vTangent
    static unsigned int tangentOffset() { return VTX_OFFSET_OF(CVertex, m_vTangent); }

    //! Retourne l'offset mémoire de la propriété m_dAltitude
    static unsigned int altitudeOffset() { return VTX_OFFSET_OF(CVertex, m_dAltitude); }

    //! Retourne l'offset mémoire de la propriété m_vDiffTexWeight_0_1_2
    static unsigned int diffTexWeight_0_1_2Offset() { return VTX_OFFSET_OF(CVertex, m_vDiffTexWeight_0_1_2); }

    //! Retourne l'offset mémoire de la propriété m_vDiffTexWeight_3_4_5
    static unsigned int diffTexWeight_3_4_5Offset() { return VTX_OFFSET_OF(CVertex, m_vDiffTexWeight_3_4_5); }

    //! Retourne l'offset mémoire de la propriété m_vDiffTexWeight_6_7_8
    static unsigned int diffTexWeight_6_7_8Offset() { return VTX_OFFSET_OF(CVertex, m_vDiffTexWeight_6_7_8); }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Opérateur de copie
    CVertex& operator=(const CVertex& target);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

public:

    QVector<int>        m_vSmoothingGroups;
    QVector<int>        m_vVertexIndicesForGroup;

protected:

    Math::CVector3      m_vPosition;				// Coordonnées de position
    Math::CVector3      m_vTexCoord;				// Coordonnées de texture mapping
    Math::CVector3      m_vDiffTexWeight_0_1_2;		// Poids des textures 0 à 2
    Math::CVector3      m_vDiffTexWeight_3_4_5;		// Poids des textures 3 à 5
    Math::CVector3      m_vDiffTexWeight_6_7_8;		// Poids des textures 6 à 8
    Math::CVector3      m_vNormal;					// Vecteur normal du vertex
    Math::CVector3      m_vTangent;					// Vecteur tangent du vertex
    Math::CVector3      m_vGravity;					// Vecteur de gravité
    double              m_dAltitude;				// Altitude du vertex
    double              m_dNormalDivider;			// Diviseur du vecteur normal
};
