
#pragma once

#include "quick3d_global.h"

// Qt
#include <QtOpenGL>
#include <QObject>
#include <QDateTime>

// Fondations
#include "CGeoloc.h"

// Application
#include "CQ3DConstants.h"
#include "CMaterial.h"
#include "CHTTPMapClient.h"

//-------------------------------------------------------------------------------------------------

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

    //! Définit la géolocalisation et le niveau
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

    //! Active ce matériau pour le rendu
    virtual QGLShaderProgram* activate(CRenderContext* pContext);

    //! Applique des transformations à la géolocalisation donnée (ex: Mercator)
    virtual CGeoloc transformGeoloc(const CGeoloc& gPosition);

protected:

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

    //! Retourne une quad key pour une géolocalisation et un niveau donnés
    QString quadKeyFromPositionAndLevel(const CGeoloc& gPosition, int iLevel);

    //!
    QString TileXYToQuadKey(int tileX, int tileY, int levelOfDetail);

    //!
    bool quadKeyPresent(QString sKey);

    //! Détruit les textures de terrain inutilisées
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
};
