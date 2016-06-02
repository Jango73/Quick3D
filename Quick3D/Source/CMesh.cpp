
// Application
#include "CLogManager.h"
#include "CRessourcesManager.h"
#include "COBJLoader.h"
#include "C3DScene.h"
#include "CMesh.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------
// Constantes

#define MAX_PARTITION_LEVEL		1

//-------------------------------------------------------------------------------------------------
// Propriétés statiques

GLuint CMesh::m_iCurrentVBO = 0;

//-------------------------------------------------------------------------------------------------

CGLMeshData::CGLMeshData(C3DScene* pScene)
	: m_pScene(pScene)
	, m_iNumRenderPoints(0)
	, m_iNumRenderIndices(0)
	, m_vRenderPoints(NULL)
	, m_vRenderIndices(NULL)
{
	m_iVBO[0] = 0;
	m_iVBO[1] = 0;

	m_pScene->makeCurrentRenderingContext();

	GL_glGenBuffers(2, m_iVBO);
}

//-------------------------------------------------------------------------------------------------

CGLMeshData::~CGLMeshData()
{
	if (m_vRenderPoints != NULL)
	{
		delete [] m_vRenderPoints;
	}

	if (m_vRenderIndices != NULL)
	{
		delete [] m_vRenderIndices;
	}

	m_pScene->makeCurrentRenderingContext();

	GL_glDeleteBuffers(2, m_iVBO);
}

//-------------------------------------------------------------------------------------------------

CComponent* CMesh::instanciator(C3DScene* pScene)
{
	return new CMesh(pScene);
}

//-------------------------------------------------------------------------------------------------

CMesh::CMesh(C3DScene* pScene, double dMaxDistance, bool bUseSpacePartitionning)
	: CPhysicalComponent(pScene)
	, m_mMutex(QMutex::Recursive)
	, m_dMaxDistance(dMaxDistance)
	, m_bAllQuads(false)
	, m_bNeedTransferBuffers(true)
	, m_bUseSpacePartitionning(bUseSpacePartitionning)
{
	m_vMaterials.append(m_pScene->getRessourcesManager()->getDefaultMaterial());
	m_vGLMeshData.append(new CGLMeshData(m_pScene));
}

//-------------------------------------------------------------------------------------------------

CMesh::~CMesh()
{
	foreach (CGLMeshData* data, m_vGLMeshData)
	{
		delete data;
	}

	deleteMaterials();
}

//-------------------------------------------------------------------------------------------------

void CMesh::deleteMaterials()
{
	QMutexLocker locker(&m_mMutex);

	m_vMaterials.clear();
}

//-------------------------------------------------------------------------------------------------

void CMesh::setMaterial(QSharedPointer<CMaterial> pMaterial)
{
	deleteMaterials();

	m_vMaterials.append(m_pScene->getRessourcesManager()->shareMaterial(pMaterial));
}

//-------------------------------------------------------------------------------------------------

void CMesh::setBounds(CBoundingBox bBox)
{
	m_bBounds = bBox;
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CMesh::getBounds() const
{
	return m_bBounds;
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CMesh::getWorldBounds() const
{
	// Récupération de la position "monde"
	CVector3 vWorldPosition = getWorldPosition();

	// Retour de la boite englobante
	return CBoundingBox(vWorldPosition + m_bBounds.minimum(), vWorldPosition + m_bBounds.maximum());
}

//-------------------------------------------------------------------------------------------------

void CMesh::update(double dDeltaTime)
{
	CPhysicalComponent::update(dDeltaTime);

	foreach (QSharedPointer<CMaterial> pMaterial, m_vMaterials)
	{
		if (pMaterial != NULL)
		{
			pMaterial->update(dDeltaTime);
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CMesh::loadParameters(CXMLNode xComponent)
{
	CPhysicalComponent::loadParameters(xComponent);

	CXMLNode xMeshNode = xComponent.getNodeByTagName(ParamName_Mesh);
	CXMLNode xProceduralMeshNode = xComponent.getNodeByTagName(ParamName_ProceduralMesh);
	CXMLNode xIRNode = xComponent.getNodeByTagName(ParamName_IR);
	QVector<CXMLNode> xDynTexNodes = xComponent.getNodesByTagName(ParamName_DynamicTexture);

	if (xMeshNode.isEmpty() == false)
	{
		QString sName = xMeshNode.m_vAttributes[ParamName_Name];

		if (sName != "")
		{
			m_pScene->getRessourcesManager()->loadMesh(this, sName);

			if (xIRNode.m_vAttributes[ParamName_Factor].isEmpty() == false)
			{
				if (m_vMaterials.count() > 0)
				{
					m_vMaterials[0]->setIRFactor(xIRNode.m_vAttributes[ParamName_Factor].toDouble());
				}
			}

			foreach (CXMLNode xDynTexNode, xDynTexNodes)
			{
				QString sTextureName = xDynTexNode.m_vAttributes[ParamName_Name];
				QString sUpdaterName = xDynTexNode.m_vAttributes[ParamName_Updater];

				m_mDynTexUpdaters[sTextureName] = sUpdaterName;
			}
		}
	}
	else if (xProceduralMeshNode.isEmpty() == false)
	{
		QString sName = xProceduralMeshNode.m_vAttributes[ParamName_Name];

		if (sName == ParamName_Sphere)
		{
			createSphere(16);
		}

		updateGeometry();
	}
}

//-------------------------------------------------------------------------------------------------

void CMesh::solveLinks(C3DScene* pScene)
{
	CPhysicalComponent::solveLinks(pScene);

	foreach (QString sTextureName, m_mDynTexUpdaters.keys())
	{
		bool bFound = false;

		foreach (QSharedPointer<CComponent> pComponent, pScene->getComponents())
		{
			QString sUpdaterName = m_mDynTexUpdaters[sTextureName];

			CComponent* pFound = pComponent->findComponent(sUpdaterName, this);

			if (pFound != NULL)
			{
				foreach (QSharedPointer<CMaterial> pMaterial, m_vMaterials)
				{
					foreach (CTexture* pTexture, pMaterial->getDiffuseTextures())
					{
						if (pTexture->getName().contains(sTextureName))
						{
							bFound = true;

							pTexture->setUpdater(pFound);
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

	m_mDynTexUpdaters.clear();
}

//-------------------------------------------------------------------------------------------------

void CMesh::updateGeometry(bool bComputeNormals)
{
	QMutexLocker locker(&m_mMutex);

	if (m_vVertices.count() > 0)
	{
		// Tri des faces par matériau
		qSort(m_vFaces);

		// Calcul des vecteurs normaux
		if (bComputeNormals)
		{
			computeNormals();
		}

		if (m_vMaterials.count() != m_vGLMeshData.count())
		{
			// Destruction des buffers de géométrie OpenGL
			foreach (CGLMeshData* data, m_vGLMeshData)
			{
				delete data;
			}

			m_vGLMeshData.clear();

			for (int iMaterialIndex = 0; iMaterialIndex < m_vMaterials.count(); iMaterialIndex++)
			{
				m_vGLMeshData.append(new CGLMeshData(m_pScene));
			}
		}

		for (int iMaterialIndex = 0; iMaterialIndex < m_vMaterials.count(); iMaterialIndex++)
		{
			QVector<int> vFaceIndices;

			CGLMeshData* pGLMeshData = m_vGLMeshData[iMaterialIndex];

			// Récupération des faces associées à ce matériau
			for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
			{
				if (m_vFaces[iFaceIndex].getMaterialIndex() == iMaterialIndex)
				{
					vFaceIndices.append(iFaceIndex);
				}
			}

			if (vFaceIndices.count() > 0)
			{
				// Définition des quantités
				pGLMeshData->m_iNumRenderPoints = m_vVertices.count();

				if (m_bAllQuads)
				{
					pGLMeshData->m_iNumRenderIndices = vFaceIndices.count() * 4;
				}
				else
				{
					pGLMeshData->m_iNumRenderIndices = getNumTriangleCountForFaces(vFaceIndices) * 3;
				}

				if (pGLMeshData->m_iNumRenderPoints > 0 && pGLMeshData->m_iNumRenderIndices > 0)
				{
					/*
					LOG_DEBUG(QString("CMesh::updateGeometry() : Allocating %1 vertices and %2 indices for %3")
						.arg(pGLMeshData->m_iNumRenderPoints)
						.arg(pGLMeshData->m_iNumRenderIndices)
						.arg(m_sName)
						);
						*/

					// Création des buffers de géométrie OpenGL
					pGLMeshData->m_vRenderPoints = new CVertex[pGLMeshData->m_iNumRenderPoints];
					pGLMeshData->m_vRenderIndices = new GLuint[pGLMeshData->m_iNumRenderIndices];

					// Remise à zéro de la boite englobante
					m_bBounds.prepare();

					// Remplissage du buffer de vertex OpenGL
					for (int iVertex = 0; iVertex < m_vVertices.count(); iVertex++)
					{
						pGLMeshData->m_vRenderPoints[iVertex] = m_vVertices[iVertex];

						// Check bounding box limits
						if (m_vVertices[iVertex].position().X < m_bBounds.minimum().X) m_bBounds.minimum().X = m_vVertices[iVertex].position().X;
						if (m_vVertices[iVertex].position().Y < m_bBounds.minimum().Y) m_bBounds.minimum().Y = m_vVertices[iVertex].position().Y;
						if (m_vVertices[iVertex].position().Z < m_bBounds.minimum().Z) m_bBounds.minimum().Z = m_vVertices[iVertex].position().Z;
						if (m_vVertices[iVertex].position().X > m_bBounds.maximum().X) m_bBounds.maximum().X = m_vVertices[iVertex].position().X;
						if (m_vVertices[iVertex].position().Y > m_bBounds.maximum().Y) m_bBounds.maximum().Y = m_vVertices[iVertex].position().Y;
						if (m_vVertices[iVertex].position().Z > m_bBounds.maximum().Z) m_bBounds.maximum().Z = m_vVertices[iVertex].position().Z;
					}

					m_bBounds.expand(CVector3(0.1, 0.1, 0.1));

					// Remplissage du buffer d'indices OpenGL
					int iIndiceIndex = 0;

					if (m_bAllQuads)
					{
						for (int iFaceIndex = 0; iFaceIndex < vFaceIndices.count(); iFaceIndex++)
						{
							CFace* pFace = &(m_vFaces[vFaceIndices[iFaceIndex]]);

							pGLMeshData->m_vRenderIndices[iIndiceIndex++] = pFace->getIndices()[0];
							pGLMeshData->m_vRenderIndices[iIndiceIndex++] = pFace->getIndices()[1];
							pGLMeshData->m_vRenderIndices[iIndiceIndex++] = pFace->getIndices()[2];
							pGLMeshData->m_vRenderIndices[iIndiceIndex++] = pFace->getIndices()[3];
						}
					}
					else
					{
						for (int iFaceIndex = 0; iFaceIndex < vFaceIndices.count(); iFaceIndex++)
						{
							CFace* pFace = &(m_vFaces[vFaceIndices[iFaceIndex]]);

							if (pFace->getIndices().count() > 2)
							{
								for (int iIndex = 2; iIndex < pFace->getIndices().count(); iIndex++)
								{
									pGLMeshData->m_vRenderIndices[iIndiceIndex++] = pFace->getIndices()[0];
									pGLMeshData->m_vRenderIndices[iIndiceIndex++] = pFace->getIndices()[iIndex - 1];
									pGLMeshData->m_vRenderIndices[iIndiceIndex++] = pFace->getIndices()[iIndex];
								}
							}
						}
					}
				}
			}

			// Partitionnement spatial
			if (m_bUseSpacePartitionning)
			{
				// Effacement des partitions
				m_mpPartitions.clear();
				m_mpPartitions = CMeshPartition(m_bBounds);

				// Création des partitions
				createPartition(m_mpPartitions, 0);
			}

			// Les buffers doivent être retransmis à OpenGL
			m_bNeedTransferBuffers = true;
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CMesh::createPartition(CMeshPartition& mpCurrentPartition, int iLevel)
{
	if (iLevel < MAX_PARTITION_LEVEL)
	{
		// Définition de la taille totale des partitions enfants
		double dSizeX = mpCurrentPartition.getBounds().maximum().X - mpCurrentPartition.getBounds().minimum().X;
		double dSizeY = mpCurrentPartition.getBounds().maximum().Y - mpCurrentPartition.getBounds().minimum().Y;
		double dSizeZ = mpCurrentPartition.getBounds().maximum().Z - mpCurrentPartition.getBounds().minimum().Z;

		double dNormalizedSizeX = dSizeX / 2.0;
		double dNormalizedSizeY = dSizeY / 2.0;
		double dNormalizedSizeZ = dSizeZ / 2.0;

		// Parcours de toutes les partitions
		for (int iZ = 0; iZ < 2; iZ++)
		{
			for (int iY = 0; iY < 2; iY++)
			{
				for (int iX = 0; iX < 2; iX++)
				{
					// Définition de la boite englobante de la partition enfant
					CVector3 vMinimum;
					CVector3 vMaximum;

					vMinimum.X = m_bBounds.minimum().X + ((double) (iX + 0) * dNormalizedSizeX);
					vMinimum.Y = m_bBounds.minimum().Y + ((double) (iY + 0) * dNormalizedSizeY);
					vMinimum.Z = m_bBounds.minimum().Z + ((double) (iZ + 0) * dNormalizedSizeZ);

					vMaximum.X = m_bBounds.minimum().X + ((double) (iX + 1) * dNormalizedSizeX);
					vMaximum.Y = m_bBounds.minimum().Y + ((double) (iY + 1) * dNormalizedSizeY);
					vMaximum.Z = m_bBounds.minimum().Z + ((double) (iZ + 1) * dNormalizedSizeZ);

					// Ajout de la partition enfant
					CMeshPartition mpChild = CMeshPartition(CBoundingBox(vMinimum, vMaximum));

					createPartition(mpChild, iLevel + 1);

					mpCurrentPartition.addChild(mpChild);
				}
			}
		}
	}
	else
	{
		// Parcours de tous les polygones
		for (int iFace = 0; iFace < m_vFaces.count(); iFace++)
		{
			// Parcours de tous les vertex du polygone en cours
			for (int iVertex = 0; iVertex < m_vFaces[iFace].getIndices().count(); iVertex++)
			{
				// Récupération de la position du vertex
				CVector3 vPosition = m_vVertices[m_vFaces[iFace].getIndices()[iVertex]].position();

				// Est-ce que la partition en cours contient le vertex?
				if (mpCurrentPartition.getBounds().contains(vPosition))
				{
					// Oui, ajout de l'index du polygone à la partition en cours et arrêt
					// de la boucle de vertex
					mpCurrentPartition.getFaceIndices().append(iFace);
					break;
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CMesh::isolateVertices()
{
	// Copie du vecteur de vertex
	QVector<CVertex> vVertices(m_vVertices);

	// Effacement du vecteur de vertex
	m_vVertices.clear();

	// Parcours de tous les polygones
	for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
	{
		for (int iVertexInFaceIndex = 0; iVertexInFaceIndex < m_vFaces[iFaceIndex].getIndices().count(); iVertexInFaceIndex++)
		{
			int iVertexIndex = m_vFaces[iFaceIndex].getIndices()[iVertexInFaceIndex];

			m_vFaces[iFaceIndex].getIndices()[iVertexInFaceIndex] = m_vVertices.count();

			m_vVertices.append(CVertex(vVertices[iVertexIndex]));
		}
	}
}

//-------------------------------------------------------------------------------------------------

// Séparation des polygones en fonction des groupes de lissage
// Algorithme (c) 2015 Jonathan Guyomard
void CMesh::splitVerticesBySmoothingGroup()
{
	// Remise à zéro des données propres aux tri par smoothing group
	{
		for (int iVertexIndex = 0; iVertexIndex < m_vVertices.count(); iVertexIndex++)
		{
			m_vVertices[iVertexIndex].m_vSmoothingGroups.clear();
			m_vVertices[iVertexIndex].m_vVertexIndicesForGroup.clear();
		}
	}

	// Récupération des smoothing groups par sommet
	// Parcours de chaque face
	//   Parcours de chaque sommet de la face
	//     Dans le sommet, on associe au smoothing group de la face le sommet lui-même
	{
		for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
		{
			int iSmoothingGroup = m_vFaces[iFaceIndex].getSmoothingGroup();

			for (int iVertexInFaceIndex = 0; iVertexInFaceIndex < m_vFaces[iFaceIndex].getIndices().count(); iVertexInFaceIndex++)
			{
				int iVertexInMeshIndex = m_vFaces[iFaceIndex].getIndices()[iVertexInFaceIndex];

				if (m_vVertices[iVertexInMeshIndex].m_vSmoothingGroups.contains(iSmoothingGroup) == false)
				{
					m_vVertices[iVertexInMeshIndex].m_vSmoothingGroups.append(iSmoothingGroup);
					m_vVertices[iVertexInMeshIndex].m_vVertexIndicesForGroup.append(iVertexInMeshIndex);
				}
			}
		}
	}

	// Duplication des sommets
	// Parcours de chaque sommet
	//   Si le sommet est associé à plus d'un smoothing group
	{
		int iCurrentVertexCount = m_vVertices.count();

		for (int iVertexInMeshIndex = 0; iVertexInMeshIndex < iCurrentVertexCount; iVertexInMeshIndex++)
		{
			if (m_vVertices[iVertexInMeshIndex].m_vSmoothingGroups.count() > 1)
			{
				if (m_vVertices[iVertexInMeshIndex].m_vSmoothingGroups.count() == m_vVertices[iVertexInMeshIndex].m_vVertexIndicesForGroup.count())
				{
					for (int iGroupIndex = 1; iGroupIndex < m_vVertices[iVertexInMeshIndex].m_vSmoothingGroups.count(); iGroupIndex++)
					{
						int iNewVertexInMeshIndex = m_vVertices.count();
						CVertex vNewVertex(m_vVertices[iVertexInMeshIndex]);
						m_vVertices.append(vNewVertex);
						m_vVertices[iVertexInMeshIndex].m_vVertexIndicesForGroup[iGroupIndex] = iNewVertexInMeshIndex;
					}
				}
				else
				{
					LOG_WARNING("CMesh::splitVerticesBySmoothingGroup() : Inconsistent count of m_vSmoothingGroups");
				}
			}
		}
	}

	// Assignation des nouveaux sommets aux facettes
	{
		for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
		{
			int iSmoothingGroup = m_vFaces[iFaceIndex].getSmoothingGroup();

			for (int iVertexInFaceIndex = 0; iVertexInFaceIndex < m_vFaces[iFaceIndex].getIndices().count(); iVertexInFaceIndex++)
			{
				int iVertexInMeshIndex = m_vFaces[iFaceIndex].getIndices()[iVertexInFaceIndex];

				if (m_vVertices[iVertexInMeshIndex].m_vSmoothingGroups.contains(iSmoothingGroup))
				{
					int iGroupIndex = m_vVertices[iVertexInMeshIndex].m_vSmoothingGroups.indexOf(iSmoothingGroup);
					m_vFaces[iFaceIndex].getIndices()[iVertexInFaceIndex] = m_vVertices[iVertexInMeshIndex].m_vVertexIndicesForGroup[iGroupIndex];
				}
			}
		}
	}

	// Remise à zéro des données propres aux tri par smoothing group
	{
		for (int iVertexIndex = 0; iVertexIndex < m_vVertices.count(); iVertexIndex++)
		{
			m_vVertices[iVertexIndex].m_vSmoothingGroups.clear();
			m_vVertices[iVertexIndex].m_vVertexIndicesForGroup.clear();
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CMesh::flipNormals()
{
	CPhysicalComponent::flipNormals();

	// Parcours de tous les polygones
	for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
	{
		QVector<int> vIndices;

		// Récupération des indices de vertex en sens inverse
		for (int iVertexIndex = 0; iVertexIndex < m_vFaces[iFaceIndex].getIndices().count(); iVertexIndex++)
		{
			vIndices.prepend(m_vFaces[iFaceIndex].getIndices()[iVertexIndex]);
		}

		// Stockage des indices de vertex en sens inverse
		for (int iVertexIndex = 0; iVertexIndex < m_vFaces[iFaceIndex].getIndices().count(); iVertexIndex++)
		{
			m_vFaces[iFaceIndex].getIndices()[iVertexIndex] = vIndices[iVertexIndex];
		}
	}

	updateGeometry();
}

//-------------------------------------------------------------------------------------------------

int CMesh::getNumTriangleCount()
{
	int iNumTriangles = 0;

	foreach (const CFace& face, m_vFaces)
	{
		if (face.getIndices().count() > 2)
		{
			iNumTriangles += face.getIndices().count() - 2;
		}
	}

	return iNumTriangles;
}

//-------------------------------------------------------------------------------------------------

int CMesh::getNumTriangleCountForFaces(const QVector<int>& vFaceIndices)
{
	int iNumTriangles = 0;

	for (int iFaceIndex = 0; iFaceIndex < vFaceIndices.count(); iFaceIndex++)
	{
		CFace* pFace = &(m_vFaces[vFaceIndices[iFaceIndex]]);

		if (pFace->getIndices().count() > 2)
		{
			iNumTriangles += pFace->getIndices().count() - 2;
		}
	}

	return iNumTriangles;
}

//-------------------------------------------------------------------------------------------------

void CMesh::computeNormals()
{
	// Remise à zéro des normales de polygone
	for (int Index = 0; Index < m_vVertices.count(); Index++)
	{
		m_vVertices[Index].normal() = CVector3(0.0, 0.0, 0.0);
		m_vVertices[Index].tangent() = CVector3(0.0, 0.0, 0.0);
		m_vVertices[Index].normalDivider() = 0.0;
	}

	// Parcours de tous les polygones
	for (int FaceIndex = 0; FaceIndex < m_vFaces.count(); FaceIndex++)
	{
		// Calcul du vecteur normal du polygone
		m_vFaces[FaceIndex].computeNormal(this);

		// Traitement de chaque vertex du polygone
		foreach (int VertexIndex, m_vFaces[FaceIndex].getIndices())
		{
			m_vVertices[VertexIndex].normal() += m_vFaces[FaceIndex].getNormal();
			m_vVertices[VertexIndex].tangent() += m_vFaces[FaceIndex].getTangent();
			m_vVertices[VertexIndex].normalDivider() += 1.0;
		}
	}

	// Division des normales et tangentes de chaque vertex
	for (int Index = 0; Index < m_vVertices.count(); Index++)
	{
		if (m_vVertices[Index].normalDivider() > 0.0)
		{
			m_vVertices[Index].normal() /= m_vVertices[Index].normalDivider();
			m_vVertices[Index].tangent() /= m_vVertices[Index].normalDivider();
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CMesh::createSphere(int iNumSegments)
{
	m_vVertices.clear();
	m_vFaces.clear();

	// Le nombre de segments doit être paire
	if ((iNumSegments & 1) == 1)
	{
		iNumSegments++;
	}

	// et supérieur à 3
	if (iNumSegments < 4)
	{
		iNumSegments = 4;
	}

	int iPanSegments = iNumSegments;
	int iTiltSegments = iNumSegments / 2;

	// Sommet du dessus
	m_vVertices.append(CVertex(CVector3(0.0, 1.0, 0.0), CVector2(0.0, 0.0)));

	// Sommets intermédiaires
	for (int iTiltIndex = 1; iTiltIndex < iTiltSegments; iTiltIndex++)
	{
		double dCurrentTilt = (((double) iTiltIndex / (double) iTiltSegments) * Math::Pi);
		CMatrix4 mRotationTilt = CMatrix4().MakeRotation(CVector3(dCurrentTilt, 0.0, 0.0));

		for (int iPanIndex = 0; iPanIndex < iPanSegments; iPanIndex++)
		{
			double dCurrentPan = ((double) iPanIndex / (double) iPanSegments) * (Math::Pi * 2.0);
			CMatrix4 mRotationPan = CMatrix4().MakeRotation(CVector3(0.0, dCurrentPan, 0.0));

			CVector3 vPosition = mRotationPan * (mRotationTilt * CVector3(0.0, 1.0, 0.0));

			m_vVertices.append(CVertex(vPosition, CVector2(0.0, 0.0)));
		}
	}

	// Sommet du dessous
	m_vVertices.append(CVertex(CVector3(0.0, -1.0, 0.0), CVector2(0.0, 0.0)));

	int iVertexOffset = 0;

	// Top faces
	for (int iPanIndex = 0; iPanIndex < iPanSegments; iPanIndex++)
	{
		int v1 = iVertexOffset + 0;
		int v2 = iVertexOffset + 1 + iPanIndex + 1;
		int v3 = iVertexOffset + 1 + iPanIndex;

		if (iPanIndex == iPanSegments - 1)
		{
			v2 -= iPanSegments;
		}

		m_vFaces.append(CFace(this, v1, v2, v3));
	}

	iVertexOffset++;

	// Middle faces
	for (int iTiltIndex = 1; iTiltIndex < iTiltSegments - 1; iTiltIndex++)
	{
		for (int iPanIndex = 0; iPanIndex < iPanSegments; iPanIndex++)
		{
			int v1 = iVertexOffset + 0 + iPanIndex;
			int v2 = iVertexOffset + 1 + iPanIndex;
			int v3 = iVertexOffset + 1 + iPanIndex + iPanSegments;
			int v4 = iVertexOffset + 0 + iPanIndex + iPanSegments;

			if (iPanIndex == iPanSegments - 1)
			{
				v2 -= iPanSegments;
				v3 -= iPanSegments;
			}

			m_vFaces.append(CFace(this, v1, v2, v3, v4));
		}

		iVertexOffset += iPanSegments;
	}

	// Bottom faces
	for (int iPanIndex = 0; iPanIndex < iPanSegments; iPanIndex++)
	{
		int v1 = iVertexOffset + 1 + iPanIndex;
		int v2 = iVertexOffset + 1 + iPanIndex + 1;
		int v3 = iVertexOffset + 0 + iPanSegments;

		if (iPanIndex == iPanSegments - 1)
		{
			v2 -= iPanSegments;
		}

		m_vFaces.append(CFace(this, v1, v2, v3));
	}
}

//-------------------------------------------------------------------------------------------------

void CMesh::createSpherePart(
		int iPanSegments,
		int iTiltSegments,
		double dStartPanDegrees,
		double dEndPanDegrees,
		double dStartTiltDegrees,
		double dEndTiltDegrees
		)
{
	m_vVertices.clear();
	m_vFaces.clear();

	// Le nombre de segments en gisement doit être paire
	if ((iPanSegments & 1) == 1)
	{
		iPanSegments++;
	}

	// Le nombre de segments en site doit être paire
	if ((iTiltSegments & 1) == 1)
	{
		iTiltSegments++;
	}

	// L'angle gisement de début doit être inférieur à l'angle de fin
	if (dStartPanDegrees > dEndPanDegrees)
	{
		double dTemp = dStartPanDegrees;
		dStartPanDegrees = dEndPanDegrees;
		dEndPanDegrees = dTemp;
	}

	// L'angle site de début doit être inférieur à l'angle de fin
	if (dStartTiltDegrees > dEndTiltDegrees)
	{
		double dTemp = dStartTiltDegrees;
		dStartTiltDegrees = dEndTiltDegrees;
		dEndTiltDegrees = dTemp;
	}

	// Calcul de la course des angles gisement et site
	double dPanSpanDegrees = dEndPanDegrees - dStartPanDegrees;
	double dTiltSpanDegrees = dEndTiltDegrees - dStartTiltDegrees;

	// Sommets
	for (int iTiltIndex = 0; iTiltIndex < (iTiltSegments + 1); iTiltIndex++)
	{
		double dCurrentTiltNormalized = (double) iTiltIndex / (double) iTiltSegments;
		double dCurrentTilt = dStartTiltDegrees + ((dCurrentTiltNormalized) * dTiltSpanDegrees);

		// Création de la matrice de rotation en site
		CMatrix4 mRotationTilt = CMatrix4().MakeRotation(CVector3(Math::Angles::toRad(dCurrentTilt), 0.0, 0.0));

		for (int iPanIndex = 0; iPanIndex < (iPanSegments + 1); iPanIndex++)
		{
			double dCurrentPanNormalized = (double) iPanIndex / (double) iPanSegments;
			double dCurrentPan = dStartPanDegrees + ((dCurrentPanNormalized) * dPanSpanDegrees);

			// Création de la matrice de rotation en gisement
			CMatrix4 mRotationPan = CMatrix4().MakeRotation(CVector3(0.0, Math::Angles::toRad(dCurrentPan), 0.0));

			// Création du vecteur de position du sommet
			CVector3 vPosition = mRotationPan * (mRotationTilt * CVector3(0.0, 1.0, 0.0));

			// Ajout du sommet
			m_vVertices.append(CVertex(
								   vPosition,
								   CVector2(dCurrentPanNormalized, 1.0 - dCurrentTiltNormalized))
							   );
		}
	}

	int iVertexOffset = 0;

	// Facettes
	for (int iTiltIndex = 0; iTiltIndex < iTiltSegments; iTiltIndex++)
	{
		for (int iPanIndex = 0; iPanIndex < iPanSegments; iPanIndex++)
		{
			int v1 = iVertexOffset + 0 + iPanIndex;
			int v2 = iVertexOffset + 1 + iPanIndex;
			int v3 = iVertexOffset + 1 + iPanIndex + (iPanSegments + 1);
			int v4 = iVertexOffset + 0 + iPanIndex + (iPanSegments + 1);

			m_vFaces.append(CFace(this, v1, v2, v3, v4));
		}

		iVertexOffset += (iPanSegments + 1);
	}
}

//-------------------------------------------------------------------------------------------------

void CMesh::createAdaptiveTriPatch(Math::CVector3 vCenter, int iNumIterations)
{
	m_vVertices.clear();
	m_vFaces.clear();

	// Create initial vertices and faces
	m_vVertices.append(CVertex(CVector3( 0.0, 0.0,  0.0), CVector2( 0.5,  0.5)));
	m_vVertices.append(CVertex(CVector3(-1.0, 0.0,  0.0), CVector2( 0.0,  0.0)));
	m_vVertices.append(CVertex(CVector3( 1.0, 0.0,  0.0), CVector2( 1.0,  0.0)));
	m_vVertices.append(CVertex(CVector3( 1.0, 0.0,  1.0), CVector2( 1.0,  1.0)));
	m_vVertices.append(CVertex(CVector3(-1.0, 0.0,  1.0), CVector2( 0.0,  1.0)));

	// m_vFaces.append(CFace(1, 2, 0));
	m_vFaces.append(CFace(this, 2, 3, 0));
	m_vFaces.append(CFace(this, 3, 4, 0));
	m_vFaces.append(CFace(this, 4, 1, 0));

	double dDistance = 1.0;

	for (int iIteration = 0; iIteration < iNumIterations; iIteration++)
	{
		QVector<CEdge> vEdges;

		for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
		{
			// Get vertex indices
			int iV1 = m_vFaces[iFaceIndex].getIndices()[0];
			int iV2 = m_vFaces[iFaceIndex].getIndices()[1];
			int iV3 = m_vFaces[iFaceIndex].getIndices()[2];

			// Get vertex positions
			CVector3 pV1 = m_vVertices[iV1].position();
			CVector3 pV2 = m_vVertices[iV2].position();
			CVector3 pV3 = m_vVertices[iV3].position();

			// Get face centroid
			CVector3 vCenter = (pV1 + pV2 + pV3) / 3.0;

			// Subdivide the face if centroid is less than current distance
			if (vCenter.getMagnitude() < dDistance)
			{
				// Get edge lengths
				double dEdgeAB = (pV2 - pV1).getMagnitude();
				double dEdgeBC = (pV3 - pV2).getMagnitude();
				double dEdgeCA = (pV1 - pV3).getMagnitude();

				if (dEdgeAB >= dEdgeBC && dEdgeAB >= dEdgeCA)
				{
					vEdges.append(CEdge(iV1, iV2));
				}
				else
					if (dEdgeBC >= dEdgeAB && dEdgeBC >= dEdgeCA)
					{
						vEdges.append(CEdge(iV2, iV3));
					}
					else
					{
						vEdges.append(CEdge(iV3, iV1));
					}
			}
		}

		QVector<CFace> vNewFaces;

		for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
		{
			// Get vertex indices
			int iV1 = m_vFaces[iFaceIndex].getIndices()[0];
			int iV2 = m_vFaces[iFaceIndex].getIndices()[1];
			int iV3 = m_vFaces[iFaceIndex].getIndices()[2];

			// Get vertex positions
			CVector3 pV1 = m_vVertices[iV1].position();
			CVector3 pV2 = m_vVertices[iV2].position();
			CVector3 pV3 = m_vVertices[iV3].position();

			int iVN = m_vVertices.count();

			if (vEdges.contains(CEdge(iV1, iV2)))
			{
				m_vVertices.append(CVertex((pV1 + pV2) / 2.0, CVector2( 0.0,  0.0)));

				vNewFaces.append(CFace(this, iV1, iVN, iV3));
				vNewFaces.append(CFace(this, iVN, iV2, iV3));

				m_vFaces.remove(iFaceIndex); iFaceIndex--;
			}
			else
				if (vEdges.contains(CEdge(iV2, iV3)))
				{
					m_vVertices.append(CVertex((pV2 + pV3) / 2.0, CVector2( 0.0,  0.0)));

					vNewFaces.append(CFace(this, iV1, iV2, iVN));
					vNewFaces.append(CFace(this, iVN, iV3, iV1));

					m_vFaces.remove(iFaceIndex); iFaceIndex--;
				}
				else
					if (vEdges.contains(CEdge(iV3, iV1)))
					{
						m_vVertices.append(CVertex((pV3 + pV1) / 2.0, CVector2( 0.0,  0.0)));

						vNewFaces.append(CFace(this, iV1, iV2, iVN));
						vNewFaces.append(CFace(this, iV2, iV3, iVN));

						m_vFaces.remove(iFaceIndex); iFaceIndex--;
					}
		}

		foreach (const CFace& face, vNewFaces)
		{
			m_vFaces.append(face);
		}

		dDistance *= 0.8;
	}
}

//-------------------------------------------------------------------------------------------------

void CMesh::createAdaptiveQuadPatch(Math::CVector3 vCenter, int iNumIterations)
{
	m_bAllQuads = true;

	m_vVertices.clear();
	m_vFaces.clear();

	// Create initial vertices and faces
	m_vVertices.append(CVertex(CVector3(-1.0, 0.0, -0.05), CVector2( 0.0,  0.0)));
	m_vVertices.append(CVertex(CVector3( 1.0, 0.0, -0.05), CVector2( 1.0,  0.0)));
	m_vVertices.append(CVertex(CVector3( 1.0, 0.0,  1.00), CVector2( 1.0,  1.0)));
	m_vVertices.append(CVertex(CVector3(-1.0, 0.0,  1.00), CVector2( 0.0,  1.0)));

	m_vFaces.append(CFace(this, 0, 1, 2, 3));

	double dDistance = 1.0;

	for (int iIteration = 0; iIteration < iNumIterations; iIteration++)
	{
		QVector<CEdge> vEdges;

		for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
		{
			// Get vertex indices
			int iVA = m_vFaces[iFaceIndex].getIndices()[0];
			int iVB = m_vFaces[iFaceIndex].getIndices()[1];
			int iVC = m_vFaces[iFaceIndex].getIndices()[2];
			int iVD = m_vFaces[iFaceIndex].getIndices()[3];

			// Get vertex positions
			CVector3 pVA = m_vVertices[iVA].position();
			CVector3 pVB = m_vVertices[iVB].position();
			CVector3 pVC = m_vVertices[iVC].position();
			CVector3 pVD = m_vVertices[iVD].position();

			// Get face centroid
			CVector3 vCenter = (pVA + pVB + pVC + pVD) / 4.0;

			// Subdivide the face if centroid is less than current distance
			if (vCenter.getMagnitude() < dDistance)
			{
				if (!(vEdges.contains(CEdge(iVA, iVB)) || vEdges.contains(CEdge(iVB, iVA))))
				{
					vEdges.append(CEdge(iVA, iVB, m_vVertices.count()));
					m_vVertices.append(CVertex((pVA + pVB) / 2.0, CVector2( 0.0,  0.0)));
				}

				if (!(vEdges.contains(CEdge(iVB, iVC)) || vEdges.contains(CEdge(iVC, iVB))))
				{
					vEdges.append(CEdge(iVB, iVC, m_vVertices.count()));
					m_vVertices.append(CVertex((pVB + pVC) / 2.0, CVector2( 0.0,  0.0)));
				}

				if (!(vEdges.contains(CEdge(iVC, iVD)) || vEdges.contains(CEdge(iVD, iVC))))
				{
					vEdges.append(CEdge(iVC, iVD, m_vVertices.count()));
					m_vVertices.append(CVertex((pVC + pVD) / 2.0, CVector2( 0.0,  0.0)));
				}

				if (!(vEdges.contains(CEdge(iVD, iVA)) || vEdges.contains(CEdge(iVA, iVD))))
				{
					vEdges.append(CEdge(iVD, iVA, m_vVertices.count()));
					m_vVertices.append(CVertex((pVD + pVA) / 2.0, CVector2( 0.0,  0.0)));
				}
			}
		}

		QVector<CFace> vNewFaces;

		for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
		{
			// Get vertex indices
			int iVA = m_vFaces[iFaceIndex].getIndices()[0];
			int iVB = m_vFaces[iFaceIndex].getIndices()[1];
			int iVC = m_vFaces[iFaceIndex].getIndices()[2];
			int iVD = m_vFaces[iFaceIndex].getIndices()[3];

			if (
					vEdges.contains(CEdge(iVA, iVB)) && vEdges.contains(CEdge(iVB, iVC)) &&
					vEdges.contains(CEdge(iVC, iVD)) && vEdges.contains(CEdge(iVD, iVA))
					)
			{
				int iEAB = vEdges.indexOf(CEdge(iVA, iVB));
				int iEBC = vEdges.indexOf(CEdge(iVB, iVC));
				int iECD = vEdges.indexOf(CEdge(iVC, iVD));
				int iEDA = vEdges.indexOf(CEdge(iVD, iVA));

				// Get vertex positions
				CVector3 pVA = m_vVertices[iVA].position();
				CVector3 pVB = m_vVertices[iVB].position();
				CVector3 pVC = m_vVertices[iVC].position();
				CVector3 pVD = m_vVertices[iVD].position();

				int iVN = m_vVertices.count();

				m_vVertices.append(CVertex((pVA + pVB + pVC + pVD) / 4.0, CVector2( 0.0,  0.0)));

				// Subdivide
				vNewFaces.append(CFace(this, iVA, vEdges[iEAB].m_iNewVertex, iVN, vEdges[iEDA].m_iNewVertex));
				vNewFaces.append(CFace(this, vEdges[iEAB].m_iNewVertex, iVB, vEdges[iEBC].m_iNewVertex, iVN));
				vNewFaces.append(CFace(this, iVN, vEdges[iEBC].m_iNewVertex, iVC, vEdges[iECD].m_iNewVertex));
				vNewFaces.append(CFace(this, vEdges[iEDA].m_iNewVertex, iVN, vEdges[iECD].m_iNewVertex, iVD));

				m_vFaces.remove(iFaceIndex); iFaceIndex--;
			}
		}

		foreach (const CFace& face, vNewFaces)
		{
			m_vFaces.append(face);
		}

		dDistance *= 0.65;
	}
}

//-------------------------------------------------------------------------------------------------

void CMesh::createQuadPatch(int iNumVerts, int iNumSkirtVerts, bool bDoubleSided)
{
	m_bAllQuads = true;

	m_vVertices.clear();
	m_vFaces.clear();

	int iNumSkirtVertsX2 = iNumSkirtVerts * 2;
	double dSpan = (1.0 / (double) iNumVerts) * ((double) iNumVerts + (double) iNumSkirtVertsX2);
	double dHalf = dSpan * 0.5;
	int iTotalVerts = iNumVerts + iNumSkirtVertsX2;

	for (int z = 0; z < iTotalVerts; z++)
	{
		for (int x = 0; x < iTotalVerts; x++)
		{
			double dx = (((double) x / (double) (iTotalVerts - 1)) * dSpan) - dHalf;
			double dz = (((double) z / (double) (iTotalVerts - 1)) * dSpan) - dHalf;

			double u = (double) x / (double) (iTotalVerts - 1);
			double v = (double) z / (double) (iTotalVerts - 1);

			m_vVertices.append(CVertex(CVector3(dx, 0.0, dz), CVector2((double) u, (double) v)));
		}
	}

	for (int z = 0; z < iTotalVerts - 1; z++)
	{
		for (int x = 0; x < iTotalVerts - 1; x++)
		{
			int v1 = ((z + 0) * iTotalVerts) + x + 0;
			int v2 = ((z + 0) * iTotalVerts) + x + 1;
			int v3 = ((z + 1) * iTotalVerts) + x + 1;
			int v4 = ((z + 1) * iTotalVerts) + x + 0;

			m_vFaces.append(CFace(this, v1, v2, v3, v4));
		}
	}

	if (bDoubleSided)
	{
		int iVertexOffset = m_vVertices.count();

		for (int z = 0; z < iTotalVerts; z++)
		{
			for (int x = 0; x < iTotalVerts; x++)
			{
				double dx = (((double) x / (double) (iTotalVerts - 1)) * dSpan) - dHalf;
				double dz = (((double) z / (double) (iTotalVerts - 1)) * dSpan) - dHalf;

				double u = (double) x / (double) (iTotalVerts - 1);
				double v = (double) z / (double) (iTotalVerts - 1);

				m_vVertices.append(CVertex(CVector3(dx, 0.0, dz), CVector2((double) u, (double) v)));
			}
		}

		for (int z = 0; z < iTotalVerts - 1; z++)
		{
			for (int x = 0; x < iTotalVerts - 1; x++)
			{
				int v1 = iVertexOffset + ((z + 0) * iTotalVerts) + x + 0;
				int v2 = iVertexOffset + ((z + 0) * iTotalVerts) + x + 1;
				int v3 = iVertexOffset + ((z + 1) * iTotalVerts) + x + 1;
				int v4 = iVertexOffset + ((z + 1) * iTotalVerts) + x + 0;

				m_vFaces.append(CFace(this, v4, v3, v2, v1));
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CMesh::createCircularQuadPatch(Math::CVector3 vCenter, int iNumVerts)
{
	m_bAllQuads = true;

	m_vVertices.clear();
	m_vFaces.clear();

	for (int z = 0; z < iNumVerts; z++)
	{
		for (int x = 0; x < iNumVerts; x++)
		{
			double dx = (double) x / (double) (iNumVerts - 1);
			double dz = (double) z / (double) (iNumVerts - 1);

			double ax = (-0.5 + pow(dx, 3.0)) * 0.05;
			// double az = pow(dz, 3.0);
			double az = (-0.5 + pow(dz, 3.0)) * 0.05;

			/*
			QMatrix4x4 mMatrix;
			mMatrix.setToIdentity();
			mMatrix.rotate((-0.5 + dx) * -45.0, QVector3D(0, 1, 0));
			*/

			QVector3D vertex(ax, 0.0, az);
			// vertex = vertex * mMatrix;

			m_vVertices.append(CVertex(CVector3(vertex.x(), vertex.y(), vertex.z()), CVector2(0.0, 0.0)));
		}
	}

	for (int z = 0; z < iNumVerts - 1; z++)
	{
		for (int x = 0; x < iNumVerts - 1; x++)
		{
			int v1 = ((z + 0) * iNumVerts) + x + 0;
			int v2 = ((z + 0) * iNumVerts) + x + 1;
			int v3 = ((z + 1) * iNumVerts) + x + 1;
			int v4 = ((z + 1) * iNumVerts) + x + 0;

			m_vFaces.append(CFace(this, v1, v2, v3, v4));
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CMesh::createSurfaceFromFFD(const QVector<Math::CVector3>& vFFDFrom, const QVector<Math::CVector3>& vFFDTo, int iNumVerts)
{
	if (vFFDFrom.count() == vFFDTo.count())
	{
		if (iNumVerts < 2) iNumVerts = 2;

		createQuadPatch(iNumVerts, 0, true);

		// Move vertices
		for (int iVertexIndex = 0; iVertexIndex < m_vVertices.count(); iVertexIndex++)
		{
			CVector3 vNewPosition = m_vVertices[iVertexIndex].position();

			for (int iFFDIndex = 0; iFFDIndex < vFFDFrom.count(); iFFDIndex++)
			{
				CVector3 vOriginalPosition = vFFDFrom[iFFDIndex];
				CVector3 vTargetPosition = vFFDTo[iFFDIndex];
				CVector3 vMovement = vTargetPosition - vOriginalPosition;

				double dDistance = (m_vVertices[iVertexIndex].position() - vOriginalPosition).getMagnitude();
				double dWeight = pow(1.0 - Angles::clipDouble(dDistance, 0.0, 1.0), 2.0);

				vNewPosition += vMovement * dWeight;
			}

			m_vVertices[iVertexIndex].position() = vNewPosition;
		}
	}

	// Mise à jour des dépendances de géométrie
	updateGeometry();
}

//-------------------------------------------------------------------------------------------------

void CMesh::subdivideQuads(int iNumIterations)
{
	for (int iIteration = 0; iIteration < iNumIterations; iIteration++)
	{
		QVector<CEdge> vEdges;

		for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
		{
			// Get vertex indices
			int iVA = m_vFaces[iFaceIndex].getIndices()[0];
			int iVB = m_vFaces[iFaceIndex].getIndices()[1];
			int iVC = m_vFaces[iFaceIndex].getIndices()[2];
			int iVD = m_vFaces[iFaceIndex].getIndices()[3];

			// Get vertex positions
			CVector3 pVA = m_vVertices[iVA].position();
			CVector3 pVB = m_vVertices[iVB].position();
			CVector3 pVC = m_vVertices[iVC].position();
			CVector3 pVD = m_vVertices[iVD].position();

			// Get face centroid
            // CVector3 vCenter = (pVA + pVB + pVC + pVD) / 4.0;

			// Subdivide the face
			if (!(vEdges.contains(CEdge(iVA, iVB)) || vEdges.contains(CEdge(iVB, iVA))))
			{
				vEdges.append(CEdge(iVA, iVB, m_vVertices.count()));
				m_vVertices.append(CVertex((pVA + pVB) / 2.0, CVector2( 0.0,  0.0)));
			}

			if (!(vEdges.contains(CEdge(iVB, iVC)) || vEdges.contains(CEdge(iVC, iVB))))
			{
				vEdges.append(CEdge(iVB, iVC, m_vVertices.count()));
				m_vVertices.append(CVertex((pVB + pVC) / 2.0, CVector2( 0.0,  0.0)));
			}

			if (!(vEdges.contains(CEdge(iVC, iVD)) || vEdges.contains(CEdge(iVD, iVC))))
			{
				vEdges.append(CEdge(iVC, iVD, m_vVertices.count()));
				m_vVertices.append(CVertex((pVC + pVD) / 2.0, CVector2( 0.0,  0.0)));
			}

			if (!(vEdges.contains(CEdge(iVD, iVA)) || vEdges.contains(CEdge(iVA, iVD))))
			{
				vEdges.append(CEdge(iVD, iVA, m_vVertices.count()));
				m_vVertices.append(CVertex((pVD + pVA) / 2.0, CVector2( 0.0,  0.0)));
			}
		}

		QVector<CFace> vNewFaces;

		for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
		{
			// Get vertex indices
			int iVA = m_vFaces[iFaceIndex].getIndices()[0];
			int iVB = m_vFaces[iFaceIndex].getIndices()[1];
			int iVC = m_vFaces[iFaceIndex].getIndices()[2];
			int iVD = m_vFaces[iFaceIndex].getIndices()[3];

			if (
					vEdges.contains(CEdge(iVA, iVB)) && vEdges.contains(CEdge(iVB, iVC)) &&
					vEdges.contains(CEdge(iVC, iVD)) && vEdges.contains(CEdge(iVD, iVA))
					)
			{
				int iEAB = vEdges.indexOf(CEdge(iVA, iVB));
				int iEBC = vEdges.indexOf(CEdge(iVB, iVC));
				int iECD = vEdges.indexOf(CEdge(iVC, iVD));
				int iEDA = vEdges.indexOf(CEdge(iVD, iVA));

				// Get vertex positions
				CVector3 pVA = m_vVertices[iVA].position();
				CVector3 pVB = m_vVertices[iVB].position();
				CVector3 pVC = m_vVertices[iVC].position();
				CVector3 pVD = m_vVertices[iVD].position();

				int iVN = m_vVertices.count();

				m_vVertices.append(CVertex((pVA + pVB + pVC + pVD) / 4.0, CVector2( 0.0,  0.0)));

				// Subdivide
				vNewFaces.append(CFace(this, iVA, vEdges[iEAB].m_iNewVertex, iVN, vEdges[iEDA].m_iNewVertex));
				vNewFaces.append(CFace(this, vEdges[iEAB].m_iNewVertex, iVB, vEdges[iEBC].m_iNewVertex, iVN));
				vNewFaces.append(CFace(this, iVN, vEdges[iEBC].m_iNewVertex, iVC, vEdges[iECD].m_iNewVertex));
				vNewFaces.append(CFace(this, vEdges[iEDA].m_iNewVertex, iVN, vEdges[iECD].m_iNewVertex, iVD));

				m_vFaces.remove(iFaceIndex); iFaceIndex--;
			}
		}

		foreach (const CFace& face, vNewFaces)
		{
			m_vFaces.append(face);
		}
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

			pMesh->createSpherePart(
						iSegmentsPerPanPart,
						iSegmentsPerTiltPart,
						((double) iPanIndex * dPanPartSpanDegrees),
						((double) iPanIndex * dPanPartSpanDegrees) + dPanPartSpanDegrees,
						((double) iTiltIndex * dTiltPartSpanDegrees),
						((double) iTiltIndex * dTiltPartSpanDegrees) + dTiltPartSpanDegrees
						);

			pMesh->setParent(pComponent);
			pMesh->updateGeometry();
		}
	}

	return pComponent;
}

//-------------------------------------------------------------------------------------------------

void CMesh::merge(const CMesh& other, bool bUpdateGeometry)
{
	int iVertexOffset = m_vVertices.count();

	foreach (const CVertex& vert, other.m_vVertices)
	{
		m_vVertices.append(vert);
	}

	foreach (const CFace& face, other.m_vFaces)
	{
		QVector<int> vNewIndices = face.getIndices();

		for (int iIndex = 0; iIndex < vNewIndices.count(); iIndex++)
		{
			vNewIndices[iIndex] += iVertexOffset;
		}

		m_vFaces.append(CFace(this, vNewIndices));
	}

	if (bUpdateGeometry)
	{
		updateGeometry(true);
	}
}

//-------------------------------------------------------------------------------------------------

void CMesh::transformVertices(const CMatrix4& matrix)
{
	CPhysicalComponent::transformVertices(matrix);

	for (int iIndex = 0; iIndex < m_vVertices.count(); iIndex++)
	{
		m_vVertices[iIndex].position() = matrix * m_vVertices[iIndex].position();
	}
}

//-------------------------------------------------------------------------------------------------

void CMesh::translateUVs(CVector2 vTranslate)
{
	for (int iIndex = 0; iIndex < m_vVertices.count(); iIndex++)
	{
		m_vVertices[iIndex].texCoord() = m_vVertices[iIndex].texCoord() + vTranslate;
	}
}

//-------------------------------------------------------------------------------------------------

void CMesh::scaleUVs(CVector2 vScale)
{
	for (int iIndex = 0; iIndex < m_vVertices.count(); iIndex++)
	{
		m_vVertices[iIndex].texCoord() = m_vVertices[iIndex].texCoord() * vScale;
	}
}

//-------------------------------------------------------------------------------------------------

RayTracingResult CMesh::intersect(CRay3 ray) const
{
	RayTracingResult dReturnResult(Q3D_INFINITY, this);

	// Transformation du rayon dans le repère local
	// CRay3 rLocalray = ray;
	// rLocalray.vOrigin -= getWorldPosition();
	CRay3 rLocalray = getWorldTransformInverse() * ray;

	if (m_bBounds.intersect(rLocalray).m_dDistance < Q3D_INFINITY)
	{
		if (m_bUseSpacePartitionning)
		{
			return intersectRecurse(m_mpPartitions, rLocalray);
		}
		else
		{
			for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
			{
				if (m_vFaces[iFaceIndex].getIndices().count() > 2)
				{
					for (int iVertIndex = 2; iVertIndex < m_vFaces[iFaceIndex].getIndices().count(); iVertIndex++)
					{
						int i1 = m_vFaces[iFaceIndex].getIndices()[0];
						int i2 = m_vFaces[iFaceIndex].getIndices()[iVertIndex - 1];
						int i3 = m_vFaces[iFaceIndex].getIndices()[iVertIndex];

						CVector3 v1 = m_vVertices[i1].position();
						CVector3 v2 = m_vVertices[i2].position();
						CVector3 v3 = m_vVertices[i3].position();

						RayTracingResult dNewResult = CFace::intersectTriangle(
									rLocalray,
									v1, v2, v3
									);

						if (dNewResult.m_dDistance < dReturnResult.m_dDistance)
						{
							dReturnResult.m_dDistance = dNewResult.m_dDistance;
							dReturnResult.m_vNormal = dNewResult.m_vNormal;
						}
					}
				}
			}
		}
	}

	return dReturnResult;
}

//-------------------------------------------------------------------------------------------------

RayTracingResult CMesh::intersectRecurse(const CMeshPartition& mpPartition, CRay3 ray) const
{
	RayTracingResult dReturnResult(Q3D_INFINITY);
	double dDistanceToBox = mpPartition.getBounds().intersect(ray).m_dDistance;

	/*
	if (dDistanceToBox < dReturnDistance)
	{
		dReturnDistance = dDistanceToBox;
	}
	*/

	if (dDistanceToBox < Q3D_INFINITY)
	{
		if (mpPartition.getChildren().count() > 0)
		{
			foreach (const CMeshPartition& mpChild, mpPartition.getChildren())
			{
				RayTracingResult dNewResult = intersectRecurse(mpChild, ray);

				if (dNewResult.m_dDistance < dReturnResult.m_dDistance)
				{
					dReturnResult = dNewResult;
				}
			}
		}
		else
		{
			for (int iPartFaceIndex = 0; iPartFaceIndex < mpPartition.getFaceIndices().count(); iPartFaceIndex++)
			{
				int iFaceIndex = mpPartition.getFaceIndices()[iPartFaceIndex];

				if (m_vFaces[iFaceIndex].getIndices().count() > 2)
				{
					for (int iVertIndex = 2; iVertIndex < m_vFaces[iFaceIndex].getIndices().count(); iVertIndex++)
					{
						int i1 = m_vFaces[iFaceIndex].getIndices()[0];
						int i2 = m_vFaces[iFaceIndex].getIndices()[iVertIndex - 1];
						int i3 = m_vFaces[iFaceIndex].getIndices()[iVertIndex];

						CVector3 v1 = m_vVertices[i1].position();
						CVector3 v2 = m_vVertices[i2].position();
						CVector3 v3 = m_vVertices[i3].position();

						RayTracingResult dNewResult = CFace::intersectTriangle(
									ray,
									v1, v2, v3
									);

						if (dNewResult.m_dDistance < dReturnResult.m_dDistance)
						{
							dReturnResult = dNewResult;
						}
					}
				}
			}
		}
	}

	return dReturnResult;
}

//-------------------------------------------------------------------------------------------------

void CMesh::paint(CRenderContext* pContext)
{
	CVector3 vPosition = pContext->internalCameraMatrix() * getWorldBounds().center();
	double dRadius = getWorldBounds().radius();

	if (
			pContext->camera()->contains(vPosition, dRadius) &&
			vPosition.getMagnitude() < m_dMaxDistance &&
			m_vGLMeshData.count() > 0
			)
	{
		QMutexLocker locker(&m_mMutex);

		// Create and set transform matrices
		CVector3 WorldPosition = getWorldPosition() - pContext->scene()->getWorldOrigin();
		CVector3 WorldRotation = getWorldRotation();
		CVector3 WorldScale = getWorldScale();

		QMatrix4x4 mModelAbsolute;
		mModelAbsolute.setToIdentity();
		mModelAbsolute.translate(WorldPosition.X, WorldPosition.Y, WorldPosition.Z);
		mModelAbsolute.rotate(Math::Angles::toDeg(WorldRotation.Y), QVector3D(0, 1, 0));
		mModelAbsolute.rotate(Math::Angles::toDeg(WorldRotation.X), QVector3D(1, 0, 0));
		mModelAbsolute.rotate(Math::Angles::toDeg(WorldRotation.Z), QVector3D(0, 0, 1));
		mModelAbsolute.scale(WorldScale.X, WorldScale.Y, WorldScale.Z);

		if (m_vMaterials.count() == m_vGLMeshData.count())
		{
			for (int iMaterialIndex = 0; iMaterialIndex < m_vMaterials.count(); iMaterialIndex++)
			{
				CGLMeshData* pData = m_vGLMeshData[iMaterialIndex];

				// If at least one point to render
				if (pData->m_iNumRenderPoints > 0 && pData->m_iNumRenderIndices > 0 && pData->m_iVBO[0] > 0 && pData->m_iVBO[1] > 0)
				{
					pContext->m_iNumMeshesDrawn++;

					// Get a program from object material
					QGLShaderProgram* pProgram = m_vMaterials[iMaterialIndex]->activate(pContext);

					// If program ok...
					if (pProgram != NULL)
					{
						if (m_iCurrentVBO != pData->m_iVBO[0])
						{
							m_iCurrentVBO = pData->m_iVBO[0];

							pProgram->setUniformValue("u_camera_projection_matrix", pContext->cameraProjectionMatrix());
							pProgram->setUniformValue("u_camera_matrix", pContext->cameraMatrix());
							pProgram->setUniformValue("u_shadow_projection_matrix", pContext->shadowProjectionMatrix());
							pProgram->setUniformValue("u_shadow_matrix", pContext->shadowMatrix());
							pProgram->setUniformValue("u_model_matrix", mModelAbsolute);

							GL_glBindBuffer(GL_ARRAY_BUFFER, pData->m_iVBO[0]);
							GL_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pData->m_iVBO[1]);

							if (m_bNeedTransferBuffers)
							{
								// Transfer vertex data to VBO 0
								GL_glBufferData(GL_ARRAY_BUFFER, pData->m_iNumRenderPoints * sizeof(CVertex), pData->m_vRenderPoints, GL_STATIC_DRAW);

								// Transfer index data to VBO 1
								GL_glBufferData(GL_ELEMENT_ARRAY_BUFFER, pData->m_iNumRenderIndices * sizeof(GLuint), pData->m_vRenderIndices, GL_STATIC_DRAW);
							}

							// Tell OpenGL how to locate vertex position data
							int vertexLocation = pProgram->attributeLocation("a_position");
							pProgram->enableAttributeArray(vertexLocation);
							GL_glVertexAttribPointer(
										vertexLocation, 3, GL_DOUBLE, GL_FALSE, sizeof(CVertex), (const void*) CVertex::positionOffset()
										);

							// Tell OpenGL how to locate vertex texture coordinate data
							int texcoordLocation = pProgram->attributeLocation("a_texcoord");
							pProgram->enableAttributeArray(texcoordLocation);
							GL_glVertexAttribPointer(
										texcoordLocation, 2, GL_DOUBLE, GL_FALSE, sizeof(CVertex), (const void*) CVertex::texCoordOffset()
										);

							// Tell OpenGL how to locate vertex diffuse texture weight data
							int diffTexWeight_0_1_2Location = pProgram->attributeLocation("a_difftext_weight_0_1_2");
							pProgram->enableAttributeArray(diffTexWeight_0_1_2Location);
							GL_glVertexAttribPointer(
										diffTexWeight_0_1_2Location, 3, GL_DOUBLE, GL_FALSE, sizeof(CVertex), (const void*) CVertex::diffTexWeight_0_1_2Offset()
										);

							// Tell OpenGL how to locate vertex diffuse texture weight data
							int diffTexWeight_3_4_5Location = pProgram->attributeLocation("a_difftext_weight_3_4_5");
							pProgram->enableAttributeArray(diffTexWeight_3_4_5Location);
							GL_glVertexAttribPointer(
										diffTexWeight_3_4_5Location, 3, GL_DOUBLE, GL_FALSE, sizeof(CVertex), (const void*) CVertex::diffTexWeight_3_4_5Offset()
										);

							// Tell OpenGL how to locate vertex diffuse texture weight data
							int diffTexWeight_6_7_8Location = pProgram->attributeLocation("a_difftext_weight_6_7_8");
							pProgram->enableAttributeArray(diffTexWeight_6_7_8Location);
							GL_glVertexAttribPointer(
										diffTexWeight_6_7_8Location, 3, GL_DOUBLE, GL_FALSE, sizeof(CVertex), (const void*) CVertex::diffTexWeight_6_7_8Offset()
										);

							// Tell OpenGL how to locate vertex normal data
							int normalLocation = pProgram->attributeLocation("a_normal");
							pProgram->enableAttributeArray(normalLocation);
							GL_glVertexAttribPointer(
										normalLocation, 3, GL_DOUBLE, GL_FALSE, sizeof(CVertex), (const void*) CVertex::normalOffset()
										);

							// Tell OpenGL how to locate vertex tangent data
							int tangentLocation = pProgram->attributeLocation("a_tangent");
							pProgram->enableAttributeArray(tangentLocation);
							GL_glVertexAttribPointer(
										tangentLocation, 3, GL_DOUBLE, GL_FALSE, sizeof(CVertex), (const void*) CVertex::tangentOffset()
										);

							// Tell OpenGL how to locate altitude data
							int altitudeLocation = pProgram->attributeLocation("a_altitude");
							pProgram->enableAttributeArray(altitudeLocation);
							GL_glVertexAttribPointer(
										altitudeLocation, 1, GL_DOUBLE, GL_FALSE, sizeof(CVertex), (const void*) CVertex::altitudeOffset()
										);
						}

						if (m_bAllQuads)
						{
							if (pContext->scene()->getDebugMode())
							{
								try
								{
									glDrawElements(GL_LINES, pData->m_iNumRenderIndices, GL_UNSIGNED_INT, 0);
								}
								catch (...)
								{
									LOG_ERROR(QString("CMesh::paint() : Exception while rendering %1").arg(m_sName));
								}
							}
							else
							{
								try
								{
									// Draw quads
									glDrawElements(GL_QUADS, pData->m_iNumRenderIndices, GL_UNSIGNED_INT, 0);
								}
								catch (...)
								{
									LOG_ERROR(QString("CMesh::paint() : Exception while rendering %1").arg(m_sName));
								}
							}

							pContext->m_iNumPolysDrawn += (pData->m_iNumRenderIndices / 4);
						}
						else
						{
							if (pContext->scene()->getDebugMode())
							{
								try
								{
									glDrawElements(GL_LINES, pData->m_iNumRenderIndices, GL_UNSIGNED_INT, 0);
								}
								catch (...)
								{
									LOG_ERROR(QString("CMesh::paint() : Exception while rendering %1").arg(m_sName));
								}
							}
							else
							{
								try
								{
									// Draw triangles
									glDrawElements(GL_TRIANGLES, pData->m_iNumRenderIndices, GL_UNSIGNED_INT, 0);
								}
								catch (...)
								{
									LOG_ERROR(QString("CMesh::paint() : Exception while rendering %1").arg(m_sName));
								}
							}

							pContext->m_iNumPolysDrawn += (pData->m_iNumRenderIndices / 3);
						}
					}
				}
			}

			m_bNeedTransferBuffers = false;
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CMesh::dump(QTextStream& stream, int iIdent)
{
	dumpIdent(stream, iIdent, QString("[CMesh]"));
	dumpIdent(stream, iIdent, QString("Num vertices : %1").arg(m_vVertices.count()));
	dumpIdent(stream, iIdent, QString("Num faces : %1").arg(m_vFaces.count()));
	dumpIdent(stream, iIdent, QString("Max distance : %1").arg(m_dMaxDistance));
	dumpIdent(stream, iIdent, QString("All quads : %1").arg(m_bAllQuads));
	dumpIdent(stream, iIdent, QString("Use partitioning : %1").arg(m_bUseSpacePartitionning));
	dumpIdent(stream, iIdent, QString("Bounds min : %1").arg(m_bBounds.minimum().toString()));
	dumpIdent(stream, iIdent, QString("Bounds max : %1").arg(m_bBounds.maximum().toString()));

	/*
	dumpIdent(stream, iIdent, QString("Vertices :"));
	dumpOpenBlock(stream, iIdent); iIdent++;
	for (int iIndex = 0; iIndex < m_vVertices.count(); iIndex++)
	{
		dumpIdent(stream, iIdent, QString("Vertex %1 : %2").arg(iIndex).arg(m_vVertices[iIndex].position().toString()));
	}
	iIdent--; dumpCloseBlock(stream, iIdent);
	*/

	CPhysicalComponent::dump(stream, iIdent);
}
