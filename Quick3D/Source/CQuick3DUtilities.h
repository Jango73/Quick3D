
#ifndef CQUICK3DUTILITIES_H
#define CQUICK3DUTILITIES_H

#include "quick3d_global.h"

// Qt
#include <QString>
#include <QImage>

// Fondations
#include "CSingleton.h"
#include "CGeoloc.h"
#include "CVector2.h"

// Application
#include "CSRTMData.h"
#include "CGeoZone.h"
#include "CCamera.h"
#include "CImageUtilities.h"
#include "IProgressListener.h"
#include "CGLWidgetScene.h"
#include "CAutoTerrain.h"
#include "CSRTMField.h"
#include "CBILField.h"
#include "CCamera.h"

//-------------------------------------------------------------------------------------------------

#define INT_TO_RGB24(a,b,c)	( (a & 0xFF) | ((b & 0xFF) << 8) | ((c & 0xFF) << 16) )
#define INT_TO_BGR24(a,b,c)	( (c & 0xFF) | ((b & 0xFF) << 8) | ((a & 0xFF) << 16) )

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CQuick3DUtilities : public CSingleton<CQuick3DUtilities>
{
    friend class CSingleton<CQuick3DUtilities>;

public:

    //!
    void prepareSceneForMatrices_CubeMapped(CPanoramicMatrixParams& tParams);

    //!
    void computeSphericalDetectionMatrices_RayTraced(
            CPanoramicMatrixParams& tParams,
            IProgressListener* pProgressListener
            );

    //!
    void computeSphericalDetectionMatrices_CubeMapped(
            CPanoramicMatrixParams& tParams,
            IProgressListener* pProgressListener
            );

    //!
    void computeMatrixImages(CPanoramicMatrixParams& tParams);

protected:

    //! Conctructeur
    CQuick3DUtilities();

    //! Destructeur
    virtual ~CQuick3DUtilities();

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

    CGLWidgetScene*     m_pCMScene;
    CHeightField*       m_pCMField;
    QSP<CAutoTerrain>   m_pCMTerrain;
    QSP<CCamera>        m_pCMCamera;
};

#endif // CQUICK3DUTILITIES_H
