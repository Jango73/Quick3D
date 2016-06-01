
#pragma once

#include "quick3d_global.h"

// Qt
#include <QMutex>
#include <QFile>

// Fondations
#include "CVector3.h"
#include "CGeoloc.h"
#include "CXMLNode.h"

/*

    Description du référentiel de la tuile

    m_gSize.Longitude
	<-------------->

    ----------------  ^
    |              |  |
    |              |  |
    |  m_gGeoloc   |  |
    |      .       |  | m_gSize.Latitude
    |              |  |
    |              |  |
    |              |  |
    ----------------  v

*/

//! Classe de stockage d'une tuile HGT
class QUICK3D_EXPORT CHGTData
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//!
	CHGTData(const Math::CGeoloc& gGeoloc, const Math::CGeoloc& gSize);

	//!
	virtual ~CHGTData();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//!
	void setFileName(QString sFileName);

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//!
	double getHeightAt(const Math::CGeoloc& gPosition, double* pRigidness = NULL);

	//!
	int getNumCellsWidth() { return m_iNumCellsWidth; }

	//!
	int getNumCellsHeight() { return m_iNumCellsHeight; }

	//-------------------------------------------------------------------------------------------------
	// Méthodes de contrôle
	//-------------------------------------------------------------------------------------------------

	//!
	bool contains(const Math::CGeoloc& gPosition) const;

	//-------------------------------------------------------------------------------------------------
	// Méthodes protégées
	//-------------------------------------------------------------------------------------------------

protected:

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

protected:

	QMutex				m_tMutex;
	QFile*				m_pFile;
	QString				m_sFileName;
	Math::CGeoloc		m_gGeoloc;
	Math::CGeoloc		m_gSize;
	int					m_iNumCellsWidth;
	int					m_iNumCellsHeight;
};
