
// qt-plus
#include "CLogger.h"

// Application
#include "C3DScene.h"
#include "CVegetationGenerator.h"
#include "CWorldChunk.h"
#include "CPerlin.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CVegetationGenerator::instantiator(C3DScene* pScene)
{
    return new CVegetationGenerator(pScene);
}

//-------------------------------------------------------------------------------------------------

CVegetationGenerator::CVegetationGenerator(C3DScene* pScene)
: CGeometryGenerator(pScene)
{
}

//-------------------------------------------------------------------------------------------------

CVegetationGenerator::~CVegetationGenerator()
{
}

//-------------------------------------------------------------------------------------------------

void CVegetationGenerator::loadParameters(const QString& sBaseFile, const CXMLNode& xVegetationNode, CXMLNode xFunctions)
{
    CGeometryGenerator::loadParameters(sBaseFile, xVegetationNode, xFunctions);

    QVector<CXMLNode> xTrees = xVegetationNode.getNodesByTagName(ParamName_Tree);

    foreach (CXMLNode xTree, xTrees)
    {
        CXMLNode xGeneral = xTree.getNodeByTagName(ParamName_General);
        CXMLNode xDNA = xTree.getNodeByTagName(ParamName_DNA);
        CXMLNode xCoverage = xTree.getNodeByTagName(ParamName_Coverage);

        CGenerateFunction* pFunction = new CGenerateFunction(xFunctions, xCoverage.getNodeByTagName(ParamName_Value));

        double dSpread = xGeneral.attributes()[ParamName_Spread].toDouble();
        int iLevels = xDNA.attributes()[ParamName_Levels].toDouble();
        double dTrunkLength = xDNA.attributes()[ParamName_TrunkLength].toDouble();
        double dTrunkRadius = xDNA.attributes()[ParamName_TrunkRadius].toDouble();
        double dBranchLengthScale = xDNA.attributes()[ParamName_BranchLengthScale].toDouble();
        double dBranchRadiusScale = xDNA.attributes()[ParamName_BranchRadiusScale].toDouble();
        double dLeafScale = xDNA.attributes()[ParamName_LeafScale].toDouble();
        double dGravityFactor = xDNA.attributes()[ParamName_GravityFactor].toDouble();

        CXMLNode xLeaf = xDNA.getNodeByTagName(ParamName_Leaf);
        CXMLNode xFFD = xLeaf.getNodeByTagName(ParamName_FFD);

        QVector<CXMLNode> xPoints = xFFD.getNodesByTagName(ParamName_Point);

        QVector<CVector3> vFFDFrom;
        QVector<CVector3> vFFDTo;

        foreach (CXMLNode xPoint, xPoints)
        {
            vFFDFrom.append(CVector3(
                                xPoint.attributes()["fx"].toDouble(),
                            xPoint.attributes()["fy"].toDouble(),
                    xPoint.attributes()["fz"].toDouble()
                    ));

            vFFDTo.append(CVector3(
                              xPoint.attributes()["tx"].toDouble(),
                          xPoint.attributes()["ty"].toDouble(),
                    xPoint.attributes()["tz"].toDouble()
                    ));
        }

        QVector<QSP<CMesh> > vMeshes;

        CVector3 vNoisePosition(
                    ((double) rand() / 32768.0) * 2.0,
                    ((double) rand() / 32768.0) * 2.0,
                    ((double) rand() / 32768.0) * 2.0
                    );

        for (int iLODLevel = 0; iLODLevel < 5; iLODLevel++)
        {
            CMesh* pMesh = new CMesh(m_pScene);

            CMeshGeometry* pMeshGeometry = m_pScene->treeGenerator()->createTree(
                        iLODLevel,
                        vNoisePosition,
                        iLevels,
                        dTrunkLength,
                        dTrunkRadius,
                        dBranchLengthScale,
                        dBranchRadiusScale,
                        dLeafScale,
                        dGravityFactor,
                        vFFDFrom,
                        vFFDTo
                        );

            pMesh->setGeometry(QSP<CMeshGeometry>(pMeshGeometry));
            vMeshes.append(QSP<CMesh>(pMesh));
        }

        m_vVegetation.append(QSP<CVegetation>(new CVegetation(CVegetation::evtTree, dSpread, pFunction, new CMeshInstance(vMeshes), nullptr)));
    }

    QVector<CXMLNode> xBushes = xVegetationNode.getNodesByTagName(ParamName_Bush);

    foreach (CXMLNode xBush, xBushes)
    {
        CXMLNode xGeneral = xBush.getNodeByTagName(ParamName_General);
        CXMLNode xCoverage = xBush.getNodeByTagName(ParamName_Coverage);
        CXMLNode xMaterial = xBush.getNodeByTagName(ParamName_Material);

        if (xCoverage.isEmpty() == false && xGeneral.attributes()[ParamName_Spread].isEmpty() == false)
        {
            CGenerateFunction* pFunction = new CGenerateFunction(xFunctions, xCoverage.getNodeByTagName(ParamName_Value));

            double dSpread = xGeneral.attributes()[ParamName_Spread].toDouble();

            if (xMaterial.isEmpty() == false)
            {
                CMaterial* pMaterial = new CMaterial(m_pScene);
                pMaterial->loadParameters(sBaseFile, xMaterial);
                pMaterial->setBillBoard(true);
                pMaterial->setHasAlpha(true);
                m_vVegetation.append(QSP<CVegetation>(new CVegetation(CVegetation::evtBush, dSpread, pFunction, nullptr, pMaterial)));
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CVegetationGenerator::generate(QSP<CWorldChunk> pChunk)
{
    CPerlin* perlin = CPerlin::getInstance();

    // Generate vegetation
    for (int iVegetIndex = 0; iVegetIndex < m_vVegetation.count(); iVegetIndex++)
    {
        QSP<CVegetation> pVegetation = m_vVegetation[iVegetIndex];

        double dSpread = pVegetation->m_dSpread * ((double) pChunk->terrain()->level() + 1.0);
        double dAltitude_Trees = 10.0;

        CGeoloc gStart(pChunk->geoloc().Latitude - pChunk->size().Latitude * 0.5, pChunk->geoloc().Longitude - pChunk->size().Longitude * 0.5, 0.0);

        double dLatStart = fmod(gStart.Latitude, dSpread);
        double dLonStart = fmod(gStart.Longitude, dSpread);

        if (dLatStart < 0.0) dLatStart -= dSpread * 0.5;
        if (dLonStart < 0.0) dLonStart -= dSpread * 0.5;

        CGeoloc gStartOffset = CGeoloc(gStart.Latitude - dLatStart, gStart.Longitude - dLonStart, 0.0);

        for (double dLat = gStartOffset.Latitude; dLat < gStartOffset.Latitude + pChunk->size().Latitude; dLat += dSpread)
        {
            for (double dLon = gStartOffset.Longitude; dLon < gStartOffset.Longitude + pChunk->size().Longitude; dLon += dSpread)
            {
                if (
                        dLat > gStart.Latitude && dLat < gStart.Latitude + pChunk->size().Latitude &&
                        dLon > gStart.Longitude && dLon < gStart.Longitude + pChunk->size().Longitude
                        )
                {
                    CGeoloc gPosition(dLat, dLon, 0.0);

                    double dLandscapeValue = pVegetation->m_pFunction->process(perlin, gPosition.toVector3(), CAxis());

                    if (dLandscapeValue > 0.0)
                    {
                        double dRigidness = 0.0;
                        gPosition.Altitude = pChunk->terrain()->getHeightAt(gPosition, &dRigidness);

                        if (gPosition.Altitude >= dAltitude_Trees)
                        {
                            switch (pVegetation->m_eType)
                            {
                                case CVegetation::evtTree:
                                    placeTree(pChunk, gPosition, 5.0, iVegetIndex);
                                    break;

                                case CVegetation::evtBush:
                                    placeBush(pChunk, gPosition, 5.0, iVegetIndex);
                                    break;

                                default:
                                    break;
                            }
                        }
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CVegetationGenerator::placeTree(QSP<CWorldChunk> pChunk, CGeoloc gPosition, double dRadius, int iVegetIndex)
{
    if (pChunk->checkPositionFree(gPosition, dRadius))
    {
        CMeshInstance* pMeshInstance = m_vVegetation[iVegetIndex]->m_pMesh->clone();

        if (pMeshInstance != nullptr)
        {
            pMeshInstance->setGeoloc(gPosition);
            pMeshInstance->setRotation(CVector3(0.0, ((double) rand() / 32768.0) * Math::Pi * 2.0, 0.0));
            pMeshInstance->computeWorldTransform();

            foreach (CBoundedMeshInstances* pBounded, pChunk->meshes())
            {
                if (pBounded->worldBounds().contains(gPosition, dRadius))
                {
                    pBounded->add(pMeshInstance);
                    return;
                }
            }

            delete pMeshInstance;
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CVegetationGenerator::placeBush(QSP<CWorldChunk> pChunk, CGeoloc gPosition, double dRadius, int iVegetIndex)
{
    if (m_vVegetation[iVegetIndex]->m_pMaterial)
    {
        QString sMaterialName = m_vVegetation[iVegetIndex]->m_pMaterial->name();

        if (sMaterialName.isEmpty() == false)
        {
            if (pChunk->bushMeshes().contains(sMaterialName) == false)
            {
                QSP<CMeshGeometry> pBushMesh = QSP<CMeshGeometry>(new CMeshGeometry(m_pScene, 100000.0));

                pBushMesh->setGLType(GL_POINTS);
                pBushMesh->setMaterial(m_vVegetation[iVegetIndex]->m_pMaterial);

                pChunk->bushMeshes()[sMaterialName] = pBushMesh;
            }

            CVector3 vGeocentricPosition = gPosition.toVector3();
            CVector3 vPosition = vGeocentricPosition - pChunk->worldPosition();

            CVertex newVertex(vPosition);
            newVertex.setNormal(vGeocentricPosition.normalized());
            pChunk->bushMeshes()[sMaterialName]->vertices().append(newVertex);
        }
    }
}
