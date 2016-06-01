
#ifndef __QUATERNION__
#define __QUATERNION__

#include "math.h"
#include "CVector3.h"

namespace Math
{
	struct CQuaternion
	{
	public:

		double X;
		double Y;
		double Z;
		double W;

		inline CQuaternion()
		{
			X = 0.0f;
			Y = 0.0f;
			Z = 0.0f;
			W = 0.0f;
		}

		inline CQuaternion(double NewX, double NewY, double NewZ, double NewW)
		{
			X = NewX;
			Y = NewY;
			Z = NewZ;
			W = NewW;
		}

		CVector3 operator * (const CVector3& Vec)
		{
			double ux = W * Vec.X + Y * Vec.Z - Z * Vec.Y;
			double uy = W * Vec.Y + Z * Vec.X - X * Vec.Z;
			double uz = W * Vec.Z + X * Vec.Y - Y * Vec.X;
			double uw = -X * Vec.X - Y * Vec.Y - Z * Vec.Z;

			double vx = -uw * X + ux * W - uy * Z + uz * Y;
			double vy = -uw * Y + uy * W - uz * X + ux * Z;
			double vz = -uw * Z + uz * W - ux * Y + uy * X;

			return CVector3(vx, vy, vz);
		}
	};
}

#endif // __QUATERNION__
