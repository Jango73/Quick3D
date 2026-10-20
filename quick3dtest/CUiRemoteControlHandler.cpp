
// Application
#include "CUiRemoteControlHandler.h"
#include "Quick3DTest.h"

//-------------------------------------------------------------------------------------------------

/*!
    Builds a user interface handler driving \a pWindow. \br\br
    \a pParent is stored as the Qt parent of this object.
*/
CUiRemoteControlHandler::CUiRemoteControlHandler(Quick3DTest* pWindow, QObject* pParent)
    : CRemoteControlHandler(pParent)
    , m_pWindow(pWindow)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys a user interface handler.
*/
CUiRemoteControlHandler::~CUiRemoteControlHandler()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns a JSON document describing the vehicle named \a sName.
*/
QString CUiRemoteControlHandler::vehicleStatus(const QString& sName)
{
    if (m_pWindow.isNull() == true)
    {
        return "{\"ok\":false,\"error\":\"no window\"}";
    }

    return m_pWindow->vehicleStatusJson(sName);
}

//-------------------------------------------------------------------------------------------------

/*!
    Selects the vehicle named \a sName through the controllable combo box, returns \c true on success.
*/
bool CUiRemoteControlHandler::selectVehicle(const QString& sName)
{
    if (m_pWindow.isNull() == true)
    {
        return false;
    }

    return m_pWindow->selectVehicleByName(sName);
}

//-------------------------------------------------------------------------------------------------

/*!
    Selects the camera named \a sName through the views combo box, returns \c true on success.
*/
bool CUiRemoteControlHandler::selectView(const QString& sName)
{
    if (m_pWindow.isNull() == true)
    {
        return false;
    }

    return m_pWindow->selectViewByName(sName);
}

//-------------------------------------------------------------------------------------------------

/*!
    Injects a key event (Qt::Key value) with \a iAction (0 = press, 1 = release, 2 = click), returns \c true on success.
*/
bool CUiRemoteControlHandler::sendKey(int iKey, int iAction)
{
    if (m_pWindow.isNull() == true)
    {
        return false;
    }

    return m_pWindow->postKeyToView(iKey, iAction);
}
