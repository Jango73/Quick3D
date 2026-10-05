
// qt-plus
#include "CLogger.h"

// Application
#include "CGLExtension.h"
#include "C3DScene.h"
#include "CComponent.h"
#include "CTexture.h"

#ifndef WIN32
#include <GL/glext.h>
#endif

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CTextureUpdater::CTextureUpdater(CTexture* pTexture)
    : m_pTexture(pTexture)
    , m_dDeltaTime(0.0)
    , m_bWork(false)
    , m_bStop(false)
{
}

//-------------------------------------------------------------------------------------------------

void CTextureUpdater::setDeltaTime(double dDeltaTime)
{
    m_dDeltaTime = dDeltaTime;
}

//-------------------------------------------------------------------------------------------------

void CTextureUpdater::stop()
{
    m_bStop = true;
    wait();
}

//-------------------------------------------------------------------------------------------------

void CTextureUpdater::work()
{
    m_bWork = true;
}

//-------------------------------------------------------------------------------------------------

void CTextureUpdater::run()
{
    while (m_bStop == false)
    {
        if (m_bWork)
        {
            if (m_pTexture->lock())
            {
                m_bWork = false;
                m_pTexture->m_pUpdater->updateTexture(m_pTexture, m_dDeltaTime);
                m_pTexture->unlock();
            }

            emit updateFinished();
        }

        msleep(20);
    }
}

//-------------------------------------------------------------------------------------------------

CTexture::CTexture(C3DScene* pScene, QString sName, const QImage& imgTexture, QSize size, int iIndex, bool bIsDynamic)
    : m_tMutex(QMutex::Recursive)
    , m_pScene(pScene)
    , m_sName(sName)
    , m_pUpdater(nullptr)
    , m_TextureUpdateWorker(nullptr)
    , m_uiGLTexture(0)
    , m_bIsDynamic(bIsDynamic)
    , m_bDirty(false)
    , m_iUpdateFPS(25)
    , m_iUpdateCounter(0)
{
    if (imgTexture.width() > 0 && imgTexture.height() > 0)
    {
        m_pScene->makeCurrentRenderingContext();

        QImage tImage = QGLWidget::convertToGLFormat(imgTexture);

        GL_glActiveTexture(GL_TEXTURE1 + iIndex);

        glGenTextures(1, &m_uiGLTexture);

        if (m_bIsDynamic)
        {
            m_imgTexture = imgTexture.copy();
            m_TextureUpdateWorker = new CTextureUpdater(this);
            connect(m_TextureUpdateWorker, SIGNAL(updateFinished()), this, SLOT(onUpdateFinished()));
            m_TextureUpdateWorker->start();
        }

        glBindTexture(GL_TEXTURE_2D, m_uiGLTexture);

        tImage = tImage.scaled(size.width(), size.height(), Qt::IgnoreAspectRatio);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tImage.width(), tImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tImage.bits());

        GL_glGenerateMipmap(GL_TEXTURE_2D);
    }
}

//-------------------------------------------------------------------------------------------------

CTexture::~CTexture()
{
    if (m_TextureUpdateWorker != nullptr)
    {
        m_TextureUpdateWorker->stop();
        delete m_TextureUpdateWorker;
    }

    m_pScene->makeCurrentRenderingContext();

    glDeleteTextures(1, &m_uiGLTexture);
}

//-------------------------------------------------------------------------------------------------

void CTexture::update(double dDeltaTime)
{
    if (m_bIsDynamic && m_pUpdater != nullptr && m_TextureUpdateWorker != nullptr)
    {
        m_iUpdateCounter++;

        if (m_iUpdateCounter > 3)
        {
            m_TextureUpdateWorker->setDeltaTime(dDeltaTime);
            m_TextureUpdateWorker->work();
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CTexture::activate(int iIndex)
{
    m_pScene->makeCurrentRenderingContext();

    GL_glActiveTexture(GL_TEXTURE1 + iIndex);

    glBindTexture(GL_TEXTURE_2D, m_uiGLTexture);

    if (m_bIsDynamic)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    else
    {
        glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
}

//-------------------------------------------------------------------------------------------------

bool CTexture::lock()
{
    return m_tMutex.tryLock(2000);
}

//-------------------------------------------------------------------------------------------------

void CTexture::unlock()
{
    m_tMutex.unlock();
}

//-------------------------------------------------------------------------------------------------

void CTexture::onUpdateFinished()
{
    if (lock())
    {
        m_pScene->makeCurrentRenderingContext();

        GL_glActiveTexture(GL_TEXTURE1 + 0);

        glBindTexture(GL_TEXTURE_2D, m_uiGLTexture);

        QImage tImage = QGLWidget::convertToGLFormat(m_imgTexture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tImage.width(), tImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tImage.bits());

        m_iUpdateCounter = 0;

        unlock();
    }
}
