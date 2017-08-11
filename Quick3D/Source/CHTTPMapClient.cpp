
// Foundations
#include "CLogger.h"

// Application
#include "CHTTPMapClient.h"

//-------------------------------------------------------------------------------------------------

CHTTPMapClient::CHTTPMapClient()
    : m_pReply(NULL)
{
    LOG_DEBUG("CHTTPMapClient::CHTTPMapClient()");

    m_sTilePath = QCoreApplication::applicationDirPath() + "/Tiles";

    if (!QDir().exists(m_sTilePath))
    {
        QDir().mkpath(m_sTilePath);
    }
}

//-------------------------------------------------------------------------------------------------

CHTTPMapClient::~CHTTPMapClient()
{
    LOG_DEBUG("CHTTPMapClient::~CHTTPMapClient()");
}

//-------------------------------------------------------------------------------------------------

void CHTTPMapClient::loadTile(QString sTileName)
{
    if (m_vRequestedTiles.contains(sTileName) == false)
    {
        m_vRequestedTiles.insert(0, sTileName);

        loadNextTile();
    }
}

//-------------------------------------------------------------------------------------------------

QImage CHTTPMapClient::getTile(QString sTileName)
{
    if (m_vLoadedTiles.contains(sTileName))
    {
        QImage imgReturnValue = m_vLoadedTiles[sTileName];

        m_vLoadedTiles.remove(sTileName);

        return imgReturnValue;
    }

    return QImage();
}

//-------------------------------------------------------------------------------------------------

void CHTTPMapClient::loadNextTile()
{
    if (m_pReply == NULL)
    {
        if (m_vRequestedTiles.count() > 0)
        {
            m_sCurrentTileName = m_vRequestedTiles[0];
            m_vRequestedTiles.remove(0);

            QString sFileName = m_sTilePath + "/" + m_sCurrentTileName + ".jpg";

            if (QFile::exists(sFileName))
            {
                QImage img;

                if (img.load(sFileName, "JPG"))
                {
                    m_vLoadedTiles[m_sCurrentTileName] = img;
                }

                emit tileReady(m_sCurrentTileName);

                loadNextTile();
            }
            else
            {
                m_uURL = QString("http://t0.tiles.virtualearth.net/tiles/%1.jpeg?g=1963&mkt={culture}&token={token}").arg(m_sCurrentTileName);

                m_pReply = m_tNetMan.get(QNetworkRequest(m_uURL));

                LOG_DEBUG(QString("CHTTPMapClient::loadNextTile() : requested tile %1").arg(m_sCurrentTileName));

                connect(m_pReply, SIGNAL(finished()), this, SLOT(httpFinished()));
                connect(m_pReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(httpError(QNetworkReply::NetworkError)));
                connect(m_pReply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CHTTPMapClient::httpFinished()
{
    if (m_pReply->error() == QNetworkReply::NoError)
    {
        if (m_baIncomingData.count() > 0)
        {
            QString sFileName = m_sTilePath + "/" + m_sCurrentTileName + ".jpg";

            QFile fImageFile(sFileName);

            if (fImageFile.open(QIODevice::WriteOnly))
            {
                fImageFile.write(m_baIncomingData);
                fImageFile.close();
            }

            LOG_DEBUG(QString("CHTTPMapClient::httpFinished() : downloaded tile %1").arg(m_sCurrentTileName));

            QImage image;

            if (image.loadFromData(m_baIncomingData, "JPG"))
            {
                m_vLoadedTiles[m_sCurrentTileName] = image.convertToFormat(QImage::Format_RGB888);
            }

            emit tileReady(m_sCurrentTileName);
        }
    }
    else
    {
        QNetworkReply::NetworkError err = m_pReply->error();

        Q_UNUSED(err);
    }

    m_baIncomingData.clear();

    m_pReply->deleteLater();
    m_pReply = NULL;

    loadNextTile();
}

//-------------------------------------------------------------------------------------------------

void CHTTPMapClient::httpError(QNetworkReply::NetworkError code)
{
    LOG_ERROR(QString("CHTTPMapClient::httpError() : tile %1").arg(m_sCurrentTileName));

    m_baIncomingData.clear();

    m_pReply->deleteLater();
    m_pReply = NULL;

    loadNextTile();
}

//-------------------------------------------------------------------------------------------------

void CHTTPMapClient::httpReadyRead()
{
    m_baIncomingData.append(m_pReply->readAll());
}
