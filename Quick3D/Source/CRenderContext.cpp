
// Application
#include "CRenderContext.h"
#include "C3DScene.h"
#include "CMaterial.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CRenderContext::CRenderContext(
        QMatrix4x4 cameraProjectionMatrix,
        QMatrix4x4 cameraMatrix,
        QMatrix4x4 shadowProjectionMatrix,
        QMatrix4x4 shadowMatrix,
        CMatrix4 internalCameraMatrix,
        CMatrix4 internalProjectionMatrix,
        C3DScene* pScene,
        CCamera* pCamera
        )
    : m_mCameraProjectionMatrix(cameraProjectionMatrix)
    , m_mCameraMatrix(cameraMatrix)
    , m_mShadowProjectionMatrix(shadowProjectionMatrix)
    , m_mShadowMatrix(shadowMatrix)
    , m_mInternalCameraMatrix(internalCameraMatrix)
    , m_mInternalProjectionMatrix(internalProjectionMatrix)
    , m_pScene(pScene)
    , m_pCamera(pCamera)
    , m_pFog(NULL)
    , bUseIR(false)
    , bUseInversePolarity(false)
    , pActiveMaterial(NULL)
    , m_iNumMeshesDrawn(0)
    , m_iNumPolysDrawn(0)
    , m_iNumChunksDrawn(0)
{
}

//-------------------------------------------------------------------------------------------------

CRenderContext::~CRenderContext()
{
}
