
#pragma once

// Qt
#include <QPointer>

// Application
#include "CRemoteControlHandler.h"

//-------------------------------------------------------------------------------------------------

class Quick3DTest;

//-------------------------------------------------------------------------------------------------

//! User interface handler for the remote control server. \br\br
//! Everything goes through the main window: vehicle selection uses the
//! controllable combo box like a user click, keys are posted to the 3D view,
//! status is read from the scene. Lives in the user interface thread.
class CUiRemoteControlHandler : public CRemoteControlHandler
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructor, drives \a pWindow
    explicit CUiRemoteControlHandler(Quick3DTest* pWindow, QObject* pParent = nullptr);

    //! Destructor
    virtual ~CUiRemoteControlHandler() Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! Returns a JSON document describing the vehicle named \a sName
    virtual QString vehicleStatus(const QString& sName) Q_DECL_OVERRIDE;

    //! Selects the vehicle named \a sName through the controllable combo box, returns \c true on success
    virtual bool selectVehicle(const QString& sName) Q_DECL_OVERRIDE;

    //! Selects the camera named \a sName through the views combo box, returns \c true on success
    virtual bool selectView(const QString& sName) Q_DECL_OVERRIDE;

    //! Injects a key event (Qt::Key value) with \a iAction (0 = press, 1 = release, 2 = click), returns \c true on success
    virtual bool sendKey(int iKey, int iAction) Q_DECL_OVERRIDE;

protected:

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QPointer<Quick3DTest>   m_pWindow;
};
