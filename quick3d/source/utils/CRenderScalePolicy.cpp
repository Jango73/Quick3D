
// Application
#include "CRenderScalePolicy.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

/*!
    Returns \a vCamera scaled position of \a vPosition by \a dScale. \br\br
    The painted vertex keeps the exact angular size of the source vertex seen from the camera.
*/
CVector3 CRenderScalePolicy::scaledPosition(const CVector3& vCamera, const CVector3& vPosition, double dScale)
{
    return vCamera + (vPosition - vCamera) * dScale;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns \a vScale scaled by \a dScale.
*/
CVector3 CRenderScalePolicy::scaledScale(const CVector3& vScale, double dScale)
{
    return vScale * dScale;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns T(C) * S(k) * T(-C), the world-space scale-about-camera matrix. \br\br
    Rigid objects scale through their top-level transform: the scaled world position
    follows scaledPosition() and the scaled world scale follows scaledScale() with
    unchanged orientation, child objects inherit through the existing composition.
*/
CMatrix4 CRenderScalePolicy::scaleAboutCameraMatrix(const CVector3& vCamera, double dScale)
{
    CMatrix4 mToOrigin = CMatrix4::makeTranslation(vCamera * -1.0);
    CMatrix4 mScale = CMatrix4::makeScale(CVector3(dScale, dScale, dScale));
    CMatrix4 mBack = CMatrix4::makeTranslation(vCamera);

    mToOrigin = mToOrigin * mScale;

    return mToOrigin * mBack;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns S(k) * \a mView, the render path application of the same transform. \br\br
    The view origin is the camera position, so a view-space uniform scale equals the
    world-space scale about the camera, with unchanged orientation and no per-object path.
*/
QMatrix4x4 CRenderScalePolicy::scaledViewMatrix(const QMatrix4x4& mView, double dScale)
{
    QMatrix4x4 mScale;
    mScale.setToIdentity();
    mScale.scale(dScale, dScale, dScale);

    return mScale * mView;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns S(k) * \a mView on the internal double-precision matrix. \br\br
    The internal matrices compose row-vector style, the scale applies after the view.
*/
CMatrix4 CRenderScalePolicy::scaledInternalViewMatrix(const CMatrix4& mView, double dScale)
{
    CMatrix4 mScaled = mView;

    return mScaled * CMatrix4::makeScale(CVector3(dScale, dScale, dScale));
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the depth range observed through the scaled pass, real bounds times \a dScale.
*/
SRenderDepthRange CRenderScalePolicy::scaledDepthRange(const SRenderDepthRange& tRange, double dScale)
{
    return SRenderDepthRange(tRange.m_dNear * dScale, tRange.m_dFar * dScale);
}

//-------------------------------------------------------------------------------------------------
