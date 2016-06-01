
// Qt
#include <QTextStream>

// Application
#include "CLogManager.h"
#include "C3DScene.h"
#include "COBJLoader.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

COBJLoader::COBJLoader()
{
}

//-------------------------------------------------------------------------------------------------

void COBJLoader::load(C3DScene* pScene, CMesh* pMesh, QString sText)
{
	LOG_DEBUG(QString("COBJLoader::load() : START : %1").arg(pMesh->getName()));

	QTextStream sInput(&sText, QIODevice::ReadOnly);

	pMesh->getFaces().clear();
	pMesh->getVertices().clear();

	int iSmoothingGroup = 0;
	int iMaterialIndex = 0;

	QVector<CVector2> vTextureVertices;

	while (sInput.atEnd() == false)
	{
		QString sLine = sInput.readLine();

		if (sLine.length() > 0)
		{
			QStringList lWords = sLine.trimmed().split(QChar(' '), QString::SkipEmptyParts);

			if (lWords.count() > 0)
			{
				QString sFirstWord = lWords.at(0);

				if (sFirstWord == "#")
				{
				}
				else if (sFirstWord == "mtllib")
				{
					QString sMaterialFileName = ":/Resources/" + lWords.at(1);

					loadMaterials(pScene, pMesh, pScene->getRessourcesManager()->getObjByFilePathName(sMaterialFileName));
				}
				else if (sFirstWord == "o" && lWords.count() > 1)
				{
					QString sObjectName = lWords.at(1);
				}
				else if (sFirstWord == "usemtl" && lWords.count() > 1)
				{
					QString sMaterialName = lWords.at(1);

					for (int iIndex = 0; iIndex < pMesh->getMaterials().count(); iIndex++)
					{
						if (pMesh->getMaterials()[iIndex]->getName() == sMaterialName)
						{
							iMaterialIndex = iIndex;
						}
					}
				}
				else if (sFirstWord == "v" && lWords.count() > 3)
				{
					double x = lWords.at(1).toDouble();
					double y = lWords.at(3).toDouble();
					double z = lWords.at(2).toDouble();

					pMesh->getVertices().append(CVertex(CVector3(x, y, z), CVector2()));
				}
				else if (sFirstWord == "vn" && lWords.count() > 3)
				{
                    // double x = lWords.at(1).toDouble();
                    // double y = lWords.at(3).toDouble();
                    // double z = lWords.at(2).toDouble();
				}
				else if (sFirstWord == "vt" && lWords.count() > 2)
				{
					double u = lWords.at(2).toDouble();
					double v = lWords.at(1).toDouble();

					vTextureVertices.append(CVector2(v, u));
				}
				else if (sFirstWord == "s" && lWords.count() > 1)
				{
					if (lWords.at(1) == "off")
					{
						iSmoothingGroup = 0;
					}
					else
					{
						iSmoothingGroup = lWords.at(1).toInt();
					}
				}
				else if (sFirstWord == "f" && lWords.count() > 3)
				{
					CFace NewFace;

					NewFace.setSmoothingGroup(iSmoothingGroup);
					NewFace.setMaterialIndex(iMaterialIndex);

					for (int Index = 1; Index < lWords.count(); Index++)
					{
						QStringList sFaceIndices = lWords.at(Index).split('/', QString::KeepEmptyParts);

						int iPositionIndex = -1;
						int iTextureIndex = -1;
                        int iNormalIndex = -1;

						if (sFaceIndices.count() > 0)
						{
							iPositionIndex = sFaceIndices.at(0).toInt() - 1;
						}

						if (sFaceIndices.count() > 1)
						{
							iTextureIndex = sFaceIndices.at(1).toInt() - 1;
						}

						if (sFaceIndices.count() > 2)
						{
							iNormalIndex = sFaceIndices.at(2).toInt() - 1;
						}

						if (iPositionIndex > 0)
						{
							NewFace.getIndices().append(iPositionIndex);

							if (iTextureIndex > 0 && iTextureIndex < vTextureVertices.count())
							{
								pMesh->getVertices()[iPositionIndex].texCoord() = vTextureVertices[iTextureIndex];
							}
						}
					}

					pMesh->getFaces().append(NewFace);
				}
			}
		}
	}

	// Séparation des polygones en fonction des groupes de lissage
	pMesh->splitVerticesBySmoothingGroup();

	// Mise à jour de la géométrie du mesh
	pMesh->updateGeometry();

	LOG_DEBUG(QString("COBJLoader::load() : END : %1").arg(pMesh->getName()));
}

//-------------------------------------------------------------------------------------------------

void COBJLoader::loadMaterials(C3DScene* pScene, CMesh* pMesh, QString sText)
{
	LOG_DEBUG(QString("COBJLoader::loadMaterials() : START : %1").arg(pMesh->getName()));

	QTextStream sInput(&sText, QIODevice::ReadOnly);

	CMaterial* pMaterial = NULL;

	pMesh->deleteMaterials();

	while (sInput.atEnd() == false)
	{
		QString sLine = sInput.readLine();

		if (sLine.length() > 0)
		{
			QStringList lWords = sLine.trimmed().split(QChar(' '), QString::SkipEmptyParts);

			if (lWords.count() > 0)
			{
				QString sFirstWord = lWords.at(0);

				if (sFirstWord == "#")
				{
				}
				else if (sFirstWord == "newmtl" && lWords.count() > 1)
				{
					pMaterial = new CMaterial(pScene, lWords.at(1));

					pMesh->getMaterials().append(pScene->getRessourcesManager()->shareMaterial(QSharedPointer<CMaterial>(pMaterial)));
				}
				else if (sFirstWord == "Ka" && lWords.count() > 3)
				{
					// Couleur ambiente

					if (pMaterial != NULL)
					{
						pMaterial->getAmbient().X = lWords.at(1).toDouble();
						pMaterial->getAmbient().Y = lWords.at(2).toDouble();
						pMaterial->getAmbient().Z = lWords.at(3).toDouble();
					}
				}
				else if (sFirstWord == "Kd" && lWords.count() > 3)
				{
					// Couleur diffuse

					if (pMaterial != NULL)
					{
						pMaterial->getDiffuse().X = lWords.at(1).toDouble();
						pMaterial->getDiffuse().Y = lWords.at(2).toDouble();
						pMaterial->getDiffuse().Z = lWords.at(3).toDouble();
					}
				}
				else if (sFirstWord == "Ks" && lWords.count() > 3)
				{
					// Couleur spéculaire

					if (pMaterial != NULL)
					{
						pMaterial->getSpecular().X = lWords.at(1).toDouble();
						pMaterial->getSpecular().Y = lWords.at(2).toDouble();
						pMaterial->getSpecular().Z = lWords.at(3).toDouble();
					}
				}
				else if (sFirstWord == "Ke" && lWords.count() > 3)
				{
					// Couleur d'émission

					/*
					double r = lWords.at(1).toDouble();
					double g = lWords.at(2).toDouble();
					double b = lWords.at(3).toDouble();
					*/
				}
				else if (sFirstWord == "map_Kd" && lWords.count() > 1)
				{
					if (pMaterial != NULL)
					{
						QString sTextureName = lWords.at(1);
						sTextureName = sTextureName.replace("\\\\", "/");

						QFileInfo info(sTextureName);
						QString sFileName = info.fileName();

						if (sFileName.startsWith(ParamName_DynTex))
						{
							pMaterial->addDynamicDiffuseTexture(sTextureName);
						}
						else
						{
							pMaterial->addDiffuseTexture(sTextureName);
						}
					}
				}
			}
		}
	}

	LOG_DEBUG(QString("COBJLoader::loadMaterials() : END : %1").arg(pMesh->getName()));
}
