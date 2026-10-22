
#pragma once

// qt-plus
#include "CLogger.h"
#include "Web/CHTTPServer.h"

// Qt
#include <QPointer>

// Application
#include "quick3d_global.h"
#include "CRemoteControlHandler.h"

//-------------------------------------------------------------------------------------------------

//! HTTP server driving the application through a remote control handler. \br\br
//! This class knows HTTP and JSON only, it never touches the scene or the
//! views directly: every action goes through the handler, which lives in the
//! user interface thread. Served endpoints are /key, /vehicle, /select, /view, /speed and /snapshot.
class QUICK3D_EXPORT CRemoteControlServer : public CHTTPServer
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructor, serves \a pHandler on \a uiPort
    CRemoteControlServer(CRemoteControlHandler* pHandler, quint16 uiPort, QObject* pParent = nullptr);

    //! Destructor
    virtual ~CRemoteControlServer() Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns \c true when the server listens for connections
    bool isListening() const;

    //-------------------------------------------------------------------------------------------------
    // Static methods
    //-------------------------------------------------------------------------------------------------

    //! Returns the success envelope as JSON through CXMLNode: {"ok":"true"}
    static QString okResponse();

    //! Returns the failure envelope as JSON through CXMLNode: {"ok":"false","error":"..."}
    static QString errorResponse(const QString& sError);

    //! Returns the failure envelope from a literal as JSON through CXMLNode
    static QString errorResponse(const char* pError);

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! Serves the remote control endpoints, runs on a request thread
    virtual void getContent(CWebContext& tContext, QString& sHead, QString& sBody, QString& sCustomResponse, QString& sCustomResponseMIME) Q_DECL_OVERRIDE;

protected:

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

    //! Converts a key name (letter, Key_X, Space, Escape, Return) to a Qt::Key value, -1 when unknown
    int keyFromName(const QString& sName) const;

    //! Converts an action name (press, release, click) to an action code, -1 when unknown
    int actionFromName(const QString& sName) const;

    //! Serves /key from \a mArguments, result as JSON in \a sResponse
    void serveKey(const QMap<QString, QString>& mArguments, QString& sResponse);

    //! Serves /vehicle from \a mArguments, result as JSON in \a sResponse
    void serveVehicle(const QMap<QString, QString>& mArguments, QString& sResponse);

    //! Serves /select from \a mArguments, result as JSON in \a sResponse
    void serveSelect(const QMap<QString, QString>& mArguments, QString& sResponse);

    //! Serves /view from \a mArguments, result as JSON in \a sResponse
    void serveView(const QMap<QString, QString>& mArguments, QString& sResponse);

    //! Serves /speed from \a mArguments, result as JSON in \a sResponse
    void serveSpeed(const QMap<QString, QString>& mArguments, QString& sResponse);

    //! Serves /snapshot, result as JSON in \a sResponse
    void serveSnapshot(QString& sResponse);

    //! Checks the handler, false with \a sResponse set to the error envelope
    bool requireHandler(QString& sResponse) const;

    //! Checks the name argument and the handler, false with \a sResponse set to the error envelope
    bool requireHandlerName(const QMap<QString, QString>& mArguments, QString& sName, QString& sResponse) const;

    //! Invokes the bool handler \a pMethod with \a sName, writes the ok envelope to \a sResponse
    bool invokeHandlerBool(const char* pMethod, const QString& sName, QString& sResponse, const char* pError) const;

    //! Invokes the string handler \a pMethod with \a sName, writes its payload or the error envelope to \a sResponse
    bool invokeHandlerString(const char* pMethod, const QString& sName, QString& sResponse, const char* pError) const;

    //! Invokes the string handler \a pMethod with no argument, writes its payload or the error envelope to \a sResponse
    bool invokeHandlerString(const char* pMethod, QString& sResponse, const char* pError) const;

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QPointer<CRemoteControlHandler>  m_pHandler;
};
