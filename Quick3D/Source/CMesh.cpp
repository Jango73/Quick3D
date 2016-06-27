
// Application
#include "CLogManager.h"
#include "CRessourcesManager.h"
#include "C3DScene.h"
#include "CMesh.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CMesh::instanciator(C3DScene* pScene)
{
    return new CMesh(pScene);
}

//-------------------------------------------------------------------------------------------------

CMesh::CMesh(C3DScene* pScene, double dMaxDistance, bool bUseSpacePartitionning)
    : CPhysicalComponent(pScene)
{
    m_pGeometry = QSP<CMeshGeometry>(new CMeshGeometry(pScene, dMaxDistance, bUseSpacePartitionning));
}

//-------------------------------------------------------------------------------------------------

CMesh::~CMesh()
{
}

//-------------------------------------------------------------------------------------------------

void CMesh::setGeometry(QSP<CMeshGeometry> pGeometry)
{
    m_pGeometry = pGeometry;
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CMesh::getBounds()
{
    if (m_pGeometry)
    {
        return m_pGeometry->getBounds();
    }

    return CBoundingBox();
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CMesh::getWorldBounds()
{
    if (m_pGeometry)
    {
        return m_pGeometry->getWorldBounds(this);
    }

    return CBoundingBox();
}

//-------------------------------------------------------------------------------------------------

void CMesh::update(double dDeltaTime)
{
    CPhysicalComponent::update(dDeltaTime);

    if (m_pGeometry)
    {
        m_pGeometry->update(dDeltaTime);
    }
}

//-------------------------------------------------------------------------------------------------

void CMesh::loadParameters(const QString& sBaseFile, CXMLNode xComponent)
{
    CPhysicalComponent::loadParameters(sBaseFile, xComponent);

    CXMLNode xMeshNode = xComponent.getNodeByTagName(ParamName_Mesh);
    CXMLNode xProceduralMeshNode = xComponent.getNodeByTagName(ParamName_ProceduralMesh);
    CXMLNode xIRNode = xComponent.getNodeByTagName(ParamName_IR);
    CXMLNode xBoundsNode = xMeshNode.getNodeByTagName(ParamName_Bounds);
    QVector<CXMLNode> xDynTexNodes = xComponent.getNodesByTagName(ParamName_DynamicTexture);

    if (xMeshNode.isEmpty() == false)
    {
        QString sName = xMeshNode.attributes()[ParamName_Name];

        if (sName != "")
        {
            m_pGeometry = m_pScene->getRessourcesManager()->loadMesh(sBaseFile, sName, this);

            if (xIRNode.attributes()[ParamName_Factor].isEmpty() == false)
            {
                if (m_pGeometry->getMaterials().count() > 0)
                {
                    m_pGeometry->getMaterials()[0]->setIRFactor(xIRNode.attributes()[ParamName_Factor].toDouble());
                }
            }

            foreach (CXMLNode xDynTexNode, xDynTexNodes)
            {
                QString sTextureName = xDynTexNode.attributes()[ParamName_Name];
                QString sUpdaterName = xDynTexNode.attributes()[ParamName_Updater];

                m_pGeometry->getDynTexUpdaters()[sTextureName] = sUpdaterName;
            }
        }

        if (xBoundsNode.isEmpty() == false && m_pGeometry)
        {
            CXMLNode xMinimum = xBoundsNode.getNodeByTagName(ParamName_Minimum);
            CXMLNode xMaximum = xBoundsNode.getNodeByTagName(ParamName_Maximum);

            if (xMinimum.isEmpty() == false && xMaximum.isEmpty() == false)
            {
                CVector3 vMinimum(
                            xMinimum.attributes()[ParamName_x].toDouble(),
                            xMinimum.attributes()[ParamName_y].toDouble(),
                            xMinimum.attributes()[ParamName_z].toDouble()
                            );

                CVector3 vMaximum(
                            xMaximum.attributes()[ParamName_x].toDouble(),
                            xMaximum.attributes()[ParamName_y].toDouble(),
                            xMaximum.attributes()[ParamName_z].toDouble()
                            );

                m_pGeometry->setBounds(CBoundingBox(vMinimum, vMaximum));
            }
        }
    }
    else if (xProceduralMeshNode.isEmpty() == false)
    {
        QString sName = xProceduralMeshNode.attributes()[ParamName_Name];

        if (sName == ParamName_Sphere)
        {
            m_pGeometry->createSphere(16);
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CMesh::solveLinks(C3DScene* pScene)
{
    CPhysicalComponent::solveLinks(pScene);

    if (m_pGeometry)
    {
        foreach (QString sTextureName, m_pGeometry->getDynTexUpdaters().keys())
        {
            bool bFound = false;

            foreach (QSP<CComponent> pComponent, pScene->getComponents())
            {
                QString sUpdaterName = m_pGeometry->getDynTexUpdaters()[sTextureName];

                QSP<CComponent> pFound = pComponent->findComponent(sUpdaterName, QSP<CComponent>(this));

                if (pFound)
                {
                    foreach (QSP<CMaterial> pMaterial, m_pGeometry->getMaterials())
                    {
                        foreach (CTexture* pTexture, pMaterial->getDiffuseTextures())
                        {
                            if (pTexture->getName().contains(sTextureName))
                            {
                                bFound = true;

                                pTexture->setUpdater(pFound.data());
                            }
                        }

                        if (bFound) break;
                    }
                }

                if (bFound) break;
            }

            if (bFound == false)
            {
                LOG_ERROR(QString("CMesh::solveLinks() : Could not resolve texture updater for %1").arg(sTextureName));
            }
        }

        m_pGeometry->getDynTexUpdaters().clear();
    }
}

//-------------------------------------------------------------------------------------------------

void CMesh::clearLinks(C3DScene* pScene)
{
    CPhysicalComponent::clearLinks(pScene);
}

//-------------------------------------------------------------------------------------------------

void CMesh::flipNormals()
{
    CPhysicalComponent::flipNormals();

    if (m_pGeometry)
    {
        m_pGeometry->flipNormals();
    }
}

//-------------------------------------------------------------------------------------------------

void CMesh::transformVertices(const CMatrix4& matrix)
{
    CPhysicalComponent::transformVertices(matrix);

    if (m_pGeometry)
    {
        m_pGeometry->transformVertices(matrix);
    }
}

//-------------------------------------------------------------------------------------------------

RayTracingResult CMesh::intersect(CRay3 ray)
{
    if (m_pGeometry)
    {
        return m_pGeometry->intersect(this, ray);
    }

    return RayTracingResult(Q3D_INFINITY);
}

//-------------------------------------------------------------------------------------------------

void CMesh::paint(CRenderContext* pContext)
{
    if (m_pGeometry)
    {
        m_pGeometry->paint(pContext, this);
    }
}

//-------------------------------------------------------------------------------------------------

CComponent* CMesh::createMultiTextureSphere(C3DScene* pScene, int iNumSegments, int m_iPanCount, int m_iTiltCount, double dMaxDistance)
{
    CComponent* pComponent = new CComponent(pScene);

    pComponent->setName("MultiTextureSphere");

    int iSegmentsPerPanPart = iNumSegments / m_iPanCount;
    int iSegmentsPerTiltPart = iNumSegments / m_iTiltCount;

    double dPanPartSpanDegrees = 360.0 / (double) m_iPanCount;
    double dTiltPartSpanDegrees = 180.0 / (double) m_iTiltCount;

    for (int iTiltIndex = 0; iTiltIndex < m_iTiltCount; iTiltIndex++)
    {
        for (int iPanIndex = 0; iPanIndex < m_iPanCount; iPanIndex++)
        {
            CMesh* pMesh = new CMesh(pScene, dMaxDistance);

            pMesh->m_pGeometry->createSpherePart(
                        iSegmentsPerPanPart,
                        iSegmentsPerTiltPart,
                        ((double) iPanIndex * dPanPartSpanDegrees),
                        ((double) iPanIndex * dPanPartSpanDegrees) + dPanPartSpanDegrees,
                        ((double) iTiltIndex * dTiltPartSpanDegrees),
                        ((double) iTiltIndex * dTiltPartSpanDegrees) + dTiltPartSpanDegrees
                        );

            pMesh->setParent(QSP<CComponent>(pComponent));
        }
    }

    return pComponent;
}

//-------------------------------------------------------------------------------------------------

void CMesh::dump(QTextStream& stream, int iIdent)
{
    dumpIdent(stream, iIdent, QString("[CMesh]"));

    if (m_pGeometry)
    {
        m_pGeometry->dump(stream, iIdent);
    }

    CPhysicalComponent::dump(stream, iIdent);
}
