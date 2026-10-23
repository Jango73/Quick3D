
// qt-plus
#include "CLogger.h"

// Application
#include "C3DScene.h"
#include "CTiledMaterial.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

#define LAT_MAX  85.05112878

//-------------------------------------------------------------------------------------------------

CTiledMaterial::CTiledMaterial(C3DScene* pScene)
    : CMaterial(pScene)
    , m_iLevels(18)
{
    connect(&m_tClient, SIGNAL(tileReady(QString)), this, SLOT(onTileReady(QString)));
}

//-------------------------------------------------------------------------------------------------

CTiledMaterial::~CTiledMaterial()
{
}

//-------------------------------------------------------------------------------------------------

void CTiledMaterial::onTileReady(QString sTileName)
{
    if (m_mTiles.contains(sTileName))
    {
        // The entry may already hold a texture (retry after an eviction):
        // release it first, CTile owns its texture pointer
        if (m_mTiles[sTileName].m_pTexture != nullptr)
        {
            delete m_mTiles[sTileName].m_pTexture;
            m_mTiles[sTileName].m_pTexture = nullptr;
        }

        m_mTiles[sTileName].m_pTexture = new CTexture(m_pScene, sTileName, m_tClient.getTile(sTileName), QSize(256, 256), m_vDiffuseTextures.count());

        LOG_METHOD_DEBUG(QString("Created texture %1").arg(m_mTiles[sTileName].m_pTexture->glTexture()));
    }
}

//-------------------------------------------------------------------------------------------------

CVector2 CTiledMaterial::texCoords(const CGeoloc& gPosition, int iLevel)
{
    double dX = gPosition.Longitude / 180.0;
    double dY = gPosition.Latitude / 90.0;

    return CVector2(dX, dY);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Selects the tile quad key for the given geolocation and level: loads
    the tile when missing, marks it in use otherwise, then collects the
    tiles unused for a while. \br\br
    Runs during the render pass, before the terrain mesh of the chunk paints.
*/
/***************************************************************************/
void CTiledMaterial::prepareForTerrainPaint(const CGeoloc& gPosition, int iLevel)
{
    m_sCurrentQuadKey = quadKeyFromPositionAndLevel(gPosition, iLevel);

    if (quadKeyPresent(m_sCurrentQuadKey) == false)
    {
        m_mTiles[m_sCurrentQuadKey] = CTile(0, 0);
        m_tClient.loadTile(m_sCurrentQuadKey);
    }
    else
    {
        // Mark the tile in use before the garbage collection below:
        // the tile about to be painted must never be evicted here
        m_mTiles[m_sCurrentQuadKey].m_tLastUsed = QDateTime::currentDateTime();
    }

    collectGarbage();
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Binds the diffuse texture maps. \br\br
    The tile bound in applyDynamicState() is the only diffuse source:
    the base diffuse path stays disabled here.
*/
/***************************************************************************/
void CTiledMaterial::bindDiffuseMaps(CRenderContext* pContext, QOpenGLShaderProgram* pProgram)
{
    Q_UNUSED(pContext);

    // The tile bound in applyDynamicState() is the only diffuse source:
    // keep the base diffuse path disabled
    pProgram->setUniformValue("u_texture_diffuse_enable", (GLint) 0);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Binds the tile for the current quad key. \br\br
    The rebind runs on every call: the shader program and the texture units
    are shared with the other materials (bounded meshes bind their own
    textures between two terrain chunks), so skipping the bind when the quad
    key is unchanged would keep a foreign texture bound.
*/
/***************************************************************************/
void CTiledMaterial::applyDynamicState(CRenderContext* pContext, QOpenGLShaderProgram* pProgram)
{
    Q_UNUSED(pContext);

    pProgram->setUniformValue("u_texture_diffuse_enable", (GLint) 0);

    if (m_mTiles.contains(m_sCurrentQuadKey))
    {
        if (m_mTiles[m_sCurrentQuadKey].m_pTexture != nullptr)
        {
            m_mTiles[m_sCurrentQuadKey].m_tLastUsed = QDateTime::currentDateTime();
            m_mTiles[m_sCurrentQuadKey].m_pTexture->activate();

            pProgram->setUniformValue("u_texture_diffuse_enable", (GLint) 1);
            pProgram->setUniformValue("u_texture_diffuse_0", (GLint) DiffuseTextureUnitBase);
        }
    }
}

//-------------------------------------------------------------------------------------------------

CGeoloc CTiledMaterial::transformGeoloc(const CGeoloc& gPosition)
{
    CGeoloc gReturnValue;

    double dNormalized = gPosition.Latitude / (90.0 / Math::Pi); // 28.6512;
    double dFinal = atan(sinh(dNormalized));

    dFinal = Math::Angles::toDeg(dFinal);

    gReturnValue.Latitude = Math::Angles::clipDouble(dFinal, -LAT_MAX, LAT_MAX);
    gReturnValue.Longitude = gPosition.Longitude;
    gReturnValue.Altitude = gPosition.Altitude;

    return gReturnValue;
}

//-------------------------------------------------------------------------------------------------

QString CTiledMaterial::quadKeyFromPositionAndLevel(const CGeoloc& gPosition, int iLevel)
{
    int iBingLevel = m_iLevels - iLevel;

    double dLatitude = (gPosition.Latitude * -1.0) + 90.0;
    double dLongitude = Math::Angles::clipAngleDegree(gPosition.Longitude + 180.0);
    double dLatScale = 90.0;
    double dLonScale = 180.0;

    return QString("a") + TileXYToQuadKey(
                (int) ( (dLongitude / dLonScale) * (pow(2.0, (double) iBingLevel - 1.0)) ),
                (int) ( (dLatitude / dLatScale) * (pow(2.0, (double) iBingLevel - 1.0)) ),
                iBingLevel
                );
}

//-------------------------------------------------------------------------------------------------

QString CTiledMaterial::TileXYToQuadKey(int tileX, int tileY, int levelOfDetail)
{
    int lTileX = tileX;
    int lTileY = tileY;

    QString sKey;

    while (levelOfDetail > 0)
    {
        int lX = lTileX % 2;
        int lY = lTileY % 2;

        lTileX = lTileX / 2;
        lTileY = lTileY / 2;

        int lCode = (lY * 2) + lX;
        sKey.insert(0, QChar(static_cast<ushort>('0' + lCode)));

        levelOfDetail--;
    }

    return sKey;
}

//-------------------------------------------------------------------------------------------------

bool CTiledMaterial::quadKeyPresent(QString sKey)
{
    return m_mTiles.contains(sKey);
}

//-------------------------------------------------------------------------------------------------

void CTiledMaterial::collectGarbage()
{
    static int iGarbageCounter = 0;

    iGarbageCounter++;

    if (iGarbageCounter > 2000)
    {
        iGarbageCounter = 0;

        QList<QString> keys = m_mTiles.keys();

        for (int iIndex = 0; iIndex < keys.count(); iIndex++)
        {
            QString sKey = keys[iIndex];

            if (m_mTiles[sKey].m_tLastUsed.secsTo(QDateTime::currentDateTime()) > 20)
            {
                m_mTiles.remove(sKey);
            }
        }
    }
}
