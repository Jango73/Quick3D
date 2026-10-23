
#pragma once

// Qt
#include <QtOpenGL>
#include <QObject>
#include <QDateTime>

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CGeoloc.h"
#include "CMaterial.h"
#include "CHTTPMapClient.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CTiledMaterial : public CMaterial
{
    Q_OBJECT

public:

    class CTile
    {
    public:

        CTile()
            : m_iLevel(0)
            , m_pTexture(nullptr)
        {
            m_tLastUsed = QDateTime::currentDateTime();
        }

        CTile(int iLevel, CTexture* pTexture)
            : m_iLevel(iLevel)
            , m_pTexture(pTexture)
        {
            m_tLastUsed = QDateTime::currentDateTime();
        }

        ~CTile()
        {
            if (m_pTexture != nullptr)
            {
                delete m_pTexture;
            }
        }

        int         m_iLevel;
        CGeoloc     m_gCenter;
        CGeoloc     m_gSize;
        CTexture*   m_pTexture;
        QDateTime   m_tLastUsed;
    };

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CTiledMaterial(C3DScene* pScene);

    //!
    virtual ~CTiledMaterial();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setLevels(int value) { m_iLevels = value; }

    //! Sets the geolocation and the level
    void setCurrentPositionAndLevel(const CGeoloc& gPosition, int iLevel);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns this object's class name
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CTiledMaterial; }

    //! Returns texture coordinates for a given geo loc
    virtual Math::CVector2 texCoords(const CGeoloc& gPosition, int iLevel);

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! The tiled diffuse source replaces the base diffuse maps: nothing to bind here
    virtual void bindDiffuseMaps(CRenderContext* pContext, QOpenGLShaderProgram* pProgram) Q_DECL_OVERRIDE;

    //! Binds the current tile, once per quad key
    virtual void applyDynamicState(CRenderContext* pContext, QOpenGLShaderProgram* pProgram) Q_DECL_OVERRIDE;

    //! Applies transformations to the given geolocation (example: Mercator)
    virtual CGeoloc transformGeoloc(const CGeoloc& gPosition);

protected:

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

    //! Returns a quad key for a given geolocation and level
    QString quadKeyFromPositionAndLevel(const CGeoloc& gPosition, int iLevel);

    //!
    QString TileXYToQuadKey(int tileX, int tileY, int levelOfDetail);

    //!
    bool quadKeyPresent(QString sKey);

    //! Destroys unused terrain textures
    void collectGarbage();

    //-------------------------------------------------------------------------------------------------
    // Slots
    //-------------------------------------------------------------------------------------------------

protected slots:

    void onTileReady(QString sTileName);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    int                     m_iLevels;
    CHTTPMapClient          m_tClient;
    QMap<QString, CTile>    m_mTiles;
    QString                 m_sCurrentQuadKey;
    QString                 m_sLastBoundQuadKey;
    QOpenGLShaderProgram*   m_pLastBoundProgram;
};
