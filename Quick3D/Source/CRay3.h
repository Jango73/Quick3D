
#ifndef __RAY3_H__
#define __RAY3_H__

//-------------------------------------------------------------------------------------------------

#include "math.h"
#include "CVector3.h"

//-------------------------------------------------------------------------------------------------

#ifndef Q3D_INFINITY
#define Q3D_INFINITY	999999.0
#endif

//-------------------------------------------------------------------------------------------------

namespace Math
{

class RayTracingResult
{
public:

    inline RayTracingResult(double dDistance, const void* pObject = NULL, CVector3 vNormal = CVector3())
        : m_dDistance(dDistance)
        , m_pObject(pObject)
        , m_vNormal(vNormal)
    {
    }

    inline RayTracingResult& operator = (const RayTracingResult& aCopy)
    {
        m_dDistance = aCopy.m_dDistance;
        m_pObject = aCopy.m_pObject;
        m_vNormal = aCopy.m_vNormal;

        return *this;
    }

    double			m_dDistance;
    const void*		m_pObject;
    CVector3		m_vNormal;
};

class CRay3
{
public:

    CVector3 vOrigin;
    CVector3 vNormal;

    //! Constructeur par défaut
    inline CRay3 ()
    {
    }

    //! Constructeur par composants
    inline CRay3 (CVector3 dNewOrigin, CVector3 vNewNormal)
    {
        vOrigin = dNewOrigin;
        vNormal = vNewNormal;
    }

    //! Constructeur de copie
    inline CRay3 (const CRay3& Target)
    {
        *this = Target;
    }

    //! Opérateur d'égalité
    inline bool operator == (const CRay3& V2) const
    {
        return (vOrigin == V2.vOrigin) && (vNormal == V2.vNormal);
    }

    //! Opérateur d'inégalité
    inline bool operator != (const CRay3& V2) const
    {
        return !(*this == V2);
    }

    //! Opérateur d'assignation
    inline CRay3& operator = (const CRay3& Target)
    {
        vOrigin = Target.vOrigin;
        vNormal = Target.vNormal;

        return *this;
    }
};

}

#endif // __RAY3_H__
