
// qt-plus
#include "CLogger.h"

// Application
#include "CHTTPMapClient.h"
#include "CPreferencesManager.h"

//-------------------------------------------------------------------------------------------------

CHTTPMapClient::CHTTPMapClient()
    : m_pReply(nullptr)
{
    m_sTilePath = QCoreApplication::applicationDirPath() + "/Tiles";

    if (!QDir().exists(m_sTilePath))
    {
        QDir().mkpath(m_sTilePath);
    }

    CXMLNode xParameters = CPreferencesManager::getInstance()->preferences();
    CXMLNode xTiles = xParameters.getNodeByTagName(TILES_PARAM);
    CXMLNode xServerURL = xTiles.getNodeByTagName(TILES_PARAM_SERVER_URL);

    if (xServerURL.isEmpty() == false)
    {
        m_sTileServerURL = xServerURL.attributes()["Value"];
    }
}

//-------------------------------------------------------------------------------------------------

CHTTPMapClient::~CHTTPMapClient()
{
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
    if (m_pReply == nullptr)
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
                if (m_sTileServerURL.isEmpty() == false)
                {
                    m_uURL = QString(m_sTileServerURL).arg(m_sCurrentTileName);

                    m_pReply = m_tNetMan.get(QNetworkRequest(m_uURL));

                    LOG_METHOD_DEBUG(QString("Requesting tile %1").arg(m_sCurrentTileName));

                    connect(m_pReply, SIGNAL(finished()), this, SLOT(httpFinished()));
                    connect(m_pReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(httpError(QNetworkReply::NetworkError)));
                    connect(m_pReply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
                }
                else
                {
                    LOG_METHOD_ERROR(QString("Tile server URL undefined"));
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CHTTPMapClient::httpFinished()
{
    if (m_pReply != nullptr)
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

                LOG_METHOD_DEBUG(QString("Downloaded tile %1").arg(m_sCurrentTileName));

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
        m_pReply = nullptr;

        loadNextTile();
    }
}

//-------------------------------------------------------------------------------------------------

void CHTTPMapClient::httpError(QNetworkReply::NetworkError code)
{
    LOG_METHOD_DEBUG(QString("Tile %1").arg(m_sCurrentTileName));

    m_baIncomingData.clear();

    m_pReply->deleteLater();
    m_pReply = nullptr;

    loadNextTile();
}

//-------------------------------------------------------------------------------------------------

void CHTTPMapClient::httpReadyRead()
{
    m_baIncomingData.append(m_pReply->readAll());
}
