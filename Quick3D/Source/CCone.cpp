
// Application
#include "CCone.h"
#include "C3DScene.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CCone::CCone(
        C3DScene* pScene,
        double dMaxDistance,
        int iNumSegments,
        int iNumHeightSegments,
        double dHeight,
        double dBaseRadius,
        double dApexRadius
        )
    : CMesh(pScene, dMaxDistance)
{
    setName("Cone");

    m_iGLType = GL_QUADS;

    if (iNumSegments < 2) iNumSegments = 2;

    setMaterial(m_pScene->getRessourcesManager()->getDefaultMaterial());
    getMaterials()[0]->setIRFactor(0.4);

    double dRadiusRange = dApexRadius - dBaseRadius;

    for (int iHeightIndex = 0; iHeightIndex <= iNumHeightSegments; iHeightIndex++)
    {
        double dCurrentHeight = ((double) iHeightIndex / (double) iNumHeightSegments) * dHeight;

        for (int iIndex = 0; iIndex < iNumSegments; iIndex++)
        {
            double dCurrentRadius = dBaseRadius + ((double) iHeightIndex / (double) iNumHeightSegments) * dRadiusRange;
            CVector3 v1(0.0, dCurrentHeight, dCurrentRadius);

            double dU = (double) iIndex / (double) iNumSegments;

            double dAngle = dU * (Math::Pi * 2.0);

            CMatrix4 mRotate = CMatrix4().MakeRotation(CVector3(0.0, dAngle, 0.0));

            double dTexU = (double) iIndex / (double) (iNumSegments - 1);
            double dTexV = (double) iHeightIndex / (double) (iNumHeightSegments - 1);

            m_vVertices.append(CVertex(mRotate * v1, CVector2(dTexU, dTexV)));
        }
    }

    for (int iHeightIndex = 0; iHeightIndex < iNumHeightSegments; iHeightIndex++)
    {
        for (int iIndex = 0; iIndex < iNumSegments; iIndex++)
        {
            int iV1 = ((iHeightIndex + 0) * iNumSegments) + (iIndex + 0);
            int iV2 = ((iHeightIndex + 0) * iNumSegments) + (iIndex + 1);
            int iV3 = ((iHeightIndex + 1) * iNumSegments) + (iIndex + 1);
            int iV4 = ((iHeightIndex + 1) * iNumSegments) + (iIndex + 0);

            if (iIndex == iNumSegments - 1)
            {
                iV2 = ((iHeightIndex + 0) * iNumSegments) + 0;
                iV3 = ((iHeightIndex + 1) * iNumSegments) + 0;
            }

            m_vFaces.append(CFace(this, iV4, iV3, iV2, iV1));
        }
    }

    setGeometryDirty(true);
}

//-------------------------------------------------------------------------------------------------

CCone::~CCone()
{
}
