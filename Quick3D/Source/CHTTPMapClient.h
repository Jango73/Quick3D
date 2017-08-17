
#pragma once

// Qt
#include <QObject>
#include <QImage>
#include <QtNetwork>
#include <QNetworkAccessManager>

//-------------------------------------------------------------------------------------------------

class CHTTPMapClient : public QObject
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    // Constructeur
    CHTTPMapClient();

    // Destructeur
    virtual ~CHTTPMapClient();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void loadTile(QString sTileName);

    //!
    QImage getTile(QString sTileName);

protected:

    void loadNextTile();

protected slots:

    void httpFinished();
    void httpError(QNetworkReply::NetworkError code);
    void httpReadyRead();

signals:

    void tileReady(QString sTileName);

protected:

    QString                 m_sTilePath;
    QUrl                    m_uURL;
    QNetworkAccessManager   m_tNetMan;
    QNetworkReply*          m_pReply;
    QByteArray              m_baIncomingData;
    QString                 m_sCurrentTileName;
    QVector<QString>        m_vRequestedTiles;
    QMap<QString, QImage>   m_vLoadedTiles;
};
