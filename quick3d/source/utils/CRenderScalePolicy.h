
//-------------------------------------------------------------------------------------------------
/*!
    \file CRenderScalePolicy.h
    \brief Reduced-size layer rendering with preserved on-screen appearance.
    \inmodule Quick3D
*/
//-------------------------------------------------------------------------------------------------

#pragma once

// Qt
#include <QMatrix4x4>

// Application
#include "quick3d_global.h"
#include "CVector3.h"
#include "CMatrix4.h"
#include "CRenderLayer.h"

//-------------------------------------------------------------------------------------------------

/*!
    \class CRenderScalePolicy
    \brief Scales one render layer about the camera without changing its appearance.
    \inmodule Quick3D
    Painting every vertex \a k times closer to the camera with \a k times the size
    preserves the exact angular size of every vertex, therefore the exact on-screen
    appearance of the full-size render, while compressing the consumed depth interval
    by \a k and improving depth precision. The world-space helpers define the mapping,
    the view-space helpers apply it: scaling the camera-space position about the camera
    position (the view origin) equals the world-space scale about the camera, so the
    render path scales the view matrices and the projection range instead of touching
    stored transforms, snapshots, physics, or vertex data.
*/
class QUICK3D_EXPORT CRenderScalePolicy
{
public:

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Returns \a vPosition scaled by \a dScale about \a vCamera: C + (P - C) * k
    static Math::CVector3 scaledPosition(const Math::CVector3& vCamera, const Math::CVector3& vPosition, double dScale);

    //! Returns \a vScale scaled by \a dScale
    static Math::CVector3 scaledScale(const Math::CVector3& vScale, double dScale);

    //! Returns T(C) * S(k) * T(-C), the world-space scale-about-camera matrix
    static Math::CMatrix4 scaleAboutCameraMatrix(const Math::CVector3& vCamera, double dScale);

    //! Returns S(k) * \a mView, the render path application of the same transform
    static QMatrix4x4 scaledViewMatrix(const QMatrix4x4& mView, double dScale);

    //! Returns \a mView scaled by k on the internal row-vector matrix, same transform as scaledViewMatrix()
    static Math::CMatrix4 scaledInternalViewMatrix(const Math::CMatrix4& mView, double dScale);

    //! Returns the depth range observed through the scaled pass, real bounds times \a dScale
    static SRenderDepthRange scaledDepthRange(const SRenderDepthRange& tRange, double dScale);
};

//-------------------------------------------------------------------------------------------------
