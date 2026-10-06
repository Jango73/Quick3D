
#pragma once

// Application
#include "quick3d_global.h"

#ifndef WIN32
#include "CJoystickReader.h"
#endif

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CJoystick
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CJoystick(unsigned int uiJoystickIndex);

    //!
    ~CJoystick();

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    bool connected() const { return m_bConnected; }

    //
    QMap<unsigned int, bool>& buttonStates() { return m_mButtonStates; }

    //
    QMap<unsigned int, double>& axisStates() { return m_dAxisStates; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void update(double dDeltaTime);

protected:

    bool                        m_bConnected;
    unsigned int                m_uiJoystickIndex;
    unsigned int                m_uiNumButtons;
    unsigned int                m_uiNumAxis;
    QMap<unsigned int, bool>    m_mButtonStates;
    QMap<unsigned int, double>  m_dAxisStates;
#ifndef WIN32
    CJoystickReader*            m_pReader;
#endif
};
