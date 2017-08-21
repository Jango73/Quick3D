
#pragma once

// Qt
#include <QImage>
#include <QtOpenGL>
#include <QThread>

// qt-plus
#include "CXMLNode.h"

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CVector3.h"
#include "CMatrix4.h"
#include "CRay3.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

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
    // Constructors and destructor
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
    QString name() const { return m_sName; }

    //!
    GLuint glTexture() const { return m_uiGLTexture; }

    //!
    QImage& image() { return m_imgTexture; }

    //!
    bool isDynamic() const { return m_bIsDynamic; }

    //!
    bool isDirty() const { return m_bDirty; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
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
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //-------------------------------------------------------------------------------------------------
    // Properties
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
