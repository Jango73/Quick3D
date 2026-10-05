
// qt-plus
#include "CLogger.h"

// Application
#include "CWorldTerrainMap.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

#define MAX_ALTITUDE 5000.0

//-------------------------------------------------------------------------------------------------

CComponent* CWorldTerrainMap::instantiator(C3DScene* pScene)
{
    return new CWorldTerrainMap(pScene);
}

//-------------------------------------------------------------------------------------------------

CWorldTerrainMap::CWorldTerrainMap(C3DScene *pScene)
    : CComponent(pScene)
    , m_pImage(nullptr)
    , m_sImageSize(QSize(128, 64))
    , m_dScale(1.0)
{
    m_iColors.addValue(0.000, CVector3(0.0, 0.0, 0.0));
    m_iColors.addValue(0.001, CVector3(0.0, 1.0, 0.5));
    m_iColors.addValue(0.150, CVector3(0.0, 1.0, 0.0));
    m_iColors.addValue(0.300, CVector3(1.0, 0.5, 0.0));
    m_iColors.addValue(1.000, CVector3(1.0, 1.0, 1.0));
}

//-------------------------------------------------------------------------------------------------

CWorldTerrainMap::~CWorldTerrainMap()
{
    m_pTerrain.reset();

    if (m_pImage != nullptr)
        delete m_pImage;
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

void CWorldTerrainMap::setCenter(CGeoloc gCenter)
{
    m_gCenter = gCenter;
}

//-------------------------------------------------------------------------------------------------

void CWorldTerrainMap::setScale(double dScale)
{
    m_dScale = dScale;
}

//-------------------------------------------------------------------------------------------------

const QImage& CWorldTerrainMap::image() const
{
    return *m_pImage;
}

//-------------------------------------------------------------------------------------------------

void CWorldTerrainMap::loadParameters(const QString& sBaseFile, const CXMLNode& xComponent)
{
    CComponent::loadParameters(sBaseFile, xComponent);
}

//-------------------------------------------------------------------------------------------------

void CWorldTerrainMap::updateImage()
{
    if (m_pTerrain != nullptr && m_pTerrain->heights() != nullptr)
    {
        if (m_pImage != nullptr)
        {
            delete m_pImage;
        }

        m_pImage = new QImage(m_sImageSize, QImage::Format_RGB888);

        for (int y = 0; y < m_sImageSize.height(); y++)
        {
            for (int x = 0; x < m_sImageSize.width(); x++)
            {
                double dLatitude = (((1.0 - ((double) y / (double) m_sImageSize.height())) - 0.5) * 2.0) * Math::DegreeQuarter;
                double dLongitude = (((((double) x / (double) m_sImageSize.width())) - 0.5) * 2.0) * Math::DegreeHalf;

                dLatitude /= m_dScale;
                dLongitude /= m_dScale;

                dLatitude += m_gCenter.Latitude;
                dLongitude += m_gCenter.Longitude;

                // Coordinates are clipped between -180.0 and +180.0 degrees
                dLatitude = Math::Angles::clipAngleDegreePIMinusPI(dLatitude);
                dLongitude = Math::Angles::clipAngleDegreePIMinusPI(dLongitude);

                if (dLatitude >= -Math::DegreeQuarter && dLatitude <= Math::DegreeQuarter && dLongitude >= -Math::DegreeHalf && dLongitude <= Math::DegreeHalf)
                {
                    // Get an altitude
                    CGeoloc gGeoloc(dLatitude, dLongitude, 0.0);
                    double dAltitude = m_pTerrain->heights()->getHeightAt(gGeoloc);
                    dAltitude = Angles::clipDouble(dAltitude / MAX_ALTITUDE, 0.0, 1.0);

                    // Get a color
                    CVector3 vColor = m_iColors.getValue(dAltitude);
                    QRgb iNewPixel = qRgb((int) (vColor.X * 255.0), (int) (vColor.Y * 255.0), (int) (vColor.Z * 255.0));

                    m_pImage->setPixel(QPoint(x, y), iNewPixel);
                }
                else
                {
                    m_pImage->setPixel(QPoint(x, y), qRgb(0, 0, 0));
                }
            }
        }
    }
}
