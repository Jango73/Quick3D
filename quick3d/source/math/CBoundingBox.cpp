
#include "CBoundingBox.h"
#include "C3DScene.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

const double EPSILON = 0.00001;

/* Minimal intersection depth. */

const double DEPTH_TOLERANCE = 1.0e-6;

/* Two values are equal if their difference is small than CLOSE_TOLERANCE. */

const double CLOSE_TOLERANCE = 1.0e-6;

/* Side hit. */

const int SIDE_X_0 = 1;
const int SIDE_X_1 = 2;
const int SIDE_Y_0 = 3;
const int SIDE_Y_1 = 4;
const int SIDE_Z_0 = 5;
const int SIDE_Z_1 = 6;

//-------------------------------------------------------------------------------------------------

CBoundingBox::CBoundingBox()
{
}

//-------------------------------------------------------------------------------------------------

CBoundingBox::CBoundingBox(const CBoundingBox& target)
{
    *this = target;
}

//-------------------------------------------------------------------------------------------------

CBoundingBox::CBoundingBox(CVector3 vMinimum, CVector3 vMaximum)
{
    m_vMinimum = vMinimum;
    m_vMaximum = vMaximum;
}

//-------------------------------------------------------------------------------------------------

CBoundingBox::~CBoundingBox()
{
}

//-------------------------------------------------------------------------------------------------

CBoundingBox& CBoundingBox::operator = (const CBoundingBox& target)
{
    m_vMinimum = target.m_vMinimum;
    m_vMaximum = target.m_vMaximum;

    return *this;
}

//-------------------------------------------------------------------------------------------------

void CBoundingBox::prepare()
{
    m_vMinimum = CVector3( 99999.0,  99999.0,  99999.0);
    m_vMaximum = CVector3(-99999.0, -99999.0, -99999.0);
}

//-------------------------------------------------------------------------------------------------

void CBoundingBox::expand(CVector3 vAmount)
{
    m_vMinimum -= vAmount;
    m_vMaximum += vAmount;
}

//-------------------------------------------------------------------------------------------------

bool CBoundingBox::contains(CVector3 vPosition) const
{
    if (vPosition.X < m_vMinimum.X) return false;
    if (vPosition.Y < m_vMinimum.Y) return false;
    if (vPosition.Z < m_vMinimum.Z) return false;

    if (vPosition.X > m_vMaximum.X) return false;
    if (vPosition.Y > m_vMaximum.Y) return false;
    if (vPosition.Z > m_vMaximum.Z) return false;

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CBoundingBox::contains(CGeoloc gPosition) const
{
    CVector3 vPosition = gPosition.toVector3();

    if (vPosition.X < m_vMinimum.X) return false;
    if (vPosition.Y < m_vMinimum.Y) return false;
    if (vPosition.Z < m_vMinimum.Z) return false;

    if (vPosition.X > m_vMaximum.X) return false;
    if (vPosition.Y > m_vMaximum.Y) return false;
    if (vPosition.Z > m_vMaximum.Z) return false;

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CBoundingBox::containsSpherical(CGeoloc gPosition) const
{
    return (gPosition.toVector3() - center()).magnitude() < radius();
}

//-------------------------------------------------------------------------------------------------

bool CBoundingBox::contains(CVector3 vPosition, double dRadius) const
{
    if (vPosition.X + dRadius < m_vMinimum.X) return false;
    if (vPosition.Y + dRadius < m_vMinimum.Y) return false;
    if (vPosition.Z + dRadius < m_vMinimum.Z) return false;

    if (vPosition.X - dRadius > m_vMaximum.X) return false;
    if (vPosition.Y - dRadius > m_vMaximum.Y) return false;
    if (vPosition.Z - dRadius > m_vMaximum.Z) return false;

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CBoundingBox::contains(CGeoloc gPosition, double dRadius) const
{
    CVector3 vPosition = gPosition.toVector3();

    if (vPosition.X + dRadius < m_vMinimum.X) return false;
    if (vPosition.Y + dRadius < m_vMinimum.Y) return false;
    if (vPosition.Z + dRadius < m_vMinimum.Z) return false;

    if (vPosition.X - dRadius > m_vMaximum.X) return false;
    if (vPosition.Y - dRadius > m_vMaximum.Y) return false;
    if (vPosition.Z - dRadius > m_vMaximum.Z) return false;

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CBoundingBox::intersects(const CBoundingBox& target) const
{
    CVector3 vPosA = center();
    CVector3 vSizA = halfSizes();
    CVector3 vPosB = target.center();
    CVector3 vSizB = target.halfSizes();

    if (fabs(vPosA.X - vPosB.X) < vSizA.X + vSizB.X)
    {
        if (fabs(vPosA.Y - vPosB.Y) < vSizA.Y + vSizB.Y)
        {
            // if (fabs(vPosA.Z - vPosB.Z) < vSizA.Z + vSizB.Z)
            {
                return true;
            }
        }
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

void CBoundingBox::scale(double dFactor)
{
    CVector3 vCenter = center();

    m_vMinimum = vCenter + ((m_vMinimum - vCenter) * dFactor);
    m_vMaximum = vCenter + ((m_vMaximum - vCenter) * dFactor);
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CBoundingBox::translated(CVector3 vAmount) const
{
    return CBoundingBox(vAmount + m_vMinimum, vAmount + m_vMaximum);
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CBoundingBox::rotated(CVector3 vAmount) const
{
    CMatrix4 mRotation = CMatrix4::makeRotation(vAmount);

    return transformed(mRotation);
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CBoundingBox::transformed(const CMatrix4& mTransform) const
{
    CBoundingBox bReturn;
    QVector<CVector3> vPoints;

    vPoints.append(CVector3(m_vMinimum.X, m_vMinimum.Y, m_vMinimum.Z));
    vPoints.append(CVector3(m_vMaximum.X, m_vMinimum.Y, m_vMinimum.Z));
    vPoints.append(CVector3(m_vMinimum.X, m_vMinimum.Y, m_vMaximum.Z));
    vPoints.append(CVector3(m_vMaximum.X, m_vMinimum.Y, m_vMaximum.Z));
    vPoints.append(CVector3(m_vMinimum.X, m_vMaximum.Y, m_vMinimum.Z));
    vPoints.append(CVector3(m_vMaximum.X, m_vMaximum.Y, m_vMinimum.Z));
    vPoints.append(CVector3(m_vMinimum.X, m_vMaximum.Y, m_vMaximum.Z));
    vPoints.append(CVector3(m_vMaximum.X, m_vMaximum.Y, m_vMaximum.Z));

    for (int iIndex = 0; iIndex < vPoints.count(); iIndex++)
    {
        vPoints[iIndex] = mTransform * vPoints[iIndex];
    }

    for (int iIndex = 0; iIndex < vPoints.count(); iIndex++)
    {
        if (vPoints[iIndex].X < bReturn.minimum().X) bReturn.minimum().X = vPoints[iIndex].X;
        if (vPoints[iIndex].Y < bReturn.minimum().Y) bReturn.minimum().Y = vPoints[iIndex].Y;
        if (vPoints[iIndex].Z < bReturn.minimum().Z) bReturn.minimum().Z = vPoints[iIndex].Z;
        if (vPoints[iIndex].X > bReturn.maximum().X) bReturn.maximum().X = vPoints[iIndex].X;
        if (vPoints[iIndex].Y > bReturn.maximum().Y) bReturn.maximum().Y = vPoints[iIndex].Y;
        if (vPoints[iIndex].Z > bReturn.maximum().Z) bReturn.maximum().Z = vPoints[iIndex].Z;
    }

    return bReturn;
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CBoundingBox::operator & (const CBoundingBox& target) const
{
    CBoundingBox bReturn(*this);

    if (target.m_vMinimum.X < bReturn.m_vMinimum.X) bReturn.m_vMinimum.X = target.m_vMinimum.X;
    if (target.m_vMinimum.Y < bReturn.m_vMinimum.Y) bReturn.m_vMinimum.Y = target.m_vMinimum.Y;
    if (target.m_vMinimum.Z < bReturn.m_vMinimum.Z) bReturn.m_vMinimum.Z = target.m_vMinimum.Z;

    if (target.m_vMaximum.X > bReturn.m_vMaximum.X) bReturn.m_vMaximum.X = target.m_vMaximum.X;
    if (target.m_vMaximum.Y > bReturn.m_vMaximum.Y) bReturn.m_vMaximum.Y = target.m_vMaximum.Y;
    if (target.m_vMaximum.Z > bReturn.m_vMaximum.Z) bReturn.m_vMaximum.Z = target.m_vMaximum.Z;

    return bReturn;
}

//-------------------------------------------------------------------------------------------------

double CBoundingBox::closestDistance(CVector3 pPoint)
{
    double dReturnValue = 0.0;

    if (m_vMinimum.X > pPoint.X)
    {
        double diff = m_vMinimum.X - pPoint.X;
        dReturnValue += diff * diff;
    }
    else if (pPoint.X > m_vMaximum.X)
    {
        double diff = pPoint.X - m_vMaximum.X;
        dReturnValue += diff * diff;
    }

    if (m_vMinimum.Y > pPoint.Y)
    {
        double diff = m_vMinimum.Y - pPoint.Y;
        dReturnValue += diff * diff;
    }
    else if (pPoint.Y > m_vMaximum.Y)
    {
        double diff = pPoint.Y - m_vMaximum.Y;
        dReturnValue += diff * diff;
    }

    if (m_vMinimum.Z > pPoint.Z)
    {
        double diff = m_vMinimum.Z - pPoint.Z;
        dReturnValue += diff * diff;
    }
    else if (pPoint.Z > m_vMaximum.Z)
    {
        double diff = pPoint.Z - m_vMaximum.Z;
        dReturnValue += diff * diff;
    }

    return sqrt(dReturnValue);
}

//-------------------------------------------------------------------------------------------------

void CBoundingBox::addSegments(C3DScene* pScene)
{
    pScene->addSegment(CVector3(m_vMinimum.X, m_vMinimum.Y, m_vMinimum.Z), CVector3(m_vMinimum.X, m_vMaximum.Y, m_vMinimum.Z));
    pScene->addSegment(CVector3(m_vMaximum.X, m_vMinimum.Y, m_vMinimum.Z), CVector3(m_vMaximum.X, m_vMaximum.Y, m_vMinimum.Z));
    pScene->addSegment(CVector3(m_vMinimum.X, m_vMinimum.Y, m_vMaximum.Z), CVector3(m_vMinimum.X, m_vMaximum.Y, m_vMaximum.Z));
    pScene->addSegment(CVector3(m_vMaximum.X, m_vMinimum.Y, m_vMaximum.Z), CVector3(m_vMaximum.X, m_vMaximum.Y, m_vMaximum.Z));

    pScene->addSegment(CVector3(m_vMinimum.X, m_vMinimum.Y, m_vMinimum.Z), CVector3(m_vMinimum.X, m_vMinimum.Y, m_vMaximum.Z));
    pScene->addSegment(CVector3(m_vMaximum.X, m_vMinimum.Y, m_vMinimum.Z), CVector3(m_vMaximum.X, m_vMinimum.Y, m_vMaximum.Z));
    pScene->addSegment(CVector3(m_vMinimum.X, m_vMaximum.Y, m_vMinimum.Z), CVector3(m_vMinimum.X, m_vMaximum.Y, m_vMaximum.Z));
    pScene->addSegment(CVector3(m_vMaximum.X, m_vMaximum.Y, m_vMinimum.Z), CVector3(m_vMaximum.X, m_vMaximum.Y, m_vMaximum.Z));

    pScene->addSegment(CVector3(m_vMinimum.X, m_vMinimum.Y, m_vMinimum.Z), CVector3(m_vMaximum.X, m_vMinimum.Y, m_vMinimum.Z));
    pScene->addSegment(CVector3(m_vMinimum.X, m_vMinimum.Y, m_vMaximum.Z), CVector3(m_vMaximum.X, m_vMinimum.Y, m_vMaximum.Z));
    pScene->addSegment(CVector3(m_vMinimum.X, m_vMaximum.Y, m_vMinimum.Z), CVector3(m_vMaximum.X, m_vMaximum.Y, m_vMinimum.Z));
    pScene->addSegment(CVector3(m_vMinimum.X, m_vMaximum.Y, m_vMaximum.Z), CVector3(m_vMaximum.X, m_vMaximum.Y, m_vMaximum.Z));
}

//-------------------------------------------------------------------------------------------------

RayTracingResult CBoundingBox::intersect(Math::CRay3 ray)
{
    int smin = 0, smax = 0;    // Side hit for min/max intersection
    double t, tmin, tmax;
    CVector3 P, D;

    P = ray.vOrigin;
    D = ray.vNormal;

    tmin = 0.0;
    tmax = Q3D_INFINITY;

    // Sides first.

    if (D.X < -EPSILON)
    {
        t = (m_vMinimum.X - P.X) / D.X;

        if (t < tmin) return RayTracingResult(Q3D_INFINITY);

        if (t <= tmax)
        {
            smax = SIDE_X_0;
            tmax = t;
        }

        t = (m_vMaximum.X - P.X) / D.X;

        if (t >= tmin)
        {
            if (t > tmax) return RayTracingResult(Q3D_INFINITY);

            smin = SIDE_X_1;
            tmin = t;
        }
    }
    else
    {
        if (D.X > EPSILON)
        {
            t = (m_vMaximum.X - P.X) / D.X;

            if (t < tmin) return RayTracingResult(Q3D_INFINITY);

            if (t <= tmax)
            {
                smax = SIDE_X_1;
                tmax = t;
            }

            t = (m_vMinimum.X - P.X) / D.X;

            if (t >= tmin)
            {
                if (t > tmax) return RayTracingResult(Q3D_INFINITY);

                smin = SIDE_X_0;
                tmin = t;
            }
        }
        else
        {
            if ((P.X < m_vMinimum.X) || (P.X > m_vMaximum.X))
            {
                return RayTracingResult(Q3D_INFINITY);
            }
        }
    }

    // Check Top/Bottom.

    if (D.Y < -EPSILON)
    {
        t = (m_vMinimum.Y - P.Y) / D.Y;

        if (t < tmin) return RayTracingResult(Q3D_INFINITY);

        if (t <= tmax - CLOSE_TOLERANCE)
        {
            smax = SIDE_Y_0;
            tmax = t;
        }
        else
        {
            if (t <= tmax + CLOSE_TOLERANCE)
            {
                if (-D.Y > fabs(D.X)) smax = SIDE_Y_0;
            }
        }

        t = (m_vMaximum.Y - P.Y) / D.Y;

        if (t >= tmin + CLOSE_TOLERANCE)
        {
            if (t > tmax) return RayTracingResult(Q3D_INFINITY);

            smin = SIDE_Y_1;
            tmin = t;
        }
        else
        {
            if (t >= tmin - CLOSE_TOLERANCE)
            {
                if (-D.Y > fabs(D.X)) smin = SIDE_Y_1;
            }
        }
    }
    else
    {
        if (D.Y > EPSILON)
        {
            t = (m_vMaximum.Y - P.Y) / D.Y;

            if (t < tmin) return RayTracingResult(Q3D_INFINITY);

            if (t <= tmax - CLOSE_TOLERANCE)
            {
                smax = SIDE_Y_1;
                tmax = t;
            }
            else
            {
                if (t <= tmax + CLOSE_TOLERANCE)
                {
                    if (D.Y > fabs(D.X)) smax = SIDE_Y_1;
                }
            }

            t = (m_vMinimum.Y - P.Y) / D.Y;

            if (t >= tmin + CLOSE_TOLERANCE)
            {
                if (t > tmax) return RayTracingResult(Q3D_INFINITY);

                smin = SIDE_Y_0;
                tmin = t;
            }
            else
            {
                if (t >= tmin - CLOSE_TOLERANCE)
                {
                    if (D.Y > fabs(D.X)) smin = SIDE_Y_0;
                }
            }
        }
        else
        {
            if ((P.Y < m_vMinimum.Y) || (P.Y > m_vMaximum.Y))
            {
                return RayTracingResult(Q3D_INFINITY);
            }
        }
    }

    // Now front/back

    if (D.Z < -EPSILON)
    {
        t = (m_vMinimum.Z - P.Z) / D.Z;

        if (t < tmin) return RayTracingResult(Q3D_INFINITY);

        if (t <= tmax - CLOSE_TOLERANCE)
        {
            smax = SIDE_Z_0;
            tmax = t;
        }
        else
        {
            if (t <= tmax + CLOSE_TOLERANCE)
            {
                switch (smax)
                {
                    case SIDE_X_0 :
                    case SIDE_X_1 : if (-D.Z > fabs(D.X)) smax = SIDE_Z_0; break;

                    case SIDE_Y_0 :
                    case SIDE_Y_1 : if (-D.Z > fabs(D.Y)) smax = SIDE_Z_0; break;
                }
            }
        }

        t = (m_vMaximum.Z - P.Z) / D.Z;

        if (t >= tmin + CLOSE_TOLERANCE)
        {
            if (t > tmax) return RayTracingResult(Q3D_INFINITY);

            smin = SIDE_Z_1;
            tmin = t;
        }
        else
        {
            if (t >= tmin - CLOSE_TOLERANCE)
            {
                switch (smin)
                {
                    case SIDE_X_0 :
                    case SIDE_X_1 : if (-D.Z > fabs(D.X)) smin = SIDE_Z_1; break;

                    case SIDE_Y_0 :
                    case SIDE_Y_1 : if (-D.Z > fabs(D.Y)) smin = SIDE_Z_1; break;
                }
            }
        }
    }
    else
    {
        if (D.Z > EPSILON)
        {
            t = (m_vMaximum.Z - P.Z) / D.Z;

            if (t < tmin) return RayTracingResult(Q3D_INFINITY);

            if (t <= tmax - CLOSE_TOLERANCE)
            {
                smax = SIDE_Z_1;
                tmax = t;
            }
            else
            {
                if (t <= tmax + CLOSE_TOLERANCE)
                {
                    switch (smax)
                    {
                        case SIDE_X_0 :
                        case SIDE_X_1 : if (D.Z > fabs(D.X)) smax = SIDE_Z_1; break;

                        case SIDE_Y_0 :
                        case SIDE_Y_1 : if (D.Z > fabs(D.Y)) smax = SIDE_Z_1; break;
                    }
                }
            }

            t = (m_vMinimum.Z - P.Z) / D.Z;

            if (t >= tmin + CLOSE_TOLERANCE)
            {
                if (t > tmax) return RayTracingResult(Q3D_INFINITY);

                smin = SIDE_Z_0;
                tmin = t;
            }
            else
            {
                if (t >= tmin - CLOSE_TOLERANCE)
                {
                    switch (smin)
                    {
                        case SIDE_X_0 :
                        case SIDE_X_1 : if (D.Z > fabs(D.X)) smin = SIDE_Z_0; break;

                        case SIDE_Y_0 :
                        case SIDE_Y_1 : if (D.Z > fabs(D.Y)) smin = SIDE_Z_0; break;
                    }
                }
            }
        }
        else
        {
            if ((P.Z < m_vMinimum.Z) || (P.Z > m_vMaximum.Z))
            {
                return RayTracingResult(Q3D_INFINITY);
            }
        }
    }

    if (tmax < DEPTH_TOLERANCE)
    {
        return RayTracingResult(Q3D_INFINITY);
    }

    return RayTracingResult(tmin);
}
