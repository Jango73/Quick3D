
#pragma once

// Application
#include "quick3d_global.h"
#include "CVector3.h"
#include "CRay3.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class CMeshGeometry;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CEdge
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Default constructor
    CEdge()
    {
        m_aIndices[0] = 0;
        m_aIndices[1] = 0;
        m_iNewVertex = 0;
    }

    //! Constructor using two vertex indices
    CEdge(int iV1, int iV2)
    {
        m_aIndices[0] = iV1;
        m_aIndices[1] = iV2;
        m_iNewVertex = 0;
    }

    //! Constructor using two vertex indices and a new vertex index
    CEdge(int iV1, int iV2, int iN)
    {
        m_aIndices[0] = iV1;
        m_aIndices[1] = iV2;
        m_iNewVertex = iN;
    }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Equality operator
    bool operator == (const CEdge& target) const
    {
        return	(m_aIndices[0] == target.m_aIndices[0] && m_aIndices[1] == target.m_aIndices[1]) ||
                (m_aIndices[0] == target.m_aIndices[1] && m_aIndices[1] == target.m_aIndices[0]);
    }

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

    int m_aIndices[2];
    int m_iNewVertex;
};

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CFace
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Default constructor
    CFace();

    //! Constructor using a parent mesh geometry
    CFace(CMeshGeometry* pParent);

    //! Copy constructor
    CFace(CMeshGeometry* pParent, const CFace&);

    //! Constructor using three vertex indices (triangle)
    CFace(CMeshGeometry* pParent, int iV1, int iV2, int iV3);

    //! Constructor using four vertex indices (quad)
    CFace(CMeshGeometry* pParent, int iV1, int iV2, int iV3, int iV4);

    //! Constructor using n vertex indices (polygon)
    CFace(CMeshGeometry* pParent, QVector<int> vIndices);

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setSmoothingGroup(int iGroup) { m_iSmoothingGroup = iGroup; }

    //!
    void setMaterialIndex(int iIndex) { m_iMaterialIndex = iIndex; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns the list of vertex indices
    QVector<int>& indices() { return m_vIndices; }

    //! Returns the list of vertex indices
    const QVector<int>& indices() const { return m_vIndices; }

    //! Returns the face's normal vector
    Math::CVector3& normal() { return m_vNormal; }

    //! Returns the face's normal vector
    Math::CVector3 normal() const { return m_vNormal; }

    //! Returns the face's tangent vector
    Math::CVector3 tangent() const { return m_vTangent; }

    //!
    int smoothingGroup() const { return m_iSmoothingGroup; }

    //!
    int materialIndex() const { return m_iMaterialIndex; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Copy operator
    CFace& operator = (const CFace& target);

    //! Less than operator
    bool operator < (const CFace& target) const;

    //! Computes the face's normal vector
    void computeNormal();

    //! Computes the instersection distance from ray's origin to the triangle described by v0, v1 and v2
    static Math::RayTracingResult intersectTriangle(
            const Math::CRay3& ray,
            Math::CVector3 v0,
            Math::CVector3 v1,
            Math::CVector3 v2
            );

    //! Computes the intersection distance from ray's origin to this face
    //! Actually just returns infinity
    Math::RayTracingResult intersect(Math::CRay3 ray);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    CMeshGeometry*  m_pParent;
    QVector<int>    m_vIndices;
    Math::CVector3  m_vNormal;
    Math::CVector3  m_vTangent;
    int             m_iSmoothingGroup;
    int             m_iMaterialIndex;
};
