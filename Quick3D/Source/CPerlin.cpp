
// Application
#include "CPerlin.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CPerlin::CPerlin()
{
    m_mNoiseRotation.makeRotation(CVector3(0.0, Math::Pi, 0.0));
}

//-------------------------------------------------------------------------------------------------

static inline double min(double v1, double v2)
{
    return (v1 < v2) ? v1 : v2;
}

//-------------------------------------------------------------------------------------------------

static inline double max(double v1, double v2)
{
    return (v1 > v2) ? v1 : v2;
}

//-------------------------------------------------------------------------------------------------

static inline double clamp(double value, double min, double max)
{
    if (value < min) return min;
    if (value > max) return max;

    return value;
}

//-------------------------------------------------------------------------------------------------

static inline double dot(CVector3 a, CVector3 b)
{
    return a.dot(b);
}

//-------------------------------------------------------------------------------------------------

static inline double fract(double a)
{
    return a - floor(a);
}

//-------------------------------------------------------------------------------------------------

static inline CVector3 floor(CVector3 a)
{
    return CVector3(floor(a.X), floor(a.Y), floor(a.Z));
}

//-------------------------------------------------------------------------------------------------

static inline CVector3 fract(CVector3 a)
{
    return CVector3(fract(a.X), fract(a.Y), fract(a.Z));
}

//-------------------------------------------------------------------------------------------------

static inline double distance(CVector3 a, CVector3 b)
{
    return (a - b).magnitude();
}

//-------------------------------------------------------------------------------------------------

static inline double mix(double s, double e, double v)
{
    return s * (1 - v) + e * v;
}

//-------------------------------------------------------------------------------------------------

static inline double hash(CVector3 p)
{
    return fract(sin(dot(p, CVector3(127.1, 311.7, 321.4))) * 43758.5453123);
}

//-------------------------------------------------------------------------------------------------

static inline double hash2(CVector3 uv)
{
    return fract(cos(uv.X + sin(uv.Y)) * 12345.6789);
}

//-------------------------------------------------------------------------------------------------

static inline CVector3 neighbor_offset(double i)
{
    double x = floor(i / 3.0);
    double y = fmod(i, 3.0);
    return CVector3(x, y, 0.0) - 1.0;
}

//-------------------------------------------------------------------------------------------------

double CPerlin::noise(CVector3 pos)
{
    CVector3 i = floor(pos);
    CVector3 f = fract(pos);

    f = f * (f * (CVector3(3.0, 3.0, 3.0) - f * 2.0));

    double n = mix(
                mix(mix(hash(i + CVector3(0.0, 0.0, 0.0)), hash(i + CVector3(1.0, 0.0, 0.0)), f.X),
                    mix(hash(i + CVector3(0.0, 1.0, 0.0)), hash(i + CVector3(1.0, 1.0, 0.0)), f.X),
                    f.Y),
                mix(mix(hash(i + CVector3(0.0, 0.0, 1.0)), hash(i + CVector3(1.0, 0.0, 1.0)), f.X),
                    mix(hash(i + CVector3(0.0, 1.0, 1.0)), hash(i + CVector3(1.0, 1.0, 1.0)), f.X),
                    f.Y),
                f.Z);

    return clamp((n - 0.49) * 2.05, -1.0, 1.0);
}

//-------------------------------------------------------------------------------------------------

double CPerlin::noise_0_1(CVector3 pos)
{
    return (noise(pos) + 1.0) * 0.5;
}

//-------------------------------------------------------------------------------------------------

double CPerlin::turbulence(CVector3 pos)
{
    int octaves = 4;
    double f = 0.0;

    for (int i = 0; i < octaves; i++)
    {
        pos = (CVector3(pos.Y, pos.Z, pos.X) + CVector3(pos.Z, pos.Y, pos.X) * CVector3(1.0, -1.0, 1.0)) / sqrt(2.0);
        f = f * 2.0 + noise(pos);
        pos = pos * 1.5;
    }

    f /= pow(2.0, (double) octaves);
    return f;
}

//-------------------------------------------------------------------------------------------------

double CPerlin::erosion(CVector3 pos, CAxis reference, double dDisplace)
{
    pos = displace(pos, dDisplace);

    // Matrix4 mFromLocalAxis = Matrix4(reference.Front, reference.Right, reference.Up);
    // pos = mFromLocalAxis * pos;

    pos.X = fmod(pos.X, 1.0);
    pos.Y = fmod(pos.Y, 1.0);
    pos.Z = 0.0;

    if (pos.X < 0.0) pos.X -= 0.5;
    if (pos.Y < 0.0) pos.Y -= 0.5;

    pos -= CVector3(0.5, 0.5, 0.5);

    double dResult = fabs(pos.X);
    if (fabs(pos.Y) > dResult) dResult = fabs(pos.Y);

    return 1.0 - (dResult * 2.0);
}

//-------------------------------------------------------------------------------------------------

double CPerlin::voronoi(CVector3 pos, CAxis reference, double dDisplace)
{
    CVector3 g = floor(pos);
    CVector3 f = fract(pos);
    double res = 1.0;
    CVector3 bb;

    for (int i = 0; i < 9; i++)
    {
        CVector3 b = neighbor_offset((double) i);
        double h = distance(b + hash2(g + b), f);
        res = min(res, h);
    }

    res -= 0.5;
    res *= 2.0;

    return res;
}

//-------------------------------------------------------------------------------------------------

CVector3 CPerlin::displace(CVector3 pos, double scale)
{
    double dX = turbulence(CVector3(pos.X + scale, pos.Y, pos.Z));
    double dY = turbulence(CVector3(pos.X, pos.Y + scale, pos.Z));
    double dZ = turbulence(CVector3(pos.X, pos.Y, pos.Z + scale));

    return pos + CVector3(dX, dY, dZ) * (scale * 0.25);
}
