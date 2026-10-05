
#pragma once

// qt-plus
#include "CXMLNode.h"

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CNamed.h"
#include "CVector3.h"
#include "CGeoloc.h"
#include "CPerlin.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CGenerateFunction : public CNamed
{
public:

    enum ETerrainOperation
    {
        toNone,
        toConstant,
        toAdd,
        toSub,
        toMul,
        toDiv,
        toPow,
        toPerlin,
        toTurbulence,
        toErosion,
        toVoronoi
    };

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CGenerateFunction(CXMLNode xFunctions, CXMLNode xNode);

    //!
    virtual ~CGenerateFunction();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void getStandardParameters(CXMLNode xFunctions, CXMLNode xParams);

    //!
    void getProceduralParameters(CXMLNode xFunctions, CXMLNode xParams);

    //!
    double process(CPerlin* pPerlin, const Math::CVector3& vPosition, const Math::CAxis& aAxis) const;

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    ETerrainOperation               m_eType;
    double                          m_dConstant;
    QVector<CGenerateFunction*>     m_vOperands;
    Math::CVector3                  m_vOffset;
    double                          m_dInputScale;
    double                          m_dOutputScale;
    double                          m_dMinClamp;
    double                          m_dMaxClamp;
    double                          m_dDisplace;
    int                             m_dIterations;
    double                          m_dInputScaleFactor;
    double                          m_dOutputScaleFactor;
};
