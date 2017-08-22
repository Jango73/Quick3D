
// Application
#include "C3DScene.h"
#include "CBuildingGenerator.h"
#include "CPerlin.h"
#include "CBox.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CBuildingGenerator::CBuildingGenerator(C3DScene* pScene)
    : m_mMutex(QMutex::Recursive)
    , m_pScene(pScene)
    , m_pBuilding1(nullptr)
{
    m_pBuilding1 = QSP<CMesh>(new CBox(m_pScene, 150.0));

    // m_pScene->getRessourcesManager()->share(m_pBuilding1);
}

//-------------------------------------------------------------------------------------------------

CBuildingGenerator::~CBuildingGenerator()
{
}

//-------------------------------------------------------------------------------------------------

CMeshInstance* CBuildingGenerator::getBuilding(EBuildingType eType, CGeoloc gPosition, double dRadius, double dSeed)
{
    return nullptr;

    /*
    if (m_pScene == nullptr) return nullptr;

    QMutexLocker locker(&m_mMutex);

    CMeshInstance* pMeshInstance = m_pScene->getRessourcesManager()->share(m_pBuilding1);
    CPerlin* perlin = CPerlin::getInstance();

    double dBuildingTypeScale = 0.04;

    switch (eType)
    {
    case BT_Skyscraper:
        {
            double dHeight = 150.0 + perlin->getNoise_0_1(vPosition * dBuildingTypeScale) * 150.0;
            double dBasemintHeight = 10.0;
            double dWidth = 20.0 + perlin->getNoise_0_1(vPosition * dBuildingTypeScale) * 20.0;

            Vector3 vNewPosition(vPosition.X, vPosition.Y - dBasemintHeight + (dHeight / 2.0), vPosition.Z);
            Vector3 vNewScale(dWidth, dHeight + dBasemintHeight, dWidth);

            pMeshInstance->setPosition(vNewPosition);
            pMeshInstance->setScale(vNewScale);
        }
        break;

    case BT_MiddleSized:
        {
            double dHeight = 15.0 + perlin->getNoise_0_1(vPosition * dBuildingTypeScale) * 15.0;
            double dBasemintHeight = 10.0;
            double dWidth = 20.0 + perlin->getNoise_0_1(vPosition * dBuildingTypeScale) * 20.0;

            Vector3 vNewPosition(vPosition.X, vPosition.Y - dBasemintHeight + (dHeight / 2.0), vPosition.Z);
            Vector3 vNewScale(dWidth, dHeight + dBasemintHeight, dWidth);

            pMeshInstance->setPosition(vNewPosition);
            pMeshInstance->setScale(vNewScale);
        }
        break;

    case BT_House:
        {
            double dHeight = 4.0 + perlin->getNoise_0_1(vPosition * dBuildingTypeScale) * 2.0;
            double dBasemintHeight = 10.0;
            double dWidth = 10.0 + perlin->getNoise_0_1(vPosition * dBuildingTypeScale) * 5.0;

            Vector3 vNewPosition(vPosition.X, vPosition.Y - dBasemintHeight + (dHeight / 2.0), vPosition.Z);
            Vector3 vNewScale(dWidth, dHeight + dBasemintHeight, dWidth);

            pMeshInstance->setPosition(vNewPosition);
            pMeshInstance->setScale(vNewScale);
        }
        break;
    }

    return pMeshInstance;
    */
}
