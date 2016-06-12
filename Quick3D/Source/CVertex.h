
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CVector2.h"
#include "CVector3.h"

//-------------------------------------------------------------------------------------------------
// Macros

#define VTX_OFFSET_OF(c,m)	((long) ( &(((c*) 0)->m) ) )

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CVertex
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Constructeur par d�faut
    CVertex();

    //! Constructeur avec coordonn�es de position et de texture mapping
    CVertex(Math::CVector3 NewPosition, Math::CVector2 NewTexCoord = Math::CVector2());

    //! Constructeur de copie
    CVertex(const CVertex&);

    //! Destructeur
    virtual ~CVertex();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! D�finit un poids pour une texture
    void setDiffuseTextureWeight(int iTexture, double dWeight);

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
    Math::CVector2& texCoord() { return m_vTexCoord; }

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
    Math::CVector2 texCoord() const { return m_vTexCoord; }

    //!
    double altitude() const { return m_dAltitude; }

    //-------------------------------------------------------------------------------------------------
    // M�thodes statiques
    //-------------------------------------------------------------------------------------------------

    //! Retourne l'offset m�moire de la propri�t� m_vPosition
    static int positionOffset() { return VTX_OFFSET_OF(CVertex, m_vPosition); }

    //! Retourne l'offset m�moire de la propri�t� m_vTexCoord
    static int texCoordOffset() { return VTX_OFFSET_OF(CVertex, m_vTexCoord); }

    //! Retourne l'offset m�moire de la propri�t� m_vNormal
    static int normalOffset() { return VTX_OFFSET_OF(CVertex, m_vNormal); }

    //! Retourne l'offset m�moire de la propri�t� m_vTangent
    static int tangentOffset() { return VTX_OFFSET_OF(CVertex, m_vTangent); }

    //! Retourne l'offset m�moire de la propri�t� m_dAltitude
    static int altitudeOffset() { return VTX_OFFSET_OF(CVertex, m_dAltitude); }

    //! Retourne l'offset m�moire de la propri�t� m_vDiffTexWeight_0_1_2
    static int diffTexWeight_0_1_2Offset() { return VTX_OFFSET_OF(CVertex, m_vDiffTexWeight_0_1_2); }

    //! Retourne l'offset m�moire de la propri�t� m_vDiffTexWeight_3_4_5
    static int diffTexWeight_3_4_5Offset() { return VTX_OFFSET_OF(CVertex, m_vDiffTexWeight_3_4_5); }

    //! Retourne l'offset m�moire de la propri�t� m_vDiffTexWeight_6_7_8
    static int diffTexWeight_6_7_8Offset() { return VTX_OFFSET_OF(CVertex, m_vDiffTexWeight_6_7_8); }

    //-------------------------------------------------------------------------------------------------
    // M�thodes de contr�le
    //-------------------------------------------------------------------------------------------------

    //! Op�rateur de copie
    CVertex& operator=(const CVertex& target);

    //-------------------------------------------------------------------------------------------------
    // Propri�t�s
    //-------------------------------------------------------------------------------------------------

public:

    QVector<int>        m_vSmoothingGroups;
    QVector<int>        m_vVertexIndicesForGroup;

protected:

    Math::CVector3      m_vPosition;				// Coordonn�es de position
    Math::CVector2      m_vTexCoord;				// Coordonn�es de texture mapping
    Math::CVector3      m_vDiffTexWeight_0_1_2;		// Poids des textures 0 � 2
    Math::CVector3      m_vDiffTexWeight_3_4_5;		// Poids des textures 3 � 5
    Math::CVector3      m_vDiffTexWeight_6_7_8;		// Poids des textures 6 � 8
    Math::CVector3      m_vNormal;					// Vecteur normal du vertex
    Math::CVector3      m_vTangent;					// Vecteur tangent du vertex
    Math::CVector3      m_vGravity;					// Vecteur de gravit�
    double              m_dAltitude;				// Altitude du vertex
    double              m_dNormalDivider;			// Diviseur du vecteur normal
};
