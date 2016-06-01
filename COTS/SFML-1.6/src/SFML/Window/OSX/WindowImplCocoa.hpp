////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2011 Marco Antognini (antognini.marco@gmail.com), 
//                         Laurent Gomila (laurent.gom@gmail.com), 
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#ifndef SFML_WINDOWIMPLCOCOA_HPP
#define SFML_WINDOWIMPLCOCOA_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Window/Event.hpp>
#include <SFML/Window/WindowImpl.hpp>

////////////////////////////////////////////////////////////
/// Predefine OBJC classes
////////////////////////////////////////////////////////////
#ifdef __OBJC__

#import <SFML/Window/OSX/WindowImplDelegateProtocol.h>
typedef id<WindowImplDelegateProtocol,NSObject> WindowImplDelegateRef;

@class NSOpenGLContext;
typedef NSOpenGLContext* NSOpenGLContextRef;

#else // If C++

typedef unsigned short unichar; // See NSString.h

typedef void* WindowImplDelegateRef;
typedef void* NSOpenGLContextRef;

#endif

namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
/// \brief Mac OS X (Cocoa) implementation of WindowImpl
///
////////////////////////////////////////////////////////////
class WindowImplCocoa : public WindowImpl
{
public:
    ////////////////////////////////////////////////////////////
    /// \brief Construct the window implementation from an existing control
    ///
    /// \param handle Platform-specific handle of the control
    ///
    ////////////////////////////////////////////////////////////
    WindowImplCocoa(WindowHandle handle);
    
    ////////////////////////////////////////////////////////////
    /// \brief Create the window implementation
    ///
    /// \param mode  Video mode to use
    /// \param title Title of the window
    /// \param style Window style (resizable, fixed, or fullscren)
    ///
    ////////////////////////////////////////////////////////////
    WindowImplCocoa(VideoMode mode, const std::string& title, unsigned long style);
    
    ////////////////////////////////////////////////////////////
    /// \brief Destructor
    ///
    ////////////////////////////////////////////////////////////
    ~WindowImplCocoa();
    
    ////////////////////////////////////////////////////////////
    /// \brief Window Closed Event – called by the cocoa window object.
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    ////////////////////////////////////////////////////////////
    void WindowClosed(void);
    
    ////////////////////////////////////////////////////////////
    /// \brief Window Resized Event – called by the cocoa window object.
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    /// \param width
    /// \param height
    ///
    ////////////////////////////////////////////////////////////
    void WindowResized(unsigned int width, unsigned int height);
    
    ////////////////////////////////////////////////////////////
    /// \brief Window Lost Focus Event – called by the cocoa window object.
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    ////////////////////////////////////////////////////////////
    void WindowLostFocus(void);
    
    ////////////////////////////////////////////////////////////
    /// \brief Window Get Focus Event – called by the cocoa window object.
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    ////////////////////////////////////////////////////////////
    void WindowGainedFocus(void);
    
    ////////////////////////////////////////////////////////////
    /// \brief Mouse Down Event – called by the cocoa view object.
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    /// \param button
    /// \param x
    /// \param y
    ///
    ////////////////////////////////////////////////////////////
    void MouseDownAt(Mouse::Button button, int x, int y);
    
    ////////////////////////////////////////////////////////////
    /// \brief Mouse Up Event – called by the cocoa view object.
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    /// \param button
    /// \param x
    /// \param y
    ///
    ////////////////////////////////////////////////////////////
    void MouseUpAt(Mouse::Button button, int x, int y);
    
    ////////////////////////////////////////////////////////////
    /// \brief Mouse Moved Event – called by the cocoa view object.
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    /// \param x
    /// \param y
    ///
    ////////////////////////////////////////////////////////////
    void MouseMovedAt(int x, int y);
    
    ////////////////////////////////////////////////////////////
    /// \brief Mouse Wheel Scrolled Event – called by the cocoa view object.
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    /// \param delta
    /// \param x
    /// \param y
    ///
    ////////////////////////////////////////////////////////////
    void MouseWheelScrolledAt(float delta, int x, int y);
    
    ////////////////////////////////////////////////////////////
    /// \brief Mouse In Event – called by the cocoa view object.
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    ////////////////////////////////////////////////////////////
    void MouseMovedIn(void);
    
    ////////////////////////////////////////////////////////////
    /// \brief Mouse Out Event – called by the cocoa view object.
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    ////////////////////////////////////////////////////////////
    void MouseMovedOut(void);
    
    ////////////////////////////////////////////////////////////
    /// \brief Key Down Event – called by the cocoa view object.
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    /// \param key
    ///
    ////////////////////////////////////////////////////////////
    void KeyDown(Event::KeyEvent key);
    
    ////////////////////////////////////////////////////////////
    /// \brief Key Up Event – called by the cocoa view object.
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    /// \param key
    ///
    ////////////////////////////////////////////////////////////
    void KeyUp(Event::KeyEvent key);
    
    ////////////////////////////////////////////////////////////
    /// \brief Text Entred Event – called by the cocoa view object.
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    /// \param charcode Input unicode
    ///
    ////////////////////////////////////////////////////////////
    void TextEntered(unichar charcode);
    
    ////////////////////////////////////////////////////////////
    /// \brief Apply the context to the view.
    ///
    /// Called by the SFML context object to finalize its creation.
    ///
    /// \param context The context to bind to the window
    ///
    ////////////////////////////////////////////////////////////
    void ApplyContext(NSOpenGLContextRef context) const;
    
    ////////////////////////////////////////////////////////////
    /// \brief Change the type of the current process to become a full GUI app.
    ///        Also ensure NSApp is constructed.
    ///
    ////////////////////////////////////////////////////////////
    static void SetUpProcess(void);
    
private:

    ////////////////////////////////////////////////////////////
    /// \brief Process incoming events from the operating system
    ///
    ////////////////////////////////////////////////////////////
    virtual void ProcessEvents();
    
    ////////////////////////////////////////////////////////////
    /// \brief Get the OS-specific handle of the window
    ///
    /// \return Handle of the window
    ///
    ////////////////////////////////////////////////////////////
    virtual WindowHandle GetSystemHandle() const;
    
    ////////////////////////////////////////////////////////////
    /// \brief Show or hide the mouse cursor
    ///
    /// \param show True to show, false to hide
    ///
    ////////////////////////////////////////////////////////////
    virtual void ShowMouseCursor(bool show);

    ////////////////////////////////////////////////////////////
    /// \brief Change the position of the window on screen
    ///
    /// \param x Left position
    /// \param y Top position
    ///
    ////////////////////////////////////////////////////////////
    virtual void SetPosition(int x, int y);
    
    ////////////////////////////////////////////////////////////
    /// \brief Change the size of the rendering region of the window
    ///
    /// \param width  New width
    /// \param height New height
    ///
    ////////////////////////////////////////////////////////////
    virtual void SetSize(unsigned int width, unsigned int height);
    
    ////////////////////////////////////////////////////////////
    /// \brief Change the title of the window
    ///
    /// \param title New title
    ///
    ////////////////////////////////////////////////////////////
    virtual void SetTitle(const std::string& title);
    
    ////////////////////////////////////////////////////////////
    /// \brief Show or hide the window
    ///
    /// \param show True to show, false to hide
    ///
    ////////////////////////////////////////////////////////////
    virtual void Show(bool show);
    
    ////////////////////////////////////////////////////////////
    /// \brief Enable or disable automatic key-repeat
    ///
    /// \param enabled True to enable, false to disable
    ///
    ////////////////////////////////////////////////////////////
    virtual void EnableKeyRepeat(bool enabled);
    
    ////////////////////////////////////////////////////////////
    /// \brief Change the window's icon
    ///
    /// \param width  Icon's width, in pixels
    /// \param height Icon's height, in pixels
    /// \param pixels Pointer to the pixels in memory, format must be RGBA 32 bits
    ///
    ////////////////////////////////////////////////////////////
    virtual void SetIcon(unsigned int width, unsigned int height, const Uint8* pixels);
    
    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    WindowImplDelegateRef myDelegate;   ///< Implementation in Obj-C.
    bool                  myShowCursor; ///< Is the cursor displayed or hidden ?
};
    
} // namespace priv
    
} // namespace sf


#endif // SFML_WINDOWIMPLCOCOA_HPP
