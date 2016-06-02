
#ifndef CRESSOURCESMANAGER_H
#define CRESSOURCESMANAGER_H

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Qt
#include <QHash>
#include <QPixmap>
#include <QIcon>
#include <QString>
#include <QMutex>
#include <QMutexLocker>

// Application
#include "CMeshInstance.h"
#include "CMaterialInstance.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CRessourcesManager
{
public:

	//! Conctructeur
	CRessourcesManager(C3DScene* pScene);

	//! Destructeur
	virtual ~CRessourcesManager();

	//!
	void loadMesh(CMesh* pMesh, const QString& sFileName);

	//! Recupere un shader par son nom
	QString getShaderByFilePathName(const QString& filePathName);

	//! Recupere un .obj par son nom
	QString getObjByFilePathName(const QString& filePathName);

	//! Recupere un îcone par son nom
	QIcon* getIconByFilePathName(const QString& filePathName);

	//! Recupere un pixmap par son nom
	QPixmap* getPixmapByFilePathName(const QString& filePathName);

	//!
	CMeshInstance* share(QSharedPointer<CMesh> pMesh);

	//!
	CMeshInstance* share(QVector<QSharedPointer<CMesh> > vMeshes);

	//!
	QSharedPointer<CMaterial> shareMaterial(QSharedPointer<CMaterial> pMaterial);

	//!
	QSharedPointer<CMaterial> getDefaultMaterial() { return m_pDefaultMaterial; }

	//!
	QSharedPointer<CMaterial> getWaterMaterial() { return m_pWaterMaterial; }

	//!
	QSharedPointer<CMaterial> getSkyboxMaterial() { return m_pSkyboxMaterial; }

	//!
	QSharedPointer<CMaterial> getTreeMaterial() { return m_pTreeMaterial; }

protected:

	QMutex		m_mMutex;
	C3DScene*	m_pScene;

	//! Table de hachage ou sont stockees les shaders
	QHash<QString, QString> m_Shaders;

	//! Table de hachage ou sont stockees les obj
	QHash<QString, QString> m_Objs;

	//! Table de hachage ou sont stockees les icones
	QHash<QString, QIcon*> m_Icons;

	//! Table de hachage ou sont stockees les pixmaps
	QHash<QString, QPixmap*> m_Pixmaps;

	//! Table des mesh
	QVector<QSharedPointer<CMesh> > m_vMeshes;

	//! Material table
	QVector<QSharedPointer<CMaterial> > m_vMaterials;

	//!
	QSharedPointer<CMaterial> m_pDefaultMaterial;

	//!
	QSharedPointer<CMaterial> m_pWaterMaterial;

	//!
	QSharedPointer<CMaterial> m_pSkyboxMaterial;

	//!
	QSharedPointer<CMaterial> m_pTreeMaterial;
};

#endif // CRESSOURCESMANAGER_H
