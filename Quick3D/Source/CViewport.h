
#ifndef CVIEWPORT_H
#define CVIEWPORT_H

#include "quick3d_global.h"

// QT
#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QImage>

// Fondations
#include "CVector2.h"
#include "CVector3.h"
#include "CMJPEGServer.h"

// Application
#include "CCamera.h"

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

    //! Constructeur
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
    QSP<CCamera> getCamera() const { return m_pCamera; }

    //!
    Math::CVector2 getPosition() const { return m_vPosition; }

    //!
    Math::CVector2 getSize() const { return m_vSize; }

    //!
    bool getStreamView() const { return m_bStreamView; }

    //!
    bool getNeedFrameBuffer() const { return m_bNeedFrameBuffer; }

    //!
    const QImage& getFrameBuffer() const { return m_imgFrameBuffer; }

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

    C3DScene*       m_pScene;				// La scène vue dans ce viewport
    CMJPEGServer*   m_pServer;				// Le serveur de streaming du viewport
    Math::CVector2  m_vPosition;			// La position du viewport dans la fenêtre de rendu
    Math::CVector2  m_vSize;				// La taille du viewport
    QImage          m_imgFrameBuffer;		// L'image capturée après rendu pour le streaming
    bool            m_bEnabled;				// Le viewport est activé?
    bool            m_bStreamView;			// Doit-on faire un streaming de la vue?
    bool            m_bNeedFrameBuffer;		// A t-on besoin du frame buffer?

    // Shared data

    QSP<CCamera>    m_pCamera;				// La caméra associée au viewport
};

#endif // CVIEWPORT_H
