
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

    //! D�finit la g�olocalisation et le niveau
    void setCurrentPositionAndLevel(const CGeoloc& gPosition, int iLevel);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Retourne le nom de la classe
    virtual QString getClassName() const { return ClassName_CTiledMaterial; }

    //! Retourne des coordonn�es de texture pour une g�olocalisation donn�e
    virtual Math::CVector2 texCoords(const CGeoloc& gPosition, int iLevel);

    //-------------------------------------------------------------------------------------------------
    // M�thodes h�rit�es
    //-------------------------------------------------------------------------------------------------

    //! Active ce mat�riau pour le rendu
    virtual QGLShaderProgram* activate(CRenderContext* pContext);

    //! Applique des transformations � la g�olocalisation donn�e (ex: Mercator)
    virtual CGeoloc transformGeoloc(const CGeoloc& gPosition);

protected:

    //-------------------------------------------------------------------------------------------------
    // M�thodes prot�g�es
    //-------------------------------------------------------------------------------------------------

    //! Retourne une quad key pour une g�olocalisation et un niveau donn�s
    QString quadKeyFromPositionAndLevel(const CGeoloc& gPosition, int iLevel);

    //!
    QString TileXYToQuadKey(int tileX, int tileY, int levelOfDetail);

    //!
    bool quadKeyPresent(QString sKey);

    //! D�truit les textures de terrain inutilis�es
    void collectGarbage();

    //-------------------------------------------------------------------------------------------------
    // Slots
    //-------------------------------------------------------------------------------------------------

protected slots:

    void onTileReady(QString sTileName);

    //-------------------------------------------------------------------------------------------------
    // Propri�t�s
    //-------------------------------------------------------------------------------------------------

protected:

    int                     m_iLevels;
    CHTTPMapClient          m_tClient;
    QMap<QString, CTile>    m_mTiles;
    QString                 m_sCurrentQuadKey;
};
