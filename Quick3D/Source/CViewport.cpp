
// qt-plus
#include "CLogger.h"

// Application
#include "CViewport.h"
#include "C3DScene.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CViewport::CViewport(C3DScene* pScene, bool bEnableMJPEGServer)
    : m_pScene(pScene)
    , m_bEnabled(false)
    , m_bStreamView(false)
    , m_bNeedFrameBuffer(false)
    , m_pCamera(nullptr)
    , m_pServer(nullptr)
{
    static int iPort = 6666;

    //-----------------------------------------------
    // Création des membres

    if (bEnableMJPEGServer)
    {
        m_pServer = new CMJPEGServer(iPort++);
    }
}

//-------------------------------------------------------------------------------------------------

CViewport::~CViewport()
{
    if (m_pServer != nullptr)
    {
        delete m_pServer;
    }
}

//-------------------------------------------------------------------------------------------------

void CViewport::update(double dDeltaTime)
{
    if ((m_bStreamView && m_pServer != nullptr) || m_bNeedFrameBuffer)
    {
        if (m_pScene->frameBuffer().width() > 0 && m_pScene->frameBuffer().height() > 0)
        {
            QRect rect((int) m_vPosition.X, (int) m_vPosition.Y, (int) m_vSize.X, (int) m_vSize.Y);

            if (
                    m_vPosition.X >= 0.0 &&
                    m_vPosition.Y >= 0.0 &&
                    m_vPosition.X + m_vSize.X <= m_pScene->frameBuffer().width() &&
                    m_vPosition.Y + m_vSize.Y <= m_pScene->frameBuffer().height()
                    )
            {
                m_imgFrameBuffer = createSubImage(m_pScene->frameBuffer(), rect).copy();

                if (m_imgFrameBuffer.width() > 0 && m_imgFrameBuffer.height() > 0)
                {
                    if (m_pServer != nullptr)
                    {
                        m_pServer->sendImage(m_imgFrameBuffer);
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

QImage CViewport::createSubImage(const QImage& image, const QRect& rect) const
{
    quint32 offset = rect.x() * image.depth() / 8 + rect.y() * image.bytesPerLine();
    return QImage(image.constBits() + offset, rect.width(), rect.height(), image.bytesPerLine(), image.format());
}
