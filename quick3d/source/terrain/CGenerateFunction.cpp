
// Application
#include "CGenerateFunction.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CGenerateFunction::CGenerateFunction(CXMLNode xFunctions, CXMLNode xNode)
    : m_eType(toNone)
    , m_dConstant(0.0)
    , m_dInputScale(1.0)
    , m_dOutputScale(1.0)
    , m_dMinClamp(-1.0)
    , m_dMaxClamp(1.0)
    , m_dDisplace(0.0)
    , m_dIterations(1)
    , m_dInputScaleFactor(10.0)
    , m_dOutputScaleFactor(0.25)
{
    const QString& sType = xNode.attributes()[ParamName_Type];

    if (sType.toLower() == "function")
    {
        QString sFunctionName = xNode.attributes()[ParamName_Name];
        QVector<CXMLNode> vFunctions = xFunctions.getNodesByTagName(ParamName_Function);

        foreach (CXMLNode xFunction, vFunctions)
        {
            if (xFunction.attributes()[ParamName_Name] == sFunctionName)
            {
                getStandardParameters(xFunctions, xFunction.getNodeByTagName(ParamName_Value));
            }
        }
    }
    else
    {
        getStandardParameters(xFunctions, xNode);
    }
}

//-------------------------------------------------------------------------------------------------

CGenerateFunction::~CGenerateFunction()
{
    foreach (CGenerateFunction* pFunction, m_vOperands)
    {
        delete pFunction;
    }
}

//-------------------------------------------------------------------------------------------------

void CGenerateFunction::getStandardParameters(CXMLNode xFunctions, CXMLNode xNode)
{
    const QString& sType = xNode.attributes()[ParamName_Type];

    if (sType.toLower() == "constant")
    {
        m_eType = toConstant;
        m_dConstant = xNode.attributes()[ParamName_Value].toDouble();
    }
    else if (sType.toLower() == "add")
    {
        m_eType = toAdd;
        QVector<CXMLNode> xOperands = xNode.getNodesByTagName(ParamName_Operand);
        foreach (CXMLNode xOperand, xOperands)
        {
            m_vOperands.append(new CGenerateFunction(xFunctions, xOperand.getNodeByTagName(ParamName_Value)));
        }
    }
    else if (sType.toLower() == "sub")
    {
        m_eType = toSub;
        QVector<CXMLNode> xOperands = xNode.getNodesByTagName(ParamName_Operand);
        foreach (CXMLNode xOperand, xOperands)
        {
            m_vOperands.append(new CGenerateFunction(xFunctions, xOperand.getNodeByTagName(ParamName_Value)));
        }
    }
    else if (sType.toLower() == "mul")
    {
        m_eType = toMul;
        QVector<CXMLNode> xOperands = xNode.getNodesByTagName(ParamName_Operand);
        foreach (CXMLNode xOperand, xOperands)
        {
            m_vOperands.append(new CGenerateFunction(xFunctions, xOperand.getNodeByTagName(ParamName_Value)));
        }
    }
    else if (sType.toLower() == "div")
    {
        m_eType = toDiv;
        QVector<CXMLNode> xOperands = xNode.getNodesByTagName(ParamName_Operand);
        foreach (CXMLNode xOperand, xOperands)
        {
            m_vOperands.append(new CGenerateFunction(xFunctions, xOperand.getNodeByTagName(ParamName_Value)));
        }
    }
    else if (sType.toLower() == "pow")
    {
        m_eType = toPow;
        m_dConstant = xNode.attributes()[ParamName_Value].toDouble();
        m_vOperands.append(new CGenerateFunction(xFunctions, xNode.getNodeByTagName(ParamName_Operand).getNodeByTagName(ParamName_Value)));
    }
    else if (sType.toLower() == "perlin")
    {
        m_eType = toPerlin;
        getProceduralParameters(xFunctions, xNode);
    }
    else if (sType.toLower() == "turbulence")
    {
        m_eType = toTurbulence;
        getProceduralParameters(xFunctions, xNode);
    }
    else if (sType.toLower() == "erosion")
    {
        m_eType = toErosion;
        getProceduralParameters(xFunctions, xNode);
    }
    else if (sType.toLower() == "voronoi")
    {
        m_eType = toVoronoi;
        getProceduralParameters(xFunctions, xNode);
    }
}

//-------------------------------------------------------------------------------------------------

void CGenerateFunction::getProceduralParameters(CXMLNode xFunctions, CXMLNode xParams)
{
    if (xParams.attributes()[ParamName_InputScale].isEmpty() == false)
    {
        m_dInputScale = xParams.attributes()[ParamName_InputScale].toDouble();
    }

    if (xParams.attributes()[ParamName_OutputScale].isEmpty() == false)
    {
        m_dOutputScale = xParams.attributes()[ParamName_OutputScale].toDouble();
    }

    if (xParams.attributes()[ParamName_MinClamp].isEmpty() == false)
    {
        m_dMinClamp = xParams.attributes()[ParamName_MinClamp].toDouble();
    }

    if (xParams.attributes()[ParamName_MaxClamp].isEmpty() == false)
    {
        m_dMaxClamp = xParams.attributes()[ParamName_MaxClamp].toDouble();
    }

    if (xParams.attributes()[ParamName_Displace].isEmpty() == false)
    {
        m_dDisplace = xParams.attributes()[ParamName_Displace].toDouble();
    }

    if (xParams.attributes()[ParamName_Iterations].isEmpty() == false)
    {
        m_dIterations = xParams.attributes()[ParamName_Iterations].toInt();

        if (m_dIterations <  1) m_dIterations =  1;
        if (m_dIterations > 50) m_dIterations = 50;
    }

    if (xParams.attributes()["InputScaleFactor"].isEmpty() == false)
    {
        m_dInputScaleFactor = xParams.attributes()["InputScaleFactor"].toDouble();
    }

    if (xParams.attributes()["OutputScaleFactor"].isEmpty() == false)
    {
        m_dOutputScaleFactor = xParams.attributes()["OutputScaleFactor"].toDouble();
    }

    CXMLNode xOffset = xParams.getNodeByTagName(ParamName_Offset);

    if (xOffset.isEmpty() == false)
    {
        m_vOffset = CVector3(
                    xOffset.attributes()[ParamName_x].toDouble(),
                    xOffset.attributes()[ParamName_y].toDouble(),
                    xOffset.attributes()[ParamName_z].toDouble()
                    );
    }
}

//-------------------------------------------------------------------------------------------------

double CGenerateFunction::process(CPerlin* pPerlin, const CVector3& vPosition, const Math::CAxis& aAxis) const
{
    switch (m_eType)
    {
        case toNone:
            break;

        case toConstant:
            return m_dConstant;

        case toAdd:
        case toSub:
        case toMul:
        case toDiv:
        {
            if (m_vOperands.count() > 0)
            {
                double dReturnValue = m_vOperands[0]->process(pPerlin, vPosition + m_vOffset, aAxis);

                for (int iIndex = 1; iIndex < m_vOperands.count(); iIndex++)
                {
                    double dCurrentValue = m_vOperands[iIndex]->process(pPerlin, vPosition + m_vOffset, aAxis);

                    switch (m_eType)
                    {
                        case toNone:
                        case toConstant : break;
                        case toAdd: dReturnValue += dCurrentValue; break;
                        case toSub: dReturnValue -= dCurrentValue; break;
                        case toMul: dReturnValue *= dCurrentValue; break;
                        case toDiv: dReturnValue /= dCurrentValue; break;
                        default:
                            break;
                    }
                }

                return dReturnValue;
            }

            return 0.0;
        }

        case toPow:
        {
            if (m_vOperands.count() > 0)
            {
                return pow(m_vOperands[0]->process(pPerlin, vPosition + m_vOffset, aAxis), m_dConstant);
            }

            return 0.0;
        }

        case toPerlin:
        case toTurbulence:
        case toErosion:
        case toVoronoi:
        {
            double dValue = 0.0;
            double dInputScale = m_dInputScale;
            double dOutputScale = m_dOutputScale;

            for (int iIteration = 0; iIteration < m_dIterations; iIteration++)
            {
                double dNewValue = 0.0;

                switch (m_eType)
                {
                    case toPerlin:
                        dNewValue = pPerlin->noise((vPosition * dInputScale) + m_vOffset);
                        break;

                    case toTurbulence:
                        dNewValue = pPerlin->turbulence((vPosition * dInputScale) + m_vOffset);
                        break;

                    case toErosion:
                        dNewValue = pPerlin->erosion((vPosition * dInputScale) + m_vOffset, aAxis, m_dDisplace);
                        break;

                    case toVoronoi:
                        dNewValue = pPerlin->voronoi((vPosition * dInputScale) + m_vOffset, aAxis, m_dDisplace);
                        break;

                    default:
                        break;
                }

                if (dNewValue < m_dMinClamp) dNewValue = m_dMinClamp;
                if (dNewValue > m_dMaxClamp) dNewValue = m_dMaxClamp;

                dValue += dNewValue * dOutputScale;

                dInputScale *= m_dInputScaleFactor;
                dOutputScale *= m_dOutputScaleFactor;
            }

            return dValue;
        }
    }

    return 0.0;
}
