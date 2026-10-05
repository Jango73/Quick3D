
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Qt
#include <QObject>

// qt-plus
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

	//! Returns a string matching the given ID
    CXMLNode& preferences() { return xPreferences; }

protected:

	//-------------------------------------------------------------------------------------------------
	// Constructors and destructor
	//-------------------------------------------------------------------------------------------------

	//! Constructor
	CPreferencesManager();

	//! Destructor
	virtual ~CPreferencesManager();

	//-------------------------------------------------------------------------------------------------
	// Properties
	//-------------------------------------------------------------------------------------------------

	CXMLNode		xPreferences;
};
