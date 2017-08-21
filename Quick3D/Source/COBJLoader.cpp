
// Qt
#include <QTextStream>

// qt-plus
#include "CLogger.h"

// Application
#include "C3DScene.h"
#include "COBJLoader.h"

//-------------------------------------------------------------------------------------------------

#define TOKEN_comment               "#"
#define TOKEN_material_lib          "mtllib"
#define TOKEN_new_material          "newmtl"
#define TOKEN_material_ambient      "Ka"
#define TOKEN_material_diffuse      "Kd"
#define TOKEN_material_specular     "Ks"
#define TOKEN_material_emission     "Ke"
#define TOKEN_texture_diffuse       "map_Kd"
#define TOKEN_use_material          "usemtl"
#define TOKEN_vertex                "v"
#define TOKEN_vertex_normal         "vn"
#define TOKEN_vertex_texture        "vt"
#define TOKEN_smoothing             "s"
#define TOKEN_face                  "f"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

COBJLoader::COBJLoader()
{
}

//-------------------------------------------------------------------------------------------------

QSP<CMeshGeometry> COBJLoader::load(const QString& sBaseFile, CComponent *pContainer, QString sText)
{
    QTextStream sInput(&sText, QIODevice::ReadOnly);

    QSP<CMeshGeometry> pMesh = QSP<CMeshGeometry>(new CMeshGeometry(pContainer->scene()));

    pMesh->faces().clear();
    pMesh->vertices().clear();

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

                if (sFirstWord == TOKEN_comment)
                {
                }
                else if (sFirstWord == TOKEN_material_lib)
                {
                    QString sMaterialFileName = ":/Resources/" + lWords.at(1);

                    loadMaterials(
                                sBaseFile,
                                pContainer->scene(),
                                pMesh.data(),
                                pContainer->scene()->ressourcesManager()->getObjByFilePathName(sMaterialFileName)
                                );
                }
                else if (sFirstWord == "o" && lWords.count() > 1)
                {
                    QString sObjectName = lWords.at(1);
                }
                else if (sFirstWord == TOKEN_use_material && lWords.count() > 1)
                {
                    QString sMaterialName = lWords.at(1);

                    for (int iIndex = 0; iIndex < pMesh->materials().count(); iIndex++)
                    {
                        if (pMesh->materials()[iIndex]->name() == sMaterialName)
                        {
                            iMaterialIndex = iIndex;
                        }
                    }
                }
                else if (sFirstWord == TOKEN_vertex && lWords.count() > 3)
                {
                    double x = lWords.at(1).toDouble();
                    double y = lWords.at(3).toDouble();
                    double z = lWords.at(2).toDouble();

                    pMesh->vertices().append(CVertex(CVector3(x, y, z), CVector2()));
                }
                else if (sFirstWord == TOKEN_vertex_normal && lWords.count() > 3)
                {
                    // double x = lWords.at(1).toDouble();
                    // double y = lWords.at(3).toDouble();
                    // double z = lWords.at(2).toDouble();
                }
                else if (sFirstWord == TOKEN_vertex_texture && lWords.count() > 2)
                {
                    double u = lWords.at(2).toDouble();
                    double v = lWords.at(1).toDouble();

                    vTextureVertices.append(CVector2(v, u));
                }
                else if (sFirstWord == TOKEN_smoothing && lWords.count() > 1)
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
                else if (sFirstWord == TOKEN_face && lWords.count() > 3)
                {
                    CFace NewFace(pMesh.data());

                    NewFace.setSmoothingGroup(iSmoothingGroup);
                    NewFace.setMaterialIndex(iMaterialIndex);

                    for (int Index = 1; Index < lWords.count(); Index++)
                    {
                        QStringList sFaceIndices = lWords.at(Index).split('/', QString::KeepEmptyParts);

                        int iPositionIndex = -1;
                        int iTextureIndex = -1;
                        // int iNormalIndex = -1;

                        if (sFaceIndices.count() > 0)
                        {
                            iPositionIndex = sFaceIndices.at(0).toInt() - 1;
                        }

                        if (sFaceIndices.count() > 1)
                        {
                            iTextureIndex = sFaceIndices.at(1).toInt() - 1;
                        }

                        /*
                        if (sFaceIndices.count() > 2)
                        {
                            iNormalIndex = sFaceIndices.at(2).toInt() - 1;
                        }
                        */

                        if (iPositionIndex > 0)
                        {
                            NewFace.getIndices().append(iPositionIndex);

                            if (iTextureIndex > 0 && iTextureIndex < vTextureVertices.count())
                            {
                                pMesh->vertices()[iPositionIndex].texCoord() = vTextureVertices[iTextureIndex];
                            }
                        }
                    }

                    pMesh->faces().append(NewFace);
                }
            }
        }
    }

    // Séparation des polygones en fonction des groupes de lissage
    pMesh->splitVerticesBySmoothingGroup();

    return pMesh;
}

//-------------------------------------------------------------------------------------------------

void COBJLoader::loadMaterials(const QString& sBaseFile, C3DScene* pScene, CMeshGeometry* pMesh, QString sText)
{
    Q_UNUSED(sBaseFile);

    QTextStream sInput(&sText, QIODevice::ReadOnly);

    CMaterial* pMaterial = nullptr;

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

                if (sFirstWord == TOKEN_comment)
                {
                }
                else if (sFirstWord == TOKEN_new_material && lWords.count() > 1)
                {
                    pMaterial = new CMaterial(pScene, lWords.at(1));

                    pMesh->materials().append(pScene->ressourcesManager()->shareMaterial(QSP<CMaterial>(pMaterial)));
                }
                else if (sFirstWord == TOKEN_material_ambient && lWords.count() > 3)
                {
                    // Couleur ambiante
                    // Ambient color

                    if (pMaterial != nullptr)
                    {
                        pMaterial->ambient().X = lWords.at(1).toDouble();
                        pMaterial->ambient().Y = lWords.at(2).toDouble();
                        pMaterial->ambient().Z = lWords.at(3).toDouble();
                    }
                }
                else if (sFirstWord == TOKEN_material_diffuse && lWords.count() > 3)
                {
                    // Couleur diffuse
                    // Diffuse color

                    if (pMaterial != nullptr)
                    {
                        pMaterial->diffuse().X = lWords.at(1).toDouble();
                        pMaterial->diffuse().Y = lWords.at(2).toDouble();
                        pMaterial->diffuse().Z = lWords.at(3).toDouble();
                    }
                }
                else if (sFirstWord == TOKEN_material_specular && lWords.count() > 3)
                {
                    // Couleur spéculaire
                    // Specular color

                    if (pMaterial != nullptr)
                    {
                        pMaterial->specular().X = lWords.at(1).toDouble();
                        pMaterial->specular().Y = lWords.at(2).toDouble();
                        pMaterial->specular().Z = lWords.at(3).toDouble();
                    }
                }
                else if (sFirstWord == TOKEN_material_emission && lWords.count() > 3)
                {
                    // Couleur d'émission
                    // Emission color

                    /*
                    double r = lWords.at(1).toDouble();
                    double g = lWords.at(2).toDouble();
                    double b = lWords.at(3).toDouble();
                    */
                }
                else if (sFirstWord == TOKEN_texture_diffuse && lWords.count() > 1)
                {
                    if (pMaterial != nullptr)
                    {
                        QString sTextureName = lWords.at(1);
                        sTextureName = sTextureName.replace("\\\\", "/");

                        QFileInfo info(sTextureName);
                        QString sFileName = info.fileName();

                        if (sFileName.startsWith(ParamName_DynTex))
                        {
                            pMaterial->addDynamicDiffuseTexture(sBaseFile, sTextureName);
                        }
                        else
                        {
                            pMaterial->addDiffuseTexture(sBaseFile, sTextureName);
                        }
                    }
                }
            }
        }
    }
}
