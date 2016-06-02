
#ifndef CRENDERCONTEXT_H
#define CRENDERCONTEXT_H

#include "quick3d_global.h"

// Qt
#include <QPainter>
#include <QMatrix4x4>

// Fondations
#include "CVector3.h"
#include "CMatrix4.h"

// Application
#include "CShaderCollection.h"
#include "CFog.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;
class CMaterial;
class CCamera;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CRenderContext
{
public:

	//!
	CRenderContext(
		QMatrix4x4 cameraProjectionMatrix,
		QMatrix4x4 cameraMatrix,
		QMatrix4x4 shadowProjectionMatrix,
		QMatrix4x4 shadowMatrix,
		Math::CMatrix4 internalCameraMatrix,
		C3DScene* pScene,
		CCamera* pCamera
		);

	//!
	~CRenderContext();

	QMatrix4x4&			cameraProjectionMatrix()	{ return m_mCameraProjectionMatrix; }
	QMatrix4x4&			cameraMatrix()				{ return m_mCameraMatrix; }
	QMatrix4x4&			shadowProjectionMatrix()	{ return m_mShadowProjectionMatrix; }
	QMatrix4x4&			shadowMatrix()				{ return m_mShadowMatrix; }
	Math::CMatrix4&		internalCameraMatrix()		{ return m_mInternalCameraMatrix; }
	C3DScene*			scene()						{ return m_pScene; }
	CCamera*			camera()					{ return m_pCamera; }
	CFog*				fog()						{ return m_pFog; }

	bool				bUseIR;
	bool				bUseInversePolarity;
	CMaterial*			pActiveMaterial;

	//! Statistiques
	int					m_iNumMeshesDrawn;
	int					m_iNumPolysDrawn;
	int					m_iNumChunksDrawn;

protected:

	QMatrix4x4			m_mCameraProjectionMatrix;
	QMatrix4x4			m_mCameraMatrix;
	QMatrix4x4			m_mShadowProjectionMatrix;
	QMatrix4x4			m_mShadowMatrix;
	Math::CMatrix4		m_mInternalCameraMatrix;
	C3DScene*			m_pScene;
	CCamera*			m_pCamera;
	CFog*				m_pFog;
};

#endif // CRENDERCONTEXT_H
