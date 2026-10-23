
// Qt
#include <QKeyEvent>

// Application
#include "CKeyboardMapper.h"

//-------------------------------------------------------------------------------------------------

namespace
{
    // Note: hexadecimal for hardware codes, named constants instead of literals
    // Linux accepts X11 keycodes (evdev + 8) and raw evdev codes, Qt Wayland
    // reports evdev while Qt X11 reports evdev + 8 for the same position
    const quint32 LINUX_X11_FORWARD_TOP = 0x19;
    const quint32 LINUX_EVDEV_FORWARD_TOP = 0x11;
    const quint32 LINUX_X11_TURN_LEFT_BOTTOM = 0x34;
    const quint32 LINUX_EVDEV_TURN_LEFT_BOTTOM = 0x2C;
    const quint32 LINUX_X11_STRAFE_LEFT_MIDDLE = 0x26;
    const quint32 LINUX_EVDEV_STRAFE_LEFT_MIDDLE = 0x1E;
    const quint32 LINUX_X11_LOOK_UP_TOP = 0x18;
    const quint32 LINUX_EVDEV_LOOK_UP_TOP = 0x10;

    // Note: Windows scan code set 1, positional like evdev
    const quint32 WINDOWS_FORWARD_TOP = 0x11;
    const quint32 WINDOWS_TURN_LEFT_BOTTOM = 0x2C;
    const quint32 WINDOWS_STRAFE_LEFT_MIDDLE = 0x1E;
    const quint32 WINDOWS_LOOK_UP_TOP = 0x10;

    // Note: macOS kVK_ANSI codes, positional as well
    const quint32 MACOS_FORWARD_W = 0x0D;
    const quint32 MACOS_TURN_LEFT_Z = 0x06;
    const quint32 MACOS_STRAFE_LEFT_A = 0x00;
    const quint32 MACOS_LOOK_UP_Q = 0x0C;
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Returns the canonical AZERTY reference key for the event.
*/
/***************************************************************************/
int CKeyboardMapper::canonicalKey(const QKeyEvent* pEvent)
{
    if (pEvent == nullptr)
    {
        return Qt::Key_unknown;
    }

    const quint32 uScanCode = pEvent->nativeScanCode();

    // Note: synthetic events (QTest, remote control) carry no scan code,
    // the logical key is already the AZERTY reference used by the tests
    if (uScanCode == 0x0)
    {
        return pEvent->key();
    }

#if defined(Q_OS_WIN)
    if (uScanCode == WINDOWS_FORWARD_TOP)
    {
        return Qt::Key_Z;
    }

    if (uScanCode == WINDOWS_TURN_LEFT_BOTTOM)
    {
        return Qt::Key_W;
    }

    if (uScanCode == WINDOWS_STRAFE_LEFT_MIDDLE)
    {
        return Qt::Key_Q;
    }

    if (uScanCode == WINDOWS_LOOK_UP_TOP)
    {
        return Qt::Key_A;
    }
#elif defined(Q_OS_MACOS)
    if (uScanCode == MACOS_FORWARD_W)
    {
        return Qt::Key_Z;
    }

    if (uScanCode == MACOS_TURN_LEFT_Z)
    {
        return Qt::Key_W;
    }

    if (uScanCode == MACOS_STRAFE_LEFT_A)
    {
        return Qt::Key_Q;
    }

    if (uScanCode == MACOS_LOOK_UP_Q)
    {
        return Qt::Key_A;
    }
#else
    if (uScanCode == LINUX_X11_FORWARD_TOP || uScanCode == LINUX_EVDEV_FORWARD_TOP)
    {
        return Qt::Key_Z;
    }

    if (uScanCode == LINUX_X11_TURN_LEFT_BOTTOM || uScanCode == LINUX_EVDEV_TURN_LEFT_BOTTOM)
    {
        return Qt::Key_W;
    }

    if (uScanCode == LINUX_X11_STRAFE_LEFT_MIDDLE || uScanCode == LINUX_EVDEV_STRAFE_LEFT_MIDDLE)
    {
        return Qt::Key_Q;
    }

    if (uScanCode == LINUX_X11_LOOK_UP_TOP || uScanCode == LINUX_EVDEV_LOOK_UP_TOP)
    {
        return Qt::Key_A;
    }
#endif

    return pEvent->key();
}
