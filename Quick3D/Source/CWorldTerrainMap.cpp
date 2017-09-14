
// qt-plus
#include "CLogger.h"

// Application
#include "CWorldTerrainMap.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

#define MAX_ALTITUDE 20000.0

//-------------------------------------------------------------------------------------------------

CComponent* CWorldTerrainMap::instantiator(C3DScene* pScene)
{
    return new CWorldTerrainMap(pScene);
}

//-------------------------------------------------------------------------------------------------

CWorldTerrainMap::CWorldTerrainMap(C3DScene *pScene)
    : CComponent(pScene)
    , m_pImage(nullptr)
    , m_sImageSize(QSize(512, 256))
{
}

//-------------------------------------------------------------------------------------------------

CWorldTerrainMap::~CWorldTerrainMap()
{
    m_pTerrain.reset();
}

//-------------------------------------------------------------------------------------------------

void CWorldTerrainMap::setTerrain(QSP<CWorldTerrain> pTerrain)
{
    m_pTerrain = pTerrain;
}

//-------------------------------------------------------------------------------------------------

void CWorldTerrainMap::setImageSize(QSize sSize)
{
    m_sImageSize = sSize;
}

//-------------------------------------------------------------------------------------------------

const QImage& CWorldTerrainMap::image() const
{
    return *m_pImage;
}

//-------------------------------------------------------------------------------------------------

void CWorldTerrainMap::loadParameters(const QString& sBaseFile, CXMLNode xComponent)
{
    CComponent::loadParameters(sBaseFile, xComponent);
}

//-------------------------------------------------------------------------------------------------

void CWorldTerrainMap::updateImage()
{
    if (m_pTerrain != nullptr)
    {
        if (m_pImage != nullptr)
        {
            delete m_pImage;
            m_pImage = new QImage(m_sImageSize, QImage::Format_RGB888);
        }

        for (int y = 0; y < m_sImageSize.height(); y++)
        {
            for (int x = 0; x < m_sImageSize.width(); x++)
            {
                double dLatitude = (((1.0 - (y / m_sImageSize.height())) - 0.5) * 2.0) * 90.0;
                double dLongitude = (((1.0 - (x / m_sImageSize.width())) - 0.5) * 2.0) * 180.0;
                CGeoloc gGeoloc(dLatitude, dLongitude, 0.0);
                double dAltitude = m_pTerrain->getHeightAt(gGeoloc);
                dAltitude = Angles::clipDouble(dAltitude / MAX_ALTITUDE, 0.0, 1.0) * 255.0;
                QRgb iNewPixel = qRgb((int) dAltitude, (int) dAltitude, (int) dAltitude);
                m_pImage->setPixel(QPoint(x, y), iNewPixel);
            }
        }
    }
}
