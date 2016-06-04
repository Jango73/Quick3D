
#pragma once

// Qt
#include <QImage>
#include <QtOpenGL>
#include <QThread>

// Application
#include "CVector3.h"
#include "CMatrix4.h"
#include "CRay3.h"
#include "CXMLNode.h"
#include "CQ3DConstants.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;
class CComponent;
class CTexture;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CTextureUpdater : public QThread
{
public:

    //!
    CTextureUpdater(CTexture* pTexture);

    //!
    void setDeltaTime(double dDeltaTime);

    //!
    virtual void run();

protected:

    CTexture*   m_pTexture;
    double      m_dDeltaTime;
};

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CTexture : public QObject
{
    Q_OBJECT

    friend class CTextureUpdater;

public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //!
    CTexture(C3DScene* pScene, QString sName, const QImage& imgTexture, QSize size, int iIndex, bool bIsDynamic = false);

    //!
    virtual ~CTexture();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setDirty(bool bValue) { m_bDirty = bValue; }

    //!
    void setUpdater(CComponent* pUpdater) { m_pUpdater = pUpdater; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    QString getName() const { return m_sName; }

    //!
    GLuint getGLTexture() const { return m_uiGLTexture; }

    //!
    QImage& getImage() { return m_imgTexture; }

    //!
    bool isDynamic() const { return m_bIsDynamic; }

    //!
    bool isDirty() const { return m_bDirty; }

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    //!
    virtual void update(double dDeltaTime);

    //!
    virtual void activate(int iIndex = 0);

    //-------------------------------------------------------------------------------------------------
    // Slots
    //-------------------------------------------------------------------------------------------------

public slots:

    void onUpdateFinished();

    //-------------------------------------------------------------------------------------------------
    // Méthodes protégés
    //-------------------------------------------------------------------------------------------------

protected:

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    C3DScene*           m_pScene;
    CComponent*         m_pUpdater;
    CTextureUpdater*    m_TextureUpdateWorker;
    QString             m_sName;
    GLuint              m_uiGLTexture;
    QImage              m_imgTexture;
    bool                m_bIsDynamic;
    bool                m_bDirty;
    int                 m_iUpdateFPS;
    int                 m_iUpdateCounter;
};
