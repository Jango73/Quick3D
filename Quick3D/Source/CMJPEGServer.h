
#pragma once

#include "quick3d_global.h"

// Qt
#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include <QFile>

// Application
#include "CHTTPServer.h"

//-------------------------------------------------------------------------------------------------
// Déclarations avancées
// Forward declarations

class CMJPEGServer;

//-------------------------------------------------------------------------------------------------

class CMJPEGThread : public QThread
{
    Q_OBJECT

    friend class CMJPEGServer;

public:

    //! Constructeur
    CMJPEGThread(CMJPEGServer* pParent);

    //! Destructor
    virtual ~CMJPEGThread();

    virtual void run();

protected:

    CMJPEGServer*	m_pParent;
    bool			m_bRun;
};

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CMJPEGServer : public CHTTPServer
{
    Q_OBJECT

    friend class CMJPEGThread;

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Default constructor
    CMJPEGServer(int iPort);

    //! Constructeur pour sortie sur fichier
    CMJPEGServer(QString sFileName);

    //! Destructor
    virtual ~CMJPEGServer();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void send(const QByteArray& baData);

    //!
    void sendRaw(const QByteArray& baData, int iWidth, int iHeight);

    //!
    void sendImage(const QImage& image);

    //!
    bool hasConnections() { return m_vSockets.count() > 0; }

    //!
    void flush();

    //!
    virtual void getContent(const CWebContext& tContext, QString& sHead, QString& sBody, QString& xmlResponse, QString& sCustomResponse);

    //!
    virtual void handleSocketBytesWritten(QTcpSocket* pSocket, qint64 iBytes);

    //!
    virtual void handleSocketDisconnection(QTcpSocket* pSocket);

    //-------------------------------------------------------------------------------------------------
    // Static methods
    //-------------------------------------------------------------------------------------------------

    //!
    static void saveMJPEG(const QVector<QImage>& vImages, QString sFileName);

    //-------------------------------------------------------------------------------------------------
    // Slots
    //-------------------------------------------------------------------------------------------------

protected slots:

    void onTimeout();

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    void processOutputImages();

    //!
    static QString getHeader();

    //!
    static QString getImageDescriptor(int iSize);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QTimer					m_tTimer;
    QMutex					m_tMutex;
    QString					m_sFileName;
    QFile*					m_pOutputFile;
    QVector<QByteArray>		m_vOutput;
    QVector<QImage>			m_vOutputImages;
    QVector<QTcpSocket*>	m_vSockets;
    CMJPEGThread*			m_pThread;
    int						m_iCompressionRate;
};
