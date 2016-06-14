
// Application
#include "CMeshGeometry.h"
#include "CFace.h"

//-------------------------------------------------------------------------------------------------

/*!
    \class CFace
    \brief This class defines polygons for a mesh.
    \inmodule Quick3D
    \sa CMesh
*/

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

const double EPSILON = 0.00001;

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CFace with its default parameters.
*/
CFace::CFace()
    : m_pParent(NULL)
    , m_iSmoothingGroup(0)
    , m_iMaterialIndex(0)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CFace that belongs to \a pParent.
*/
CFace::CFace(CMeshGeometry* pParent)
    : m_pParent(pParent)
    , m_iSmoothingGroup(0)
    , m_iMaterialIndex(0)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CFace that belongs to \a pParent and is a clone of \a target.
*/
CFace::CFace(CMeshGeometry* pParent, const CFace& target)
    : m_pParent(pParent)
    , m_iSmoothingGroup(0)
    , m_iMaterialIndex(0)
{
    *this = target;
}

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CFace that belongs to \a pParent and uses 3 vertex indices: \a iV1, \a iV2 and \a iV3.
*/
CFace::CFace(CMeshGeometry* pParent, int iV1, int iV2, int iV3)
    : m_pParent(pParent)
    , m_iSmoothingGroup(0)
    , m_iMaterialIndex(0)
{
    m_vIndices.append(iV1);
    m_vIndices.append(iV2);
    m_vIndices.append(iV3);
}

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CFace that belongs to \a pParent and uses 4 vertex indices: \a iV1, \a iV2, \a iV3 and \a iV4.
*/
CFace::CFace(CMeshGeometry* pParent, int iV1, int iV2, int iV3, int iV4)
    : m_pParent(pParent)
    , m_iSmoothingGroup(0)
    , m_iMaterialIndex(0)
{
    m_vIndices.append(iV1);
    m_vIndices.append(iV2);
    m_vIndices.append(iV3);
    m_vIndices.append(iV4);
}

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CFace that belongs to \a pParent and uses vertex indices in \a vIndices.
*/
CFace::CFace(CMeshGeometry* pParent, QVector<int> vIndices)
    : m_pParent(pParent)
    , m_iSmoothingGroup(0)
    , m_iMaterialIndex(0)
{
    m_vIndices = vIndices;
}

//-------------------------------------------------------------------------------------------------

/*!
    Copies properties of \a target to this face, except the parent.
*/
CFace& CFace::operator = (const CFace& target)
{
    m_vIndices          = target.m_vIndices;
    m_vNormal           = target.m_vNormal;
    m_vTangent          = target.m_vTangent;
    m_iSmoothingGroup   = target.m_iSmoothingGroup;
    m_iMaterialIndex    = target.m_iMaterialIndex;

    return *this;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns \c true if this face's material index is less than \a target 's material index.
*/
bool CFace::operator < (const CFace& target) const
{
    return m_iMaterialIndex < target.m_iMaterialIndex;
}

//-------------------------------------------------------------------------------------------------

/*!
    Compmutes the normal vector of this face.
*/
void CFace::computeNormal()
{
    if (m_pParent != NULL)
    {
        CVertex v1, v2, v3;

        if (m_vIndices.count() > 2)
        {
            if (m_vIndices[0] < m_pParent->getVertices().count()) v1 = m_pParent->getVertices()[m_vIndices[0]];
            if (m_vIndices[1] < m_pParent->getVertices().count()) v2 = m_pParent->getVertices()[m_vIndices[1]];
            if (m_vIndices[2] < m_pParent->getVertices().count()) v3 = m_pParent->getVertices()[m_vIndices[2]];

            CVector3 U = v3.position() - v1.position();
            CVector3 V = v2.position() - v1.position();

            m_vNormal = U.CrossProduct(V).Normalize();
            m_vTangent = U.Normalize();
        }
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns an object containing the result of the intersection between \a ray and a triangle defined by \a v1, \a v2 and \a v3.
*/
RayTracingResult CFace::intersectTriangle(
        const CRay3& ray,
        CVector3 v1,
        CVector3 v2,
        CVector3 v3
        )
{
    bool bSwap = false;

    CVector3 U = v3 - v1;
    CVector3 V = v2 - v1;

    CVector3 vNormal = U.CrossProduct(V).Normalize();

    // Get dominant axis
    int iMajorAxis = 0;
    if (fabs(vNormal.Y) > fabs(vNormal.X)) iMajorAxis = 1;
    if (fabs(vNormal.Z) > fabs(vNormal.X) && fabs(vNormal.Z) > fabs(vNormal.Y)) iMajorAxis = 2;

    switch (iMajorAxis)
    {
        case 0: if ((v2.Y - v3.Y) * (v2.Z - v1.Z) < (v2.Z - v3.Z) * (v2.Y - v1.Y)) bSwap = true; break;
        case 1: if ((v2.X - v3.X) * (v2.Z - v1.Z) < (v2.Z - v3.Z) * (v2.X - v1.X)) bSwap = true; break;
        case 2: if ((v2.X - v3.X) * (v2.Y - v1.Y) < (v2.Y - v3.Y) * (v2.X - v1.X)) bSwap = true; break;
    }

    if (bSwap)
    {
        CVector3 Temp = v2;
        v2 = v1;
        v1 = Temp;
    }

    //
    double dNormalDotDirection = vNormal.DotProduct(ray.vNormal);

    if (fabs(dNormalDotDirection) < EPSILON) return RayTracingResult(Q3D_INFINITY);

    double dNormalDotOrigin = vNormal.DotProduct(ray.vOrigin);

    double dDistance = vNormal.DotProduct(v1) * -1.0;
    double dDepth = -(dDistance + dNormalDotOrigin) / dNormalDotDirection;
    double s, t;

    switch (iMajorAxis)
    {
        case 0:

            s = ray.vOrigin.Y + dDepth * ray.vNormal.Y;
            t = ray.vOrigin.Z + dDepth * ray.vNormal.Z;

            if ((v2.Y - s) * (v2.Z - v1.Z) < (v2.Z - t) * (v2.Y - v1.Y)) return RayTracingResult(Q3D_INFINITY);
            if ((v3.Y - s) * (v3.Z - v2.Z) < (v3.Z - t) * (v3.Y - v2.Y)) return RayTracingResult(Q3D_INFINITY);
            if ((v1.Y - s) * (v1.Z - v3.Z) < (v1.Z - t) * (v1.Y - v3.Y)) return RayTracingResult(Q3D_INFINITY);

            return RayTracingResult(fabs(dDepth), NULL, vNormal);

        case 1:

            s = ray.vOrigin.X + dDepth * ray.vNormal.X;
            t = ray.vOrigin.Z + dDepth * ray.vNormal.Z;

            if ((v2.X - s) * (v2.Z - v1.Z) < (v2.Z - t) * (v2.X - v1.X)) return RayTracingResult(Q3D_INFINITY);
            if ((v3.X - s) * (v3.Z - v2.Z) < (v3.Z - t) * (v3.X - v2.X)) return RayTracingResult(Q3D_INFINITY);
            if ((v1.X - s) * (v1.Z - v3.Z) < (v1.Z - t) * (v1.X - v3.X)) return RayTracingResult(Q3D_INFINITY);

            return RayTracingResult(fabs(dDepth), NULL, vNormal);

        case 2:

            s = ray.vOrigin.X + dDepth * ray.vNormal.X;
            t = ray.vOrigin.Y + dDepth * ray.vNormal.Y;

            if ((v2.X - s) * (v2.Y - v1.Y) < (v2.Y - t) * (v2.X - v1.X)) return RayTracingResult(Q3D_INFINITY);
            if ((v3.X - s) * (v3.Y - v2.Y) < (v3.Y - t) * (v3.X - v2.X)) return RayTracingResult(Q3D_INFINITY);
            if ((v1.X - s) * (v1.Y - v3.Y) < (v1.Y - t) * (v1.X - v3.X)) return RayTracingResult(Q3D_INFINITY);

            return RayTracingResult(fabs(dDepth), NULL, vNormal);
    }

    return RayTracingResult(Q3D_INFINITY);
}

/*
double CFace::intersectTriangle(
    const Ray3& ray,
    Vector3 v0,
    Vector3 v1,
    Vector3 v2,
    const Vector3& vNormal,
    int iMajorAxis
    )
{
    // compute plane's normal
    Vector3 A, B;
    A = v1 - v0;
    B = v2 - v0;

    // no need to normalize
    Vector3 N = A.CrossProduct(B); // N

    // Step 1: finding P

    // check if ray and plane are parallel ?
    double NdotRayDirection = N.DotProduct(ray.vNormal);

    if (NdotRayDirection == 0) return Q3D_INFINITY; // they are parallel so they don't intersect !

    // compute d parameter using equation 2
    double d = N.DotProduct(v0);

    // compute t (equation 3)
    double t = -(N.DotProduct(ray.vOrigin) + d) / NdotRayDirection;

    // check if the triangle is in behind the ray
    if (t < 0) return Q3D_INFINITY; // the triangle is behind

    // compute the intersection point using equation 1
    Vector3 P = ray.vOrigin + (ray.vNormal * t);

    // Step 2: inside-outside test

    Vector3 C; // vector perpendicular to triangle's plane

    // edge 0
    Vector3 edge0 = v1 - v0;
    Vector3 VP0 = P - v0;
    C = edge0.CrossProduct(VP0);
    if (N.DotProduct(C) < 0) return Q3D_INFINITY; // P is on the right side

    // edge 1
    Vector3 edge1 = v2 - v1;
    Vector3 VP1 = P - v1;
    C = edge1.CrossProduct(VP1);
    if (N.DotProduct(C) < 0) return Q3D_INFINITY; // P is on the right side

    // edge 2
    Vector3 edge2 = v0 - v2;
    Vector3 VP2 = P - v2;
    C = edge2.CrossProduct(VP2);
    if (N.DotProduct(C) < 0) return Q3D_INFINITY; // P is on the right side;

    return t;
}
*/

/*
double CFace::intersectTriangle(
    const Ray3& ray,
    Vector3 v2,
    Vector3 v1,
    Vector3 v0,
    const Vector3& vNormal,
    int iMajorAxis
    )
{
    Vector3	u, v, n;              // triangle vectors
    Vector3	w0, w;           // ray vectors
    Vector3	p;
    double	r, a, b;              // params to calc ray-plane intersect

    // get triangle edge vectors and plane normal
    u = v1 - v0;
    v = v2 - v0;
    n = u * v;              // cross product
    if (n.valid() == false) return Q3D_INFINITY;                  // do not deal with this case

    w0 = ray.vOrigin - v0;
    a = -n.DotProduct(w0);
    b = n.DotProduct(ray.vNormal);

    if (fabs(b) < EPSILON) {     // ray is  parallel to triangle plane
        if (a == 0)                 // ray lies in triangle plane
            return Q3D_INFINITY;
        else return Q3D_INFINITY;              // ray disjoint from plane
    }

    // get intersect point of ray with triangle plane
    r = a / b;
    if (r < 0.0) return Q3D_INFINITY;                   // ray goes away from triangle

    // for a segment, also test if (r > 1.0) => no intersect

    p = ray.vOrigin + ray.vNormal * r;            // intersect point of ray and plane

    // is I inside T?
    double uu, uv, vv, wu, wv, D;
    uu = u.DotProduct(u);
    uv = u.DotProduct(v);
    vv = v.DotProduct(v);
    w = p - v0;
    wu = w.DotProduct(u);
    wv = w.DotProduct(v);
    D = uv * uv - uu * vv;

    // get and test parametric coords
    double s, t;
    s = (uv * wv - vv * wu) / D;

    if (s < 0.0 || s > 1.0)         // I is outside T
        return Q3D_INFINITY;
    t = (uv * wu - uu * wv) / D;

    if (t < 0.0 || (s + t) > 1.0)  // I is outside T
        return Q3D_INFINITY;

    return r;                       // I is in T}
}
*/

//-------------------------------------------------------------------------------------------------

RayTracingResult CFace::intersect(CRay3 ray)
{
    return RayTracingResult(Q3D_INFINITY);
}
