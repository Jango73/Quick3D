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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/RenderTextureImplDefault.hpp>
#include <SFML/Graphics/GLCheck.hpp>
#include <SFML/Graphics/TextureSaver.hpp>
#include <SFML/Window/Context.hpp>
#include <SFML/System/Err.hpp>


namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
RenderTextureImplDefault::RenderTextureImplDefault() :
myContext(0),
myWidth  (0),
myHeight (0)
{

}


////////////////////////////////////////////////////////////
RenderTextureImplDefault::~RenderTextureImplDefault()
{
    // Destroy the context
    delete myContext;
}


////////////////////////////////////////////////////////////
bool RenderTextureImplDefault::Create(unsigned int width, unsigned int height, unsigned int, bool depthBuffer)
{
    // Store the dimensions
    myWidth = width;
    myHeight = height;

    // Create the in-memory OpenGL context
    myContext = new Context(ContextSettings(depthBuffer ? 32 : 0), width, height);

    return true;
}


////////////////////////////////////////////////////////////
bool RenderTextureImplDefault::Activate(bool active)
{
    return myContext->SetActive(active);
}


////////////////////////////////////////////////////////////
void RenderTextureImplDefault::UpdateTexture(unsigned int textureId)
{
    // Make sure that the current texture binding will be preserved
    priv::TextureSaver save;

    // Copy the rendered pixels to the texture
    GLCheck(glBindTexture(GL_TEXTURE_2D, textureId));
    GLCheck(glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, myWidth, myHeight));
}

} // namespace priv

} // namespace sf
