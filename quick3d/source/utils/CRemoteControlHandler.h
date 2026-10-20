
#pragma once

// Qt
#include <QObject>
#include <QString>

// Application
#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

//! Contract between the remote control HTTP server and the user interface. \br\br
//! The server only talks to this interface, every scene or view access goes
//! through its implementation. Methods may be called from any thread, the
//! implementation marshals them to the user interface thread.
class QUICK3D_EXPORT CRemoteControlHandler : public QObject
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructor
    explicit CRemoteControlHandler(QObject* pParent = nullptr);

    //! Destructor
    virtual ~CRemoteControlHandler();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Returns a JSON document describing the vehicle named \a sName
    Q_INVOKABLE virtual QString vehicleStatus(const QString& sName) = 0;

    //! Selects the vehicle named \a sName through the user interface, returns \c true on success
    Q_INVOKABLE virtual bool selectVehicle(const QString& sName) = 0;

    //! Selects the camera named \a sName through the user interface, returns \c true on success
    Q_INVOKABLE virtual bool selectView(const QString& sName) = 0;

    //! Injects a key event (Qt::Key value) with \a iAction (0 = press, 1 = release, 2 = click), returns \c true on success
    Q_INVOKABLE virtual bool sendKey(int iKey, int iAction) = 0;
};
