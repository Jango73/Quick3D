
#ifndef CHTTPSERVER_H
#define CHTTPSERVER_H

#include "quick3d_global.h"

// Qt
#include <QDebug>
#include <QString>
#include <QTcpServer>
#include <QTcpSocket>

//-------------------------------------------------------------------------------------------------

// New line as output in web page
#define HTML_NL		"\n"
// #define HTML_NL

//-------------------------------------------------------------------------------------------------

//! Définit un contexte de serveur HTTP
class QUICK3D_EXPORT CWebContext
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//! Constructeur
	CWebContext(QTcpSocket* pSocket, QString sPeer, QString sHost, QStringList lPath, QMap<QString, QString> mArguments);

	//! Destructeur
	virtual ~CWebContext();

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

public:

	QTcpSocket*				m_pSocket;
	QString					m_sPeer;
	QString					m_sHost;
	QStringList				m_lPath;
	QMap<QString, QString>	m_mArguments;
	QByteArray				m_baPostContent;
};

//-------------------------------------------------------------------------------------------------

//! Définit un serveur HTTP
class QUICK3D_EXPORT CHTTPServer : public QTcpServer
{
	Q_OBJECT

public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//! Constructeur
	CHTTPServer(quint16 port, QObject* parent = 0);

	//! Destructeur
	virtual ~CHTTPServer();

	//-------------------------------------------------------------------------------------------------
	// Méthodes de contrôle
	//-------------------------------------------------------------------------------------------------

	//!
	void addAuthorizedFolder(QString sFolderName);

	//!
	void pause();

	//!
	void resume();

	//! A implémenter par toute classe dérivée pour générer du contenu web
	//! Retourne le contenu de la page web dynamique, à implémenter par toute classe dérivée
	//! Si sCustomResponse est non vide, il sera retourné tel quel au client, sHead et sBody seront ignorés
	//! Sinon, si xmlResponse est non vide, il sera retourné tel quel au client, sHead et sBody seront ignorés
	//! Sinon sHead et sBody seront retournés au client sous forme d'un document HTML
	virtual void getContent(const CWebContext& tContext, QString& sHead, QString& sBody, QString& xmlResponse, QString& sCustomResponse);

	//! A implémenter par toute classe dérivée pour réagir après l'envoi de données au client
	virtual void handleSocketBytesWritten(QTcpSocket* pSocket, qint64 iBytes);

	virtual void handleSocketBytesRead(QTcpSocket* pSocket);

	//! A implémenter par toute classe dérivée pour réagir après déconnexion d'un client
	virtual void handleSocketDisconnection(QTcpSocket* pSocket);

	//-------------------------------------------------------------------------------------------------
	// Méthodes statiques
	//-------------------------------------------------------------------------------------------------

	//! Encode une chaîne au format URL (remplace les caractères ASCII par leur %XX correspondant)
	static QString encodeURLParameters(QString sText);

	//! Décode une chaîne encodée au format URL (remplace les %XX par leur caractère ASCII correspondant)
	static QString decodeURLParameters(QString sText);

	//-------------------------------------------------------------------------------------------------
	// Méthodes protégées
	//-------------------------------------------------------------------------------------------------

protected:

	//! Retourne le nombre d'octets attendus dans la requête
	int getExpectedBytes(QByteArray baData);

	//! Prend en charge la requête du client
	void processRequest(QTcpSocket* pSocket);

	//! Récupère et envoi au client le fichier demandé s'il existe
	bool getResponseFile(const CWebContext& tContext, QTcpSocket* pSocket);

	//! Récupère et envoi au client le contenu dynamique
	//! Renvoie vrai si la connexion doit être keep-alive
	bool getResponseDynamicContent(const CWebContext& tContext, QTcpSocket* pSocket);

	//! Retourne le chemin décomposé et les arguments d'une requête
	void getRequestPathAndArgs(const QStringList& lTokens, QStringList& lPath, QMap<QString, QString>& mArguments);

	//! Place dans baContent le contenu d'une requête POST
	void getRequestPostContent(const QString& sText, QByteArray& baContent);

	//! Retourne la valeur correspondant à un token MIME dans un header HTTP
	QString getTokenValue(const QStringList& lTokens, QString sTokenName);

	//! Retourne le type MIME correspondant à une extension de fichier
	QString getContentTypeByExtension(const QString& sExtension) const;

	//! Log une requête HTTP entrante
	void LogRequest(QString sIP, QString sText);

	//-------------------------------------------------------------------------------------------------
	// Slots protégés
	//-------------------------------------------------------------------------------------------------

protected slots:

	void onNewConnection();
	void onSocketDisconnected();
	void onSocketReadyRead();
	void onSocketBytesWritten(qint64 iBytes);

	//-------------------------------------------------------------------------------------------------
	// Classes imbriquées
	//-------------------------------------------------------------------------------------------------

public:

	//! Cette classe utilitaire sert à associer des données utilisateur à une socket
	//! Le pointeur vers cette classe est stocké dans les propriétés de QTcpSocket
	class CClientData
	{
	public:

		#define PROP_DATA "Data"

		//!
		CClientData(QTcpSocket* pSocket)
			: m_bHeaderRead(false)
			, m_iExpectedBytes(0)
		{
			pSocket->setProperty(PROP_DATA, (qulonglong) this);
		}

		//!
		static CClientData* getFromSocket(QTcpSocket* pSocket)
		{
			return (CClientData*) pSocket->property(PROP_DATA).toULongLong();
		}

		//!
		static void deleteFromSocket(QTcpSocket* pSocket)
		{
			CClientData* pData = getFromSocket(pSocket);

			if (pData != NULL)
			{
				delete pData;
			}
		}

		bool					m_bHeaderRead;
		int						m_iExpectedBytes;
		QByteArray				m_baBuffer;
		QMap<QString, QVariant>	m_vUserData;
	};

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

protected:

	bool					m_bDisabled;				// Indique si le serveur doit ignorer les requêtes entrantes
	bool					m_bFirstMessage;				// Indique s'il s'agit du premier message lu
	QVector<QString>		m_vAuthorizedFolders;		// Indique quels sont les répertoires autorisés pour les GET non-dynamiques
	QMap<QString, QString>	m_vExtensionToContentType;	// Tableau pour conversion de token vers type MIME
};

#endif // CHTTPSERVER_H
