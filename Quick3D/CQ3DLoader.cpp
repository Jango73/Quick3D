
// Qt
#include <QTextStream>

// Application
#include "CLogManager.h"
#include "C3DScene.h"
#include "CQ3DLoader.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CQ3DLoader::CQ3DLoader()
{
}

//-------------------------------------------------------------------------------------------------

void CQ3DLoader::load(C3DScene* pScene, CMesh* pMesh, QString sText)
{
	CXMLNode xNode = CXMLNode::parseXML(sText);

	QVector<QSharedPointer<CMaterial> > vMaterials;

	loadComponent(pScene, pMesh, xNode, vMaterials);

	// Elaboration de la boite englobante

	CBoundingBox bBox;
	CMatrix4 mTransform;

	addBounds(pMesh, bBox, mTransform);

	pMesh->setBounds(bBox);
}

//-------------------------------------------------------------------------------------------------

void CQ3DLoader::loadComponent(
		C3DScene* pScene,
		CMesh* pMesh,
		CXMLNode xComponent,
		QVector<QSharedPointer<CMaterial> >& vMaterials,
		CComponent* pParent
		)
{
	CXMLNode xPositionNode = xComponent.getNodeByTagName(ParamName_Position);
	CXMLNode xRotationNode = xComponent.getNodeByTagName(ParamName_Rotation);

	// Assignation du parent
	if (pParent != NULL)
	{
		pMesh->setParent(pParent);
	}

	// Assignation du nom
	if (xComponent.m_vAttributes[ParamName_Name].isEmpty() == false)
	{
		pMesh->setName(xComponent.m_vAttributes[ParamName_Name]);
	}

	// Position d'origine
	if (xPositionNode.isEmpty() == false)
	{
		pMesh->setOriginPosition(CVector3(
			xPositionNode.m_vAttributes[ParamName_x].toDouble(),
			xPositionNode.m_vAttributes[ParamName_y].toDouble(),
			xPositionNode.m_vAttributes[ParamName_z].toDouble()
		));
	}

	// Rotation d'origine
	if (xRotationNode.isEmpty() == false)
	{
		pMesh->setOriginRotation(CVector3(
			xRotationNode.m_vAttributes[ParamName_x].toDouble(),
			xRotationNode.m_vAttributes[ParamName_y].toDouble(),
			xRotationNode.m_vAttributes[ParamName_z].toDouble()
		));
	}

	// Lecture des matériaux
	QVector<CXMLNode> vxMaterials = xComponent.getNodesByTagName(ParamName_Material);

	foreach (CXMLNode xMaterial, vxMaterials)
	{
		QString sMaterialName = xMaterial.m_vAttributes[ParamName_Name];

		CMaterial* pNewMaterial = new CMaterial(pScene, sMaterialName);

		// Lecture couleur ambiante
		CXMLNode xAmbient = xMaterial.getNodeByTagName(ParamName_Ambient);

		// Lecture couleur diffuse
		CXMLNode xDiffuse = xMaterial.getNodeByTagName(ParamName_Diffuse);

		// Lecture couleur spéculaire
		CXMLNode xSpecular = xMaterial.getNodeByTagName(ParamName_Specular);

		if (xDiffuse.isEmpty() == false)
		{
			pNewMaterial->getAmbient().X = xAmbient.m_vAttributes[ParamName_r].toDouble();
			pNewMaterial->getAmbient().Y = xAmbient.m_vAttributes[ParamName_g].toDouble();
			pNewMaterial->getAmbient().Z = xAmbient.m_vAttributes[ParamName_b].toDouble();

			pNewMaterial->getDiffuse().X = xDiffuse.m_vAttributes[ParamName_r].toDouble();
			pNewMaterial->getDiffuse().Y = xDiffuse.m_vAttributes[ParamName_g].toDouble();
			pNewMaterial->getDiffuse().Z = xDiffuse.m_vAttributes[ParamName_b].toDouble();

			pNewMaterial->getSpecular().X = xSpecular.m_vAttributes[ParamName_r].toDouble();
			pNewMaterial->getSpecular().Y = xSpecular.m_vAttributes[ParamName_g].toDouble();
			pNewMaterial->getSpecular().Z = xSpecular.m_vAttributes[ParamName_b].toDouble();

			QString sTextureName = xDiffuse.m_vAttributes[ParamName_Map];

			if (sTextureName.isEmpty() == false)
			{
				QString sFileName = QFileInfo(sTextureName).fileName();

				if (sFileName.startsWith(ParamName_DynTex))
				{
					pNewMaterial->addDynamicDiffuseTexture(sTextureName);
				}
				else
				{
					pNewMaterial->addDiffuseTexture(sTextureName);
				}
			}
		}

		vMaterials.append(QSharedPointer<CMaterial>(pNewMaterial));
	}

	// Lecture des vertex
	CXMLNode xVertices = xComponent.getNodeByTagName(ParamName_Vertices);

	QVector<CXMLNode> vxVertices = xVertices.getNodesByTagName(ParamName_Vertex);

	foreach (CXMLNode xVertex, vxVertices)
	{
		double x = xVertex.m_vAttributes[ParamName_x].toDouble();
		double y = xVertex.m_vAttributes[ParamName_y].toDouble();
		double z = xVertex.m_vAttributes[ParamName_z].toDouble();
		double u = xVertex.m_vAttributes[ParamName_u].toDouble();
		double v = xVertex.m_vAttributes[ParamName_v].toDouble();

		pMesh->getVertices().append(CVertex(CVector3(x, y, z), CVector2(u, v)));
	}

	// Map servant à définir des index de matériaux locaux au mesh
	// Seuls les matériaux utilisés par le mesh y sont stockés
	QMap<int, int> mMaterialIndex;

	// Le composant racine doit stocker tous les matériaux pour permettre
	// l'activation des textures dynamiques
	if (pParent == NULL)
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
		CFace NewFace;

		QStringList lVertices = xFace.m_vAttributes[ParamName_Vertices].split(",");

		foreach (QString sVertex, lVertices)
		{
			NewFace.getIndices().append(sVertex.toInt());
		}

		int iMaterialIndex = xFace.m_vAttributes[ParamName_Material].toInt();

		if (iMaterialIndex < vMaterials.count())
		{
			NewFace.setMaterialIndex(iMaterialIndex);

			if (mMaterialIndex.contains(iMaterialIndex) == false)
			{
				mMaterialIndex[iMaterialIndex] = 0;
			}
		}

		pMesh->getFaces().append(NewFace);
	}

	pMesh->getMaterials().clear();

	// Ajout de chaque matériau utilisé par le mesh à ce dernier
	foreach (int iUsedMaterial, mMaterialIndex.keys())
	{
		mMaterialIndex[iUsedMaterial] = pMesh->getMaterials().count();
		pMesh->getMaterials().append(vMaterials[iUsedMaterial]);
	}

	// Modification des indices de matériau des polygones
	for (int iFaceIndex = 0; iFaceIndex < pMesh->getFaces().count(); iFaceIndex++)
	{
		int iFaceMaterialIndex = pMesh->getFaces()[iFaceIndex].getMaterialIndex();
		int iNewFaceMaterialIndex = mMaterialIndex[iFaceMaterialIndex];
		pMesh->getFaces()[iFaceIndex].setMaterialIndex(iNewFaceMaterialIndex);
	}

	// Mise à jour de la géométrie du mesh
	pMesh->updateGeometry();

	// Chargement des noeuds enfants
	QVector<CXMLNode> vComponents = xComponent.getNodesByTagName(ParamName_Component);

	foreach (CXMLNode xChild, vComponents)
	{
		CMesh* pChildMesh = new CMesh(pScene);

		loadComponent(pScene, pChildMesh, xChild, vMaterials, pMesh);
	}
}

//-------------------------------------------------------------------------------------------------

void CQ3DLoader::addBounds(CMesh* pMesh, CBoundingBox& bBox, CMatrix4 mTransform)
{
	CMatrix4 mLocalTransform;
	CVector3 vRotation;
	CVector3 vPosition;

	if (pMesh->isRootObject() == false)
	{
		vPosition = pMesh->getOriginPosition();
		vRotation = pMesh->getOriginRotation();
	}

	mLocalTransform = mLocalTransform * CMatrix4::MakeRotation(vRotation);
	mLocalTransform = mLocalTransform * CMatrix4::MakeTranslation(vPosition);

	mTransform = mLocalTransform * mTransform;

	bBox = bBox & pMesh->getBounds().transformed(mTransform);

	foreach (CComponent* pChild, pMesh->getChildren())
	{
		CMesh* pChildMesh = dynamic_cast<CMesh*>(pChild);

		if (pChildMesh != NULL)
		{
			addBounds(pChildMesh, bBox, mTransform);
		}
	}
}
