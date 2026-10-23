
#pragma once

// Qt
#include <Qt>

// Application
#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

class QKeyEvent;

//-------------------------------------------------------------------------------------------------

/*!
* \brief Maps physical key positions to the AZERTY reference layout. \br\br
* Real keyboards report a platform native scan code in QKeyEvent::nativeScanCode
* that follows the physical position, not the printed label. Synthetic events
* (QTest, remote control) carry 0 and fall back to QKeyEvent::key unchanged. \br\br
* Reference positions are AZERTY: forward is the top row second key (Z on
* AZERTY, W on QWERTY), turn left is the bottom row first key (W on AZERTY,
* Z on QWERTZ). Every action stays reachable on every hardware layout.
*/
class QUICK3D_EXPORT CKeyboardMapper
{

public:

    //!
    //! Returns the canonical AZERTY reference key for \a pEvent.
    //! Real events resolve through the native scan code tables below.
    //! Synthetic events with a zero scan code return QKeyEvent::key unchanged.
    //!
    static int canonicalKey(const QKeyEvent* pEvent);
};
