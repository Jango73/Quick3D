
// Qt
#include <QMetaObject>
#include <QVariant>

// qt-plus
#include "CLogger.h"

// Application
#include "CRemoteControlServer.h"

//-------------------------------------------------------------------------------------------------

/*!
    Builds a remote control server for \a pHandler on \a uiPort. \br\br
    \a pParent is stored as the Qt parent of this object. The handler must
    live in the user interface thread.
*/
CRemoteControlServer::CRemoteControlServer(CRemoteControlHandler* pHandler, quint16 uiPort, QObject* pParent)
    : CHTTPServer(uiPort, pParent)
    , m_pHandler(pHandler)
{
    useFloodProtection(false);

    if (isListening() == true)
    {
        LOG_METHOD_INFO(QString("Remote control serves on port %1").arg(uiPort));
    }
    else
    {
        LOG_ERROR(QString("Remote control cannot listen on port %1").arg(uiPort));
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys a remote control server.
*/
CRemoteControlServer::~CRemoteControlServer()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns \c true when the server listens for connections.
*/
bool CRemoteControlServer::isListening() const
{
    return QTcpServer::isListening();
}

//-------------------------------------------------------------------------------------------------

/*!
    Serves the remote control endpoints, runs on a request thread. \br\br
    /key injects a key event, /vehicle returns a vehicle status document,
    /select selects the active vehicle, /view selects the active camera,
    any other path returns a help text.
    Results travel as \a sCustomResponse with a JSON MIME type.
*/
void CRemoteControlServer::getContent(CWebContext& tContext, QString& sHead, QString& sBody, QString& sCustomResponse, QString& sCustomResponseMIME)
{
    Q_UNUSED(sHead);
    Q_UNUSED(sBody);

    sCustomResponseMIME = MIME_Content_JSON;

    if (tContext.m_lPath.count() == 0)
    {
        sCustomResponse = "{\"ok\":false,\"error\":\"empty path\"}";
        return;
    }

    QString sEndpoint = tContext.m_lPath[0].toLower();

    if (sEndpoint == "key")
    {
        serveKey(tContext.m_mArguments, sCustomResponse);
    }
    else if (sEndpoint == "vehicle")
    {
        serveVehicle(tContext.m_mArguments, sCustomResponse);
    }
    else if (sEndpoint == "select")
    {
        serveSelect(tContext.m_mArguments, sCustomResponse);
    }
    else if (sEndpoint == "view")
    {
        serveView(tContext.m_mArguments, sCustomResponse);
    }
    else
    {
        sCustomResponseMIME = "text/plain";
        sCustomResponse = "Quick3D remote control: /key?code=H&action=press|release|click /vehicle?name=Visitor2 /select?name=Visitor2 /view?name=Driver";
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Converts a key name (letter, Key_X, Space, Escape, Return) to a Qt::Key value, -1 when unknown.
*/
int CRemoteControlServer::keyFromName(const QString& sName) const
{
    QString sCode = sName.trimmed();

    if (sCode.startsWith("Key_") == true)
    {
        sCode = sCode.mid(4);
    }

    if (sCode.compare("Space", Qt::CaseInsensitive) == 0)
    {
        return Qt::Key_Space;
    }

    if (sCode.compare("Escape", Qt::CaseInsensitive) == 0)
    {
        return Qt::Key_Escape;
    }

    if (sCode.compare("Return", Qt::CaseInsensitive) == 0)
    {
        return Qt::Key_Return;
    }

    if (sCode.length() == 1)
    {
        QChar cUpper = sCode[0].toUpper();

        if (cUpper >= 'A' && cUpper <= 'Z')
        {
            return int(Qt::Key_A) + (cUpper.unicode() - 'A');
        }

        if (cUpper >= '0' && cUpper <= '9')
        {
            return int(Qt::Key_0) + (cUpper.unicode() - '0');
        }
    }

    return -1;
}

//-------------------------------------------------------------------------------------------------

/*!
    Converts an action name (press, release, click) to an action code, -1 when unknown.
*/
int CRemoteControlServer::actionFromName(const QString& sName) const
{
    QString sAction = sName.trimmed().toLower();

    if (sAction.isEmpty() == true || sAction == "click")
    {
        return 2;
    }

    if (sAction == "press" || sAction == "down")
    {
        return 0;
    }

    if (sAction == "release" || sAction == "up")
    {
        return 1;
    }

    return -1;
}

//-------------------------------------------------------------------------------------------------

/*!
    Serves /key from \a mArguments, result as JSON in \a sResponse.
*/
void CRemoteControlServer::serveKey(const QMap<QString, QString>& mArguments, QString& sResponse)
{
    int iKey = keyFromName(mArguments.value("code"));
    int iAction = actionFromName(mArguments.value("action"));

    if (iKey < 0 || iAction < 0 || m_pHandler.isNull() == true)
    {
        sResponse = "{\"ok\":false,\"error\":\"unknown key, action or missing handler\"}";
        return;
    }

    bool bResult = false;

    QMetaObject::invokeMethod(m_pHandler, "sendKey", Qt::BlockingQueuedConnection,
        Q_RETURN_ARG(bool, bResult),
        Q_ARG(int, iKey),
        Q_ARG(int, iAction));

    sResponse = bResult == true ? "{\"ok\":true}" : "{\"ok\":false,\"error\":\"key rejected\"}";
}

//-------------------------------------------------------------------------------------------------

/*!
    Serves /vehicle from \a mArguments, result as JSON in \a sResponse.
*/
void CRemoteControlServer::serveVehicle(const QMap<QString, QString>& mArguments, QString& sResponse)
{
    QString sName;

    if (requireHandlerName(mArguments, sName, sResponse) == false)
    {
        return;
    }

    QString sStatus;

    QMetaObject::invokeMethod(m_pHandler, "vehicleStatus", Qt::BlockingQueuedConnection,
        Q_RETURN_ARG(QString, sStatus),
        Q_ARG(QString, sName));

    sResponse = sStatus.isEmpty() == true ? "{\"ok\":false,\"error\":\"no status\"}" : sStatus;
}

//-------------------------------------------------------------------------------------------------

/*!
    Serves /select from \a mArguments, result as JSON in \a sResponse.
*/
void CRemoteControlServer::serveSelect(const QMap<QString, QString>& mArguments, QString& sResponse)
{
    QString sName;

    if (requireHandlerName(mArguments, sName, sResponse) == false)
    {
        return;
    }

    invokeHandlerBool("selectVehicle", sName, sResponse, "vehicle not selected");
}

//-------------------------------------------------------------------------------------------------

/*!
    Serves /view from \a mArguments, result as JSON in \a sResponse.
*/
void CRemoteControlServer::serveView(const QMap<QString, QString>& mArguments, QString& sResponse)
{
    QString sName;

    if (requireHandlerName(mArguments, sName, sResponse) == false)
    {
        return;
    }

    invokeHandlerBool("selectView", sName, sResponse, "view not selected");
}

//-------------------------------------------------------------------------------------------------

/*!
    Checks the name argument and the handler, false with \a sResponse set to the error envelope
*/
bool CRemoteControlServer::requireHandlerName(const QMap<QString, QString>& mArguments, QString& sName, QString& sResponse) const
{
    sName = mArguments.value("name");

    if (sName.isEmpty() == true || m_pHandler.isNull() == true)
    {
        sResponse = "{\"ok\":false,\"error\":\"missing name or handler\"}";
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

/*!
    Invokes the bool handler \a pMethod with \a sName, writes the ok envelope to \a sResponse
*/
bool CRemoteControlServer::invokeHandlerBool(const char* pMethod, const QString& sName, QString& sResponse, const char* pError) const
{
    bool bResult = false;

    QMetaObject::invokeMethod(m_pHandler, pMethod, Qt::BlockingQueuedConnection,
        Q_RETURN_ARG(bool, bResult),
        Q_ARG(QString, sName));

    sResponse = bResult == true ? "{\"ok\":true}" : QString("{\"ok\":false,\"error\":\"%1\"}").arg(pError);

    return bResult;
}
