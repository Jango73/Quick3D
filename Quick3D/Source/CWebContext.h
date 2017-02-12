
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Qt
#include <QString>
#include <QStringList>
#include <QMap>
#include <QTcpSocket>

//-------------------------------------------------------------------------------------------------

//! D�finit un contexte de serveur HTTP
class QUICK3D_EXPORT CWebContext
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructors and destructor
	//-------------------------------------------------------------------------------------------------

    //! Default constructor
    CWebContext();

	//! Constructeur
    CWebContext(QTcpSocket* pSocket, QString sPeer, QString sHost, QStringList lPath, QMap<QString, QString> mArguments);

	//! Destructor
	virtual ~CWebContext();

	//-------------------------------------------------------------------------------------------------
	// Properties
	//-------------------------------------------------------------------------------------------------

public:

    QTcpSocket*				m_pSocket;
    QString					m_sPeer;
	QString					m_sHost;
	QStringList				m_lPath;
	QMap<QString, QString>	m_mArguments;
	QByteArray				m_baPostContent;
};
