
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

    //! Constructor with position coordinates and texture mapping
    CVertex(Math::CVector3 NewPosition, Math::CVector3 NewTexCoord = Math::CVector3());

    //! Copy constructor
    CVertex(const CVertex&);

    //! Destructor
    virtual ~CVertex();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! Sets a weight for a texture
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

    //! Returns the memory offset of property m_vPosition
    static unsigned int positionOffset() { return VTX_OFFSET_OF(CVertex, m_vPosition); }

    //! Returns the memory offset of property m_vTexCoord
    static unsigned int texCoordOffset() { return VTX_OFFSET_OF(CVertex, m_vTexCoord); }

    //! Returns the memory offset of property m_vNormal
    static unsigned int normalOffset() { return VTX_OFFSET_OF(CVertex, m_vNormal); }

    //! Returns the memory offset of property m_vTangent
    static unsigned int tangentOffset() { return VTX_OFFSET_OF(CVertex, m_vTangent); }

    //! Returns the memory offset of property m_dAltitude
    static unsigned int altitudeOffset() { return VTX_OFFSET_OF(CVertex, m_dAltitude); }

    //! Returns the memory offset of property m_vDiffTexWeight_0_1_2
    static unsigned int diffTexWeight_0_1_2Offset() { return VTX_OFFSET_OF(CVertex, m_vDiffTexWeight_0_1_2); }

    //! Returns the memory offset of property m_vDiffTexWeight_3_4_5
    static unsigned int diffTexWeight_3_4_5Offset() { return VTX_OFFSET_OF(CVertex, m_vDiffTexWeight_3_4_5); }

    //! Returns the memory offset of property m_vDiffTexWeight_6_7_8
    static unsigned int diffTexWeight_6_7_8Offset() { return VTX_OFFSET_OF(CVertex, m_vDiffTexWeight_6_7_8); }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Copy operator
    CVertex& operator=(const CVertex& target);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

public:

    QVector<int>        m_vSmoothingGroups;
    QVector<int>        m_vVertexIndicesForGroup;

protected:

    Math::CVector3      m_vPosition;				// Position coordinates
    Math::CVector3      m_vTexCoord;				// Texture mapping coordinates
    Math::CVector3      m_vDiffTexWeight_0_1_2;		// Weights of textures 0 to 2
    Math::CVector3      m_vDiffTexWeight_3_4_5;		// Weights of textures 3 to 5
    Math::CVector3      m_vDiffTexWeight_6_7_8;		// Weights of textures 6 to 8
    Math::CVector3      m_vNormal;					// Vecteur normal du vertex
    Math::CVector3      m_vTangent;					// Tangent vector of the vertex
    Math::CVector3      m_vGravity;					// Gravity vector
    double              m_dAltitude;				// Altitude of the vertex
    double              m_dNormalDivider;			// Divider of the normal vector
};
