
#ifndef CPREFERENCESMANAGER_H
#define CPREFERENCESMANAGER_H

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Qt
#include <QObject>

// Application
#include "CSingleton.h"
#include "CXMLNode.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CPreferencesManager : public QObject, public CSingleton<CPreferencesManager>
{
	friend class CSingleton<CPreferencesManager>;

	Q_OBJECT

public:

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//! Retourne une chaine de caractère correspondant à l'ID donné
	CXMLNode& getPreferences() { return xPreferences; }

protected:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//! Constructeur
	CPreferencesManager();

	//! Destructeur
	virtual ~CPreferencesManager();

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

	CXMLNode		xPreferences;
};

#endif // CPREFERENCESMANAGER_H
