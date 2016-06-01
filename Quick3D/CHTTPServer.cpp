
// Qt
#include <QDateTime>
#include <QCoreApplication>
#include <QFile>

// Application
#include "CLogManager.h"
#include "CHTTPServer.h"

//-------------------------------------------------------------------------------------------------

// 1er token d'un header HTTP

#define HTTP_HEADER						"HTTP/1.0 "

// Liste des retours possibles d'un serveur HTTP

#define HTTP_200_OK						"200 Ok"

#define HTTP_201_CREATED				"201 Created"
#define HTTP_202_ACCEPTED				"202 Accepted"
#define HTTP_203_NA_INFORMATION			"203 Non-Authoritative Information"
#define HTTP_204_NO_CONTENT				"204 No Content"
#define HTTP_205_RESET_CONTENT			"205 Reset Content"
#define HTTP_206_PARTIAL_CONTENT		"206 Partial Content"

#define HTTP_400_BAD_REQUEST			"400 Bad Request"
#define HTTP_401_UNAUTHORIZED			"401 Unauthorized"
#define HTTP_402_PAYMENT_REQUIRED		"402 Payment Required"
#define HTTP_403_FORBIDDEN				"403 Forbidden"
#define HTTP_404_NOT_FOUND				"404 Not Found"
#define HTTP_405_METHOD_NOT_ALLOWED		"405 Method Not Allowed"
#define HTTP_406_NOT_ACCEPTABLE			"406 Not Acceptable"
#define HTTP_407_PROXY_AUTH_REQUIRED	"407 Proxy Authentication Required"
#define HTTP_408_REQUEST_TIMEOUT		"408 Request Timeout"
#define HTTP_409_CONFLICT				"409 Conflict"
#define HTTP_410_GONE					"410 Gone"

#define HTTP_500_INTERNAL_SERVER_ERROR	"500 Internal Server Error"
#define HTTP_501_NOT_IMPLEMENTED		"501 Not Implemented"
#define HTTP_502_BAD_GATEWAY			"502 Bad Gateway"
#define HTTP_503_SERVICE_UNAVAILABLE	"503 Service Unavailable"
#define HTTP_504_GATEWAY_TIMEOUT		"504 Gateway Timeout"
#define HTTP_505_HTTP_VERSION_NS		"505 HTTP Version Not Supported"

// Tokens importants du header HTTP

#define Token_Host						"Host:"
#define Token_Connection				"Connection:"
#define Token_ContentType				"Content-Type:"
#define Token_ContentLength				"Content-Length:"

// Tokens correspondant � un type MIME

#define Content_PlainText				"text/plain"
#define Content_HTML					"text/html"
#define Content_CSS						"text/css"
#define Content_URLForm					"application/x-www-form-urlencoded"
#define Content_Javascript				"application/javascript"
#define Content_Stream					"application/octet-stream"
#define Content_PDF						"application/pdf"
#define Content_XHTML					"application/xhtml+xml"
#define Content_JSON					"application/json"
#define Content_XML						"application/xml"
#define Content_ZIP						"application/zip"
#define Content_GIF						"image/gif"
#define Content_JPG						"image/jpg"
#define Content_PNG						"image/png"

//-------------------------------------------------------------------------------------------------

CWebContext::CWebContext(QTcpSocket* pSocket, QString sPeer, QString sHost, QStringList lPath, QMap<QString, QString> mArguments)
: m_pSocket(pSocket)
, m_sPeer(sPeer)
, m_sHost(sHost)
, m_lPath(lPath)
, m_mArguments(mArguments)
{
	LOG_DEBUG(QString("CWebContext::CWebContext()"));
}

//-------------------------------------------------------------------------------------------------

CWebContext::~CWebContext()
{
}

//-------------------------------------------------------------------------------------------------

CHTTPServer::CHTTPServer(quint16 port, QObject* parent)
: QTcpServer(parent)
, m_bDisabled(false)
, m_bFirstMessage(true)
{
	LOG_DEBUG("CHTTPServer::CHTTPServer()");

	// Remplissage du tableau des types MIME

	m_vExtensionToContentType["txt"]	= Content_PlainText;
	m_vExtensionToContentType["log"]	= Content_PlainText;
	m_vExtensionToContentType["html"]	= Content_HTML;
	m_vExtensionToContentType["css"]	= Content_CSS;
	m_vExtensionToContentType["js"]		= Content_Javascript;
	m_vExtensionToContentType["pdf"]	= Content_PDF;
	m_vExtensionToContentType["xhtml"]	= Content_XHTML;
	m_vExtensionToContentType["json"]	= Content_JSON;
	m_vExtensionToContentType["xml"]	= Content_XML;
	m_vExtensionToContentType["zip"]	= Content_ZIP;
	m_vExtensionToContentType["gif"]	= Content_GIF;
	m_vExtensionToContentType["jpg"]	= Content_JPG;
	m_vExtensionToContentType["jpeg"]	= Content_JPG;
	m_vExtensionToContentType["png"]	= Content_PNG;

	// Ecoute du port

	if (port > 0)
	{
		if (listen(QHostAddress::Any, port))
		{
			LOG_INFO(QString("Web server started on %1").arg(port));

			connect(this, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
		}
		else
		{
			LOG_ERROR(QString("CHTTPServer::CHTTPServer() : bind to %1 failed").arg(port));
		}
	}
}

//-------------------------------------------------------------------------------------------------

CHTTPServer::~CHTTPServer()
{
	// Fermeture du serveur
	close();
}

//-------------------------------------------------------------------------------------------------

void CHTTPServer::onNewConnection()
{
	LOG_DEBUG("CHTTPServer::onNewConnection()");

	// R�cup�ration socket entrante
	QTcpSocket* pSocket = nextPendingConnection();

	pSocket->setReadBufferSize(20000);

	// Cr�ation des donn�es utilisateurs
	// Pas besoin de stocker la valeur de retour, elle est plac�e dans les propri�t�s de la socket
	new CClientData(pSocket);
	
	// On r�initialise le flag de premier message
	m_bFirstMessage = true;

	// Connexion des signaux
	connect(pSocket, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
	connect(pSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(onSocketBytesWritten(qint64)));
	connect(pSocket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));
}

//-------------------------------------------------------------------------------------------------

void CHTTPServer::onSocketDisconnected()
{
	LOG_DEBUG("CHTTPServer::onSocketDisconnected()");

	// R�cup�ration de l'objet �metteur du signal
	QTcpSocket* pSocket = dynamic_cast<QTcpSocket*>(QObject::sender());

	// Appel du gestionnaire impl�ment� (ou non) par les sous-classes
	handleSocketDisconnection(pSocket);

	// Destruction des donn�es utilisateur associ�es � la socket
	CClientData::deleteFromSocket(pSocket);

	// La socket est d�truite en temps voulu par Qt
	pSocket->deleteLater();
}

//-------------------------------------------------------------------------------------------------

void CHTTPServer::onSocketReadyRead()
{
	// R�cup�ration de l'objet �metteur du signal
	QTcpSocket* pSocket = dynamic_cast<QTcpSocket*>(QObject::sender());

	// Est-ce que la socket est valide?
	if (pSocket != NULL)
	{
		// Si le serveur est d�sactiv�, on lib�re la socket et les donn�es utilisateur associ�es
		if (m_bDisabled)
		{
			CClientData::deleteFromSocket(pSocket);
			pSocket->disconnect();
			pSocket->deleteLater();
			return;
		}

		// R�cup�ration des donn�es utilisateurs associ�es � la socket
		CClientData* pData = CClientData::getFromSocket(pSocket);

		if (pData != NULL)
		{
			// Si la socket est en �tat connect�
			if (pSocket->state() == QTcpSocket::ConnectedState)
			{
				if (m_bFirstMessage)
				{
					// Lecture de toutes les donn�es disponibles
					QByteArray baData = pSocket->readAll();

					// Log de la requ�te
					LogRequest(pSocket->peerAddress().toString(), QString(baData));

					pData->m_baBuffer.append(baData);

					// Est-ce que l'ent�te a �t� lue?
					if (pData->m_bHeaderRead == false)
					{
						pData->m_bHeaderRead = true;
						pData->m_iExpectedBytes = getExpectedBytes(baData);
					}

					// D�cr�mentation du nombre d'octets attendus
					pData->m_iExpectedBytes -= baData.count();

					if (pData->m_iExpectedBytes <= 0)
					{
						// L'ent�te n'a pas �t� lue
						pData->m_bHeaderRead = false;

						// On ex�cute la requ�te du client
						processRequest(pSocket);

						m_bFirstMessage = false;
					}
				}
				else
				{
					// Appel du gestionnaire impl�ment� (ou non) par les sous-classes
					handleSocketBytesRead(pSocket);
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CHTTPServer::onSocketBytesWritten(qint64 iBytes)
{
	// R�cup�ration de l'objet �metteur du signal
	QTcpSocket* pSocket = dynamic_cast<QTcpSocket*>(QObject::sender());

	// Appel du gestionnaire impl�ment� (ou non) par les sous-classes
	handleSocketBytesWritten(pSocket, iBytes);
}

//-------------------------------------------------------------------------------------------------

int CHTTPServer::getExpectedBytes(QByteArray baData)
{
	int iExpectedBytes = 0;
	QString sText = baData;

	// R�cup�ration des tokens
	QStringList lTokens = sText.split(QRegExp("[ \n][ \n]*"));

	if (lTokens[0] == "POST")
	{
		// R�cup�ration du type de contenu
		QString sContentType = getTokenValue(lTokens, Token_ContentType);

		// R�cup�ration du type de contenu
		QString sContentLength = getTokenValue(lTokens, Token_ContentLength);

		if (sContentLength.isEmpty() == false)
		{
			iExpectedBytes = sContentLength.toInt();
		}
	}

	return iExpectedBytes;
}

//-------------------------------------------------------------------------------------------------

void CHTTPServer::processRequest(QTcpSocket* pSocket)
{
	CClientData* pData = CClientData::getFromSocket(pSocket);

	if (pData != NULL)
	{
		bool bKeepAlive = false;
		QString sText = pData->m_baBuffer;

		// R�cup�ration des tokens du header HTTP
		QStringList lTokens = sText.split(QRegExp("[ \r\n][ \r\n]*"));

		QStringList lPath;						// Chemin demand� par le client (route)
		QMap<QString, QString> mArguments;		// Arguments pass� dans l'URI
		QString sHost;							// Nom de l'h�te (serveur, � savoir nous-m�me)
		QString sContentType;					// Type de contenu en cas de POST
		QByteArray baPostContent;				// Contenu en cas de POST
		bool bForceKeepAlive = false;

		// On traite si on a au moins un token
		if (lTokens.count() > 0)
		{
			// Pour l'instant, seul GET et POST son trait�s
			if (lTokens[0] == "GET" || lTokens[0] == "POST")
			{
				// GET Example :
				//-----------------------------------------------------------
				// GET /drivers HTTP/1.1
				// Host: 127.0.0.1:9991
				// Connection: keep-alive
				// Cache-Control: max-age=0
				// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
				// User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.153 Safari/537.36
				// Referer: http://127.0.0.1:9991/drivers
				// Accept-Encoding: gzip,deflate,sdch
				// Accept-Language: fr-FR,fr;q=0.8,en-US;q=0.6,en;q=0.4
				//-----------------------------------------------------------

				// POST Example :
				//-----------------------------
				// POST /drivers HTTP/1.1
				// Host: 127.0.0.1:9991
				// Connection: keep-alive
				// Cache-Control: max-age=0
				// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
				// User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.153 Safari/537.36
				// Referer: http://127.0.0.1:9991/drivers
				// Accept-Encoding: gzip,deflate,sdch
				// Accept-Language: fr-FR,fr;q=0.8,en-US;q=0.6,en;q=0.4
				// Content-Type: application/x-www-form-urlencoded
				// Content-Length: 30
				// username=zurfyx&pass=password
				//-----------------------------------------------------------

				// Est-ce qu'on a plus d'un token?
				if (lTokens.count() > 1)
				{
					// R�cup�ration du chemin demand� et des arguments
					getRequestPathAndArgs(lTokens, lPath, mArguments);

					// R�cup�ration de l'h�te
					sHost = getTokenValue(lTokens, Token_Host);

					QString sKeepAlive = getTokenValue(lTokens, Token_Connection);

					// Le client a t-il demand� une connexion permanente?
					if (sKeepAlive.toLower() == "keep-alive")
					{
						bKeepAlive = true;
					}

					// R�cup�ration du type de contenu
					sContentType = getTokenValue(lTokens, Token_ContentType);

					if (sContentType.isEmpty() == false)
					{
						getRequestPostContent(sText, baPostContent);
					}
				}

				// Cr�ation d'un contexte Web pour qu'une sous-classe puisse g�n�rer du contenu
				CWebContext tContext(pSocket, pSocket->peerAddress().toString(), sHost, lPath, mArguments);

				if (sContentType == Content_URLForm)
				{
					QStringList lArgs = QString(baPostContent).split("&");

					foreach(QString sArg, lArgs)
					{
						QStringList lCurrentArg = sArg.split("=");

						if (lCurrentArg.count() > 1)
						{
							tContext.m_mArguments[lCurrentArg[0]] = lCurrentArg[1];
						}
					}
				}

				// Assignation du contenu POST du contexte
				tContext.m_baPostContent = baPostContent;

				// Si un contenu non-dynamique (fichier) est trouv�, il est enovy� au client
				// Sinon, la sous-classe g�n�rera du contenu dynamique
				if (getResponseFile(tContext, pSocket) == true)
				{
				}
				else
				{
					bForceKeepAlive = getResponseDynamicContent(tContext, pSocket);
				}
			}
			else
			{
				LOG_DEBUG(QString("CHTTPServer::processRequest() : First token is not a GET"));
			}
		}
		else
		{
			LOG_WARNING(QString("CHTTPServer::processRequest() : No token to read"));
		}

		LOG_DEBUG(QString("CHTTPServer::processRequest() : Force keep alive = %1").arg(bForceKeepAlive));

		// On vide le buffer d'entr�e dans les donn�es utilisateur associ�es � la socket
		pData->m_baBuffer.clear();

		// Si la socket est toujours connect�e...
		if (pSocket->state() == QAbstractSocket::ConnectedState)
		{
			// On vide les buffers de sortie de la socket
			pSocket->flush();

			// Si la connexion ne doit pas �tre permanente,
			// On lib�re la socket et les donn�es utilisateur associ�es
			if (bKeepAlive == false && bForceKeepAlive == false)
			{
				CClientData::deleteFromSocket(pSocket);
				pSocket->disconnect();
				pSocket->deleteLater();
			}
		}
		else
		{
			LOG_WARNING(QString("CHTTPServer::processRequest() : Socket is not in connected state"));
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CHTTPServer::getRequestPathAndArgs(const QStringList& lTokens, QStringList& lPath, QMap<QString, QString>& mArguments)
{
	// Le chemin suit directement le token GET ou POST

	if (lTokens.count() > 1)
	{
		QStringList lPathAndArgs = lTokens[1].split("?");

		// Un split permet de s�parer chaque composant du chemin

		if (lPathAndArgs.count() > 0)
		{
			lPath = lPathAndArgs[0].split("/", QString::SkipEmptyParts);
		}

		if (lPathAndArgs.count() > 1)
		{
			// R�cup�ration des arguments

			QStringList lArgs = lPathAndArgs[1].split("&");

			foreach(QString sArg, lArgs)
			{
				QStringList lCurrentArg = sArg.split("=");

				if (lCurrentArg.count() > 1)
				{
					mArguments[lCurrentArg[0]] = lCurrentArg[1];
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CHTTPServer::getRequestPostContent(const QString& sText, QByteArray& baContent)
{
	// Pour connaitre la fin du contenu POST,
	// on cherche un double passage � la ligne, � savoir soit "\n\n", soit "\r\n\r\n"

	if (sText.contains(Token_ContentLength))
	{
		int iPos = sText.indexOf("\n\n");

		if (iPos > 0)
		{
			iPos += 2;
		}
		else
		{
			iPos = sText.indexOf("\r\n\r\n");

			if (iPos > 0)
			{
				iPos += 4;
			}
		}

		if (iPos > 0)
		{
			QString sContent = sText.mid(iPos);
			baContent = sContent.toLatin1();
		}
	}
}

//-------------------------------------------------------------------------------------------------

QString CHTTPServer::getTokenValue(const QStringList& lTokens, QString sTokenName)
{
	QString sReturnValue;

	if (lTokens.contains(sTokenName, Qt::CaseInsensitive))
	{
		bool bGotToken = false;

		foreach(QString sCurrentToken, lTokens)
		{
			if (bGotToken == true)
			{
				sReturnValue = sCurrentToken;
				break;
			}
			else if (sCurrentToken.contains(sTokenName, Qt::CaseInsensitive))
			{
				bGotToken = true;
			}
		}
	}

	return sReturnValue;
}

//-------------------------------------------------------------------------------------------------

QString CHTTPServer::getContentTypeByExtension(const QString& sExtention) const
{
	QString sLowerExtention = sExtention.toLower();

	if (m_vExtensionToContentType.contains(sLowerExtention))
	{
		return m_vExtensionToContentType[sLowerExtention];
	}

	return Content_Stream;
}

//-------------------------------------------------------------------------------------------------

void CHTTPServer::addAuthorizedFolder(QString sFolderName)
{
	if (m_vAuthorizedFolders.contains(sFolderName) == false)
	{
		m_vAuthorizedFolders.append(sFolderName);
	}
}

//-------------------------------------------------------------------------------------------------

void CHTTPServer::pause()
{
	m_bDisabled = true;
}

//-------------------------------------------------------------------------------------------------

void CHTTPServer::resume()
{
	m_bDisabled = false;
}

//-------------------------------------------------------------------------------------------------

bool CHTTPServer::getResponseFile(const CWebContext& tContext, QTcpSocket* pSocket)
{
	// On ne traite que si la socket est en �tat connect�
	if (pSocket->state() == QAbstractSocket::ConnectedState)
	{
		if (tContext.m_lPath.count() > 0)
		{
			bool bAuthorized = false;

			// V�rification du droit d'acc�s au r�pertoire
			foreach(QString sAuthorizedFolder, m_vAuthorizedFolders)
			{
				if (sAuthorizedFolder == tContext.m_lPath[0])
				{
					bAuthorized = true;
					break;
				}
			}

			// Composition du nom de fichier sur disque
			QString sFileName = QCoreApplication::applicationDirPath();

			foreach(QString sPathName, tContext.m_lPath)
			{
				sFileName += "/" + sPathName;
			}

			// Est-ce que le fichier existe sur disque?
			if (QFile::exists(sFileName))
			{
				// Est-ce que son acc�s est autoris�
				if (bAuthorized)
				{
					QFile tFile(sFileName);

					// On ouvre le fichier est on l'envoie apr�s un header de r�ponse HTTP
					if (tFile.open(QIODevice::ReadOnly))
					{
						QString sType = getContentTypeByExtension(sFileName.split(".").last());

						QByteArray baData;

						baData.append(HTTP_HEADER);
						baData.append(HTTP_200_OK);
						baData.append("\r\n");
						baData.append(Token_ContentType);
						baData.append(" ");
						baData.append(sType.toLatin1());
						baData.append("; charset=\"utf-8\"");
						baData.append("\r\n");
						baData.append(Token_ContentLength);
						baData.append(QString(" %1").arg(tFile.size()).toLatin1());
						baData.append("\r\n");
						baData.append("\r\n");
						baData.append(tFile.readAll());

						pSocket->write(baData);

						tFile.close();

						return true;
					}
				}
				else
				{
					// On envoie au client une r�ponse d'erreur : acc�s interdit
					QByteArray baHTML;
					QByteArray baData;

					baHTML.append("<!doctype html>\r\n");
					baHTML.append("<html>\r\n");
					baHTML.append("<body>\r\n");
					baHTML.append(QString("%1").arg(HTTP_403_FORBIDDEN));
					baHTML.append("</body>\r\n");
					baHTML.append("</html>\r\n");

					baData.append(HTTP_HEADER);
					baData.append(HTTP_403_FORBIDDEN);
					baData.append("\r\n");
					baData.append(Token_ContentType);
					baData.append(" ");
					baData.append(Content_HTML);
					baData.append("; charset=\"utf-8\"");
					baData.append("\r\n");
					baData.append(Token_ContentLength);
					baData.append(QString(" %1").arg(baHTML.count()));
					baData.append("\r\n");
					baData.append("\r\n");
					baData.append(baHTML);

					pSocket->write(baData);

					return true;
				}
			}
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------

bool CHTTPServer::getResponseDynamicContent(const CWebContext& tContext, QTcpSocket* pSocket)
{
	QString sHead;
	QString sBody;
	QString sXMLResponse;
	QString sCustomResponse;

	// Appel de la m�thode virtuelle pour remplir la page
	getContent(tContext, sHead, sBody, sXMLResponse, sCustomResponse);

	// On ne traite que si la socket est en �tat connect�
	if (pSocket->state() == QAbstractSocket::ConnectedState)
	{
		// Au cas o� la r�ponse customis�e est non-vide, on l'envoie tel quel au client
		if (sCustomResponse.isEmpty() == false)
		{
			QByteArray baData;

			baData.append(HTTP_HEADER);
			baData.append(HTTP_200_OK);
			baData.append(sCustomResponse);

			pSocket->write(baData);

			return true;
		}
		// Sinon, si la r�ponse au format XML est non-vide, c'est elle qu'on envoie au client
		else if (sXMLResponse.isEmpty() == false)
		{
			QByteArray baData;

			baData.append(HTTP_HEADER);
			baData.append(HTTP_200_OK);
			baData.append("\r\n");
			baData.append(Token_ContentType);
			baData.append(QString(" %1; charset=\"utf-8\"").arg(Content_XML));
			baData.append("\r\n");
			baData.append(Token_ContentLength);
			baData.append(QString(" %1").arg(sXMLResponse.size()));
			baData.append("\r\n");
			baData.append("\r\n");
			baData.append(sXMLResponse);

			pSocket->write(baData);
		}
		// Sinon, les contenus HTML de sHead et sBody sont retourn�s au client
		else
		{
			// Streaming de la page

			QByteArray baHTML;
			QByteArray baData;

			baHTML.append("<!doctype html>\r\n");
			baHTML.append("<html>\r\n");
			baHTML.append("<head>\r\n");
			baHTML.append(sHead);
			baHTML.append("</head>\r\n");
			baHTML.append("<body>\r\n");
			baHTML.append(sBody);
			baHTML.append("</body>\r\n");
			baHTML.append("</html>\r\n");

			baData.append(HTTP_HEADER);
			baData.append(HTTP_200_OK);
			baData.append("\r\n");
			baData.append(Token_ContentType);
			baData.append(QString(" %1; charset=\"utf-8\"").arg(Content_HTML));
			baData.append("\r\n");
			baData.append(Token_ContentLength);
			baData.append(QString(" %1").arg(baHTML.count()));
			baData.append("\r\n");
			baData.append("\r\n");
			baData.append(baHTML);

			pSocket->write(baData);
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------

void CHTTPServer::getContent(const CWebContext& tContext, QString& sHead, QString& sBody, QString& xmlResponse, QString& sCustomResponse)
{
	// Cette m�thode est � impl�menter par une sous-classe pour g�n�rer du contenu dynamique
	// Soit en remplissant sCustomResponse par du texte quelconque
	// Soit en en remplissant xmlResponse par du contenu XML
	// Soit en remplissant sHead et sBody par du contenu HTML
}

//-------------------------------------------------------------------------------------------------

void CHTTPServer::handleSocketDisconnection(QTcpSocket* pSocket)
{
	// Cette m�thode peut �tre impl�ment�e par une sous-classe pour r�agir
	// lors de la d�connexion d'un client
}

//-------------------------------------------------------------------------------------------------

void CHTTPServer::handleSocketBytesWritten(QTcpSocket* pSocket, qint64 iBytes)
{
	// Cette m�thode peut �tre impl�ment�e par une sous-classe pour r�agir
	// lorsque des octets sont envoy�s � un client
}

void CHTTPServer::handleSocketBytesRead(QTcpSocket* pSocket)
{
	// Cette m�thode peut �tre impl�ment�e par une sous-classe pour r�agir
	// lorsque des octets sont envoy�s � un client
}

//-------------------------------------------------------------------------------------------------

QString CHTTPServer::encodeURLParameters(QString sText)
{
	// TODO : A impl�menter si n�cessaire

	return sText;
}

//-------------------------------------------------------------------------------------------------

QString CHTTPServer::decodeURLParameters(QString sText)
{
	return sText
		.replace("%20", " ")
		.replace("%0A", "\r")
		.replace("%0D", "\n")
		.replace("%3F", "?")
		.replace("%2A", "*");
}

//-------------------------------------------------------------------------------------------------

void CHTTPServer::LogRequest(QString sIP, QString sText)
{
	// R�cup�ration des tokens
	QStringList lTokens = sText.split(QRegExp("[ \n][ \n]*"));

	if (lTokens.count() > 1)
	{
		LOG_INFO(QString("%1 : %2 : %3")
			.arg(sIP)
			.arg(lTokens[0])
			.arg(lTokens[1])
			);
	}
}
