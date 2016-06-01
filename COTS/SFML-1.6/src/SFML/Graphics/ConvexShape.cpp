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
#include <SFML/Graphics/ConvexShape.hpp>


namespace sf
{
////////////////////////////////////////////////////////////
ConvexShape::ConvexShape(unsigned int pointCount)
{
    SetPointCount(pointCount);
}


////////////////////////////////////////////////////////////
void ConvexShape::SetPointCount(unsigned int count)
{
    myPoints.resize(count);
    Update();
}


////////////////////////////////////////////////////////////
unsigned int ConvexShape::GetPointCount() const
{
    return static_cast<unsigned int>(myPoints.size());
}


////////////////////////////////////////////////////////////
void ConvexShape::SetPoint(unsigned int index, const Vector2f& point)
{
    myPoints[index] = point;
    Update();
}


////////////////////////////////////////////////////////////
Vector2f ConvexShape::GetPoint(unsigned int index) const
{
    return myPoints[index];
}

} // namespace sf
