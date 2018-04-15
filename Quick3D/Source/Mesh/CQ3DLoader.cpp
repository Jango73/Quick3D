
// Qt
#include <QTextStream>

// qt-plus
#include "CLogger.h"

// Application
#include "C3DScene.h"
#include "CQ3DLoader.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

/*!
    \class CQ3DLoader
    \brief A class that loads a .q3d file.
    \inmodule Quick3D
*/

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CQ3DLoader with its default parameters.
*/
CQ3DLoader::CQ3DLoader()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Loads a .q3d file. \br\br
    \a sBaseFile is the name of the file containing data.
    \a pContainer is the component that will contain the loaded data.
    \a sText is the q3d data.
*/
QSP<CMeshGeometry> CQ3DLoader::load(const QString& sBaseFile, CComponent* pContainer, QString sText)
{
    CXMLNode xNode = CXMLNode::parseXML(sText);

    QVector<QSP<CMaterial> > vMaterials;

    QSP<CMeshGeometry> pMesh = QSP<CMeshGeometry>(new CMeshGeometry(pContainer->scene()));

    loadComponent(sBaseFile, pContainer, pMesh, xNode, vMaterials);

    // Elaboration de la boite englobante

    CBoundingBox bBox;
    CMatrix4 mTransform;

    addBounds(pContainer, bBox, mTransform);

    pMesh->setBounds(bBox);

    return pMesh;
}

//-------------------------------------------------------------------------------------------------

void CQ3DLoader::loadComponent(
        const QString& sBaseFile,
        CComponent* pContainer,
        QSP<CMeshGeometry> pMesh,
        CXMLNode xComponent,
        QVector<QSP<CMaterial> >& vMaterials,
        CComponent* pParent
        )
{
    CXMLNode xPositionNode = xComponent.getNodeByTagName(ParamName_Position);
    CXMLNode xRotationNode = xComponent.getNodeByTagName(ParamName_Rotation);

    // Assignation du parent
    if (pParent != nullptr)
    {
        pContainer->setParent(QSP<CComponent>(pParent));
    }

    // Assignation du nom
    if (xComponent.attributes()[ParamName_Name].isEmpty() == false)
    {
        pContainer->setName(xComponent.attributes()[ParamName_Name]);
    }

    // Position d'origine
    if (xPositionNode.isEmpty() == false)
    {
        pContainer->setPosition(CVector3(
                                     xPositionNode.attributes()[ParamName_x].toDouble(),
                                     xPositionNode.attributes()[ParamName_y].toDouble(),
                                     xPositionNode.attributes()[ParamName_z].toDouble()
                                     ));
    }

    // Rotation d'origine
    if (xRotationNode.isEmpty() == false)
    {
        pContainer->setRotation(CVector3(
                                     xRotationNode.attributes()[ParamName_x].toDouble(),
                                     xRotationNode.attributes()[ParamName_y].toDouble(),
                                     xRotationNode.attributes()[ParamName_z].toDouble()
                                     ));
    }

    // Lecture des matériaux
    QVector<CXMLNode> vxMaterials = xComponent.getNodesByTagName(ParamName_Material);

    foreach (CXMLNode xMaterial, vxMaterials)
    {
        CMaterial* pNewMaterial = new CMaterial(pContainer->scene());

        pNewMaterial->loadParameters(sBaseFile, xMaterial);

        vMaterials.append(QSP<CMaterial>(pNewMaterial));
    }

    // Lecture des vertex
    CXMLNode xVertices = xComponent.getNodeByTagName(ParamName_Vertices);

    QVector<CXMLNode> vxVertices = xVertices.getNodesByTagName(ParamName_Vertex);

    foreach (CXMLNode xVertex, vxVertices)
    {
        double x = xVertex.attributes()[ParamName_x].toDouble();
        double y = xVertex.attributes()[ParamName_y].toDouble();
        double z = xVertex.attributes()[ParamName_z].toDouble();
        double u = xVertex.attributes()[ParamName_u].toDouble();
        double v = xVertex.attributes()[ParamName_v].toDouble();

        pMesh->vertices().append(CVertex(CVector3(x, y, z), CVector2(u, v)));
    }

    // Map servant à définir des index de matériaux locaux au mesh
    // Seuls les matériaux utilisés par le mesh y sont stockés
    QMap<int, int> mMaterialIndex;

    // Le composant racine doit stocker tous les matériaux pour permettre
    // l'activation des textures dynamiques
    if (pParent == nullptr)
    {
        for (int iIndex = 0; iIndex < vMaterials.count(); iIndex++)
        {
            mMaterialIndex[iIndex] = 0;
        }
    }

    // Lecture des polygones
    CXMLNode xFaces = xComponent.getNodeByTagName(ParamName_Faces);

    QVector<CXMLNode> vxFaces = xFaces.getNodesByTagName(ParamName_Face);

    foreach (CXMLNode xFace, vxFaces)
    {
        CFace NewFace(pMesh.data());

        QStringList lVertices = xFace.attributes()[ParamName_Vertices].split(",");

        foreach (QString sVertex, lVertices)
        {
            NewFace.indices().append(sVertex.toInt());
        }

        int iMaterialIndex = xFace.attributes()[ParamName_Material].toInt();

        if (iMaterialIndex < vMaterials.count())
        {
            NewFace.setMaterialIndex(iMaterialIndex);

            if (mMaterialIndex.contains(iMaterialIndex) == false)
            {
                mMaterialIndex[iMaterialIndex] = 0;
            }
        }

        pMesh->faces().append(NewFace);
    }

    pMesh->materials().clear();

    // Ajout de chaque matériau utilisé par le mesh à ce dernier
    foreach (int iUsedMaterial, mMaterialIndex.keys())
    {
        mMaterialIndex[iUsedMaterial] = pMesh->materials().count();
        pMesh->materials().append(vMaterials[iUsedMaterial]);
    }

    // Modification des indices de matériau des polygones
    for (int iFaceIndex = 0; iFaceIndex < pMesh->faces().count(); iFaceIndex++)
    {
        int iFaceMaterialIndex = pMesh->faces()[iFaceIndex].materialIndex();
        int iNewFaceMaterialIndex = mMaterialIndex[iFaceMaterialIndex];
        pMesh->faces()[iFaceIndex].setMaterialIndex(iNewFaceMaterialIndex);
    }

    // Chargement des noeuds enfants
    QVector<CXMLNode> vComponents = xComponent.getNodesByTagName(ParamName_Component);

    foreach (CXMLNode xChild, vComponents)
    {
        CMesh* pChildMesh = new CMesh(pContainer->scene());

        loadComponent(sBaseFile, pChildMesh, pChildMesh->geometry(), xChild, vMaterials, pContainer);
    }
}

//-------------------------------------------------------------------------------------------------

void CQ3DLoader::addBounds(CComponent* pContainer, CBoundingBox& bBox, CMatrix4 mTransform)
{
    CMatrix4 mLocalTransform;
    CVector3 vRotation;
    CVector3 vPosition;

    if (pContainer->isRootObject() == false)
    {
        vPosition = pContainer->position();
        vRotation = pContainer->rotation();
    }

    mLocalTransform = mLocalTransform * CMatrix4::makeRotation(vRotation);
    mLocalTransform = mLocalTransform * CMatrix4::makeTranslation(vPosition);

    mTransform = mLocalTransform * mTransform;

    bBox = bBox & pContainer->bounds().transformed(mTransform);

    foreach (QSP<CComponent> pChild, pContainer->childComponents())
    {
        QSP<CMesh> pChildMesh = QSP_CAST(CMesh, pChild);

        if (pChildMesh != nullptr)
        {
            addBounds(pChildMesh.data(), bBox, mTransform);
        }
    }
}
