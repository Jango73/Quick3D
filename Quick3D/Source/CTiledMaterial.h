
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
            , m_pTexture(NULL)
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
            if (m_pTexture != NULL)
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
    // Constructeurs et destructeur
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

    //! Retourne le nom de la classe
    virtual QString getClassName() const { return ClassName_CTiledMaterial; }

    //! Retourne des coordonnées de texture pour une géolocalisation donnée
    virtual Math::CVector2 texCoords(const CGeoloc& gPosition, int iLevel);

    //-------------------------------------------------------------------------------------------------
    // Méthodes héritées
    //-------------------------------------------------------------------------------------------------

    //! Active ce matériau pour le rendu
    virtual QGLShaderProgram* activate(CRenderContext* pContext);

    //! Applique des transformations à la géolocalisation donnée (ex: Mercator)
    virtual CGeoloc transformGeoloc(const CGeoloc& gPosition);

protected:

    //-------------------------------------------------------------------------------------------------
    // Méthodes protégées
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
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    int                     m_iLevels;
    CHTTPMapClient          m_tClient;
    QMap<QString, CTile>    m_mTiles;
    QString                 m_sCurrentQuadKey;
};
