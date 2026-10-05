
#pragma once

#include "math.h"
#include "CVector3.h"
#include "CRay3.h"

namespace Math
{
	class CPlane3
	{

	public:

		double dDistance;
		CVector3 vNormal;

		//! Default constructor
		inline CPlane3 ()
		{
			dDistance = 0.0;
		}

        //! Constructor with components
		inline CPlane3 (double dNewDistance, CVector3 vNewNormal)
		{
			dDistance = dNewDistance;
			vNormal = vNewNormal;
		}

        //! Copy constructor
		inline CPlane3 (const CPlane3& Target)
		{
			*this = Target;
		}

        //! Equality operator
		inline bool operator == (const CPlane3& V2) const
		{
			return (dDistance == V2.dDistance) && (vNormal == V2.vNormal);
		}

        //! Inequality operator
		inline bool operator != (const CPlane3& V2) const
		{
			return !(*this == V2);
		}

        //! Assign operator
		inline CPlane3& operator = (const CPlane3& Target)
		{
			dDistance = Target.dDistance;
			vNormal = Target.vNormal;

			return *this;
		}

		//! Distance d'intersection avec un rayon, valide si > 0
		inline RayTracingResult intersect(const CRay3& rRay)
		{
			// Find t.
			double t = - (vNormal.dot(rRay.vOrigin) + dDistance) / (vNormal.dot(rRay.vNormal));

			if (t < 0) // the ray does not hit the surface, that is, the surface is "behind" the ray
				return 0;

			// Get a point on the plane.
			CVector3 p = rRay.vOrigin + rRay.vNormal * t;

			// Does the ray intersect the plane inside or outside?
			CVector3 planeToRayStart = rRay.vOrigin - p;
			double dot = planeToRayStart.dot(vNormal);

            if (dot <= 0) return 0;

			return RayTracingResult(t, nullptr, vNormal);
		}

		//! Normalisation
		inline void normalize()
		{
			vNormal = vNormal.normalized();
		}
	};
}
