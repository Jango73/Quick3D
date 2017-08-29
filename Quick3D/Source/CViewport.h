
#pragma once

// Qt
#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QImage>

// qt-plus
#include "CMJPEGServer.h"

// Application
#include "quick3d_global.h"
#include "CVector2.h"
#include "CVector3.h"
#include "CCamera.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;
class CViewport;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CViewport : public QObject
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructor
    CViewport(C3DScene* pScene, bool bEnableMJPEGServer = false);

    //! Destructor
    virtual ~CViewport();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setEnabled(bool value) { m_bEnabled = value; }

    //!
    void setCamera(QSP<CCamera> pCamera) { m_pCamera = pCamera; }

    //!
    void setPosition(Math::CVector2 vPosition) { m_vPosition = vPosition; }

    //!
    void setSize(Math::CVector2 vSize) { m_vSize = vSize; }

    //!
    void setStreamView(bool value) { m_bStreamView = value; }

    //!
    void setNeedFrameBuffer(bool value) { m_bNeedFrameBuffer = value; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    bool isEnabled() const { return m_bEnabled; }

    //!
    QSP<CCamera> camera() const { return m_pCamera; }

    //!
    Math::CVector2 position() const { return m_vPosition; }

    //!
    Math::CVector2 size() const { return m_vSize; }

    //!
    bool streamView() const { return m_bStreamView; }

    //!
    bool needFrameBuffer() const { return m_bNeedFrameBuffer; }

    //!
    const QImage& frameBuffer() const { return m_imgFrameBuffer; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void update(double dDeltaTime);

    //!
    QImage createSubImage(const QImage& image, const QRect& rect) const;

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    C3DScene*       m_pScene;               // The scene displayed in this viewport
    CMJPEGServer*   m_pServer;              // The streaming server for this viewport
    Math::CVector2  m_vPosition;            // The viewport's position in the render window
    Math::CVector2  m_vSize;                // The viewport's size
    QImage          m_imgFrameBuffer;       // The captured frame
    bool            m_bEnabled;             // Is the viewport active?
    bool            m_bStreamView;          // Is the view streaming enabled?
    bool            m_bNeedFrameBuffer;     // Do we need the frame buffer?

    // Shared data

    QSP<CCamera>    m_pCamera;              // The camera used by this viewport
};
