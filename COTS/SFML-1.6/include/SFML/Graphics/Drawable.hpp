////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2009 Laurent Gomila (laurent.gom@gmail.com)
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

#ifndef SFML_DRAWABLE_HPP
#define SFML_DRAWABLE_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/Export.hpp>
#include <SFML/Graphics/RenderStates.hpp>


namespace sf
{
class RenderTarget;

////////////////////////////////////////////////////////////
/// \brief Abstract base class for objects that can be drawn
///        to a render target
///
////////////////////////////////////////////////////////////
class SFML_GRAPHICS_API Drawable
{
public :

    ////////////////////////////////////////////////////////////
    /// \brief Virtual destructor
    ///
    ////////////////////////////////////////////////////////////
    virtual ~Drawable() {}

private :

    friend class RenderTarget;

    ////////////////////////////////////////////////////////////
    /// \brief Draw the object to a render target
    ///
    /// This is a pure virtual function that has to be implemented
    /// by the derived class to define how the drawable should be
    /// drawn.
    ///
    /// \param target Render target to draw to
    /// \param states Current render states
    ///
    ////////////////////////////////////////////////////////////
    virtual void Draw(RenderTarget& target, RenderStates states) const = 0;
};

} // namespace sf


#endif // SFML_DRAWABLE_HPP


////////////////////////////////////////////////////////////
/// \class sf::Drawable
/// \ingroup graphics
///
/// sf::Drawable is a very simple base class that allows objects
/// of derived classes to be drawn to a sf::RenderTarget.
///
/// All you have to do in your derived class is to override the
/// Draw virtual function.
///
/// Note that inheriting from sf::Drawable is not mandatory,
/// but it allows this nice syntax "window.Draw(object)" rather
/// than "object.Draw(window)", which is more consistent with other
/// SFML classes.
///
/// Example:
/// \code
/// class MyDrawable : public sf::Drawable
/// {
/// public :
///
///    ...
///
/// private :
///
///     virtual void Draw(sf::RenderTarget& target, RenderStates states) const
///     {
///         // You can draw other high-level objects
///         target.Draw(mySprite, states);
///
///         // ... or use the low-level API
///         states.Texture = &myTexture;
///         target.Draw(myVertices, states);
///
///         // ... or draw with OpenGL directly
///         glBegin(GL_QUADS);
///         ...
///         glEnd();
///     }
///
///     sf::Sprite mySprite;
///     sf::Texture myTexture;
///     sf::VertexArray myVertices;
/// };
/// \endcode
///
/// \see sf::RenderTarget
///
////////////////////////////////////////////////////////////
