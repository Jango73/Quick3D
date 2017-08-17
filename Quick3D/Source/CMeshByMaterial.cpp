
// Application
#include "CMeshByMaterial.h"
#include "C3DScene.h"

#ifndef WIN32
#include <GL/glext.h>
#endif

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CMeshByMaterial::CMeshByMaterial()
{
}

//-------------------------------------------------------------------------------------------------

CMeshByMaterial::~CMeshByMaterial()
{
}

//-------------------------------------------------------------------------------------------------

void CMeshByMaterial::addGeometry(CComponent* pContainer, CMeshGeometry* pGeometry)
{
    if (pGeometry->glMeshData().count() > 0 && pGeometry->glMeshData().count() == pGeometry->materials().count())
    {
        for (int iMaterialIndex = 0; iMaterialIndex < pGeometry->materials().count(); iMaterialIndex++)
        {
            CGLMeshData* pData = pGeometry->glMeshData()[iMaterialIndex];
            CMaterial* pMaterial = pGeometry->materials()[iMaterialIndex].data();

            if (m_vGeometry.contains(pMaterial) == false)
            {
                m_vGeometry[pMaterial] = QMap<CGLMeshData*, CComponent*>();
            }

            m_vGeometry[pMaterial][pData] = pContainer;
        }
    }
}

//-------------------------------------------------------------------------------------------------

bool compareMaterialsByAlpha(CMaterial* pMat1, CMaterial* pMat2)
{
    return (int) pMat1->hasAlpha() < (int) pMat2->hasAlpha();
}

//-------------------------------------------------------------------------------------------------

void CMeshByMaterial::paint(CRenderContext* pContext)
{
    QList<CMaterial*> vMaterials;

    foreach (CMaterial* pMaterial, m_vGeometry.keys())
    {
        vMaterials.append(pMaterial);
    }

    qSort(vMaterials.begin(), vMaterials.end(), compareMaterialsByAlpha);

    foreach (CMaterial* pMaterial, vMaterials)
    {
        // Get a program from object material
        QGLShaderProgram* pProgram = pMaterial->activate(pContext);

        // If program ok...
        if (pProgram != nullptr)
        {
            foreach (CGLMeshData* pData, m_vGeometry[pMaterial].keys())
            {
                CComponent* pContainer = m_vGeometry[pMaterial][pData];
                QMatrix4x4 mModelAbsolute;

                mModelAbsolute.setToIdentity();

                if (pContainer != nullptr)
                {
                    // Set transform matrix
                    CVector3 WorldPosition = pContainer->getWorldPosition() - pContext->scene()->worldOrigin();
                    CVector3 WorldRotation = pContainer->getWorldRotation();
                    CVector3 WorldScale = pContainer->getWorldScale();

                    mModelAbsolute.translate(WorldPosition.X, WorldPosition.Y, WorldPosition.Z);
                    mModelAbsolute.rotate(Math::Angles::toDeg(WorldRotation.Y), QVector3D(0, 1, 0));
                    mModelAbsolute.rotate(Math::Angles::toDeg(WorldRotation.X), QVector3D(1, 0, 0));
                    mModelAbsolute.rotate(Math::Angles::toDeg(WorldRotation.Z), QVector3D(0, 0, 1));
                    mModelAbsolute.scale(WorldScale.X, WorldScale.Y, WorldScale.Z);
                }

                pData->paint(pContext, mModelAbsolute, pProgram, pData->m_iGLType);
            }
        }
    }
}
