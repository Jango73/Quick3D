
#ifndef CCONNECTEDSTREAM_H
#define CCONNECTEDSTREAM_H

#include "quick3d_global.h"

// Qt
#include <QIODevice>

class QUICK3D_EXPORT CConnectedStream : public QIODevice
{
	Q_OBJECT

public:

	//-------------------------------------------------------------------------------------------------
	// Constructors and destructor
	//-------------------------------------------------------------------------------------------------

	//! Default constructor
	CConnectedStream();

	//! Destructor
	virtual ~CConnectedStream();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//!
	void setMinBytesForReadyRead(int iMinBytes);

	//-------------------------------------------------------------------------------------------------
	// Signaux
	//-------------------------------------------------------------------------------------------------

signals:

	//!
	void connected();

	//!
	void disconnected();

	//-------------------------------------------------------------------------------------------------
	// Properties
	//-------------------------------------------------------------------------------------------------

protected:

	int	m_iMinBytesForReadyRead;
};

#endif // CCONNECTEDSTREAM_H
