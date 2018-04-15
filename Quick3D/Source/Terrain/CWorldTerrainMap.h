
#pragma once

// Qt
#include <QImage>

// Application
#include "quick3d_global.h"
#include "CWorldTerrain.h"
#include "CInterpolator.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CWorldTerrainMap : public CComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instantiator(C3DScene* pScene);

    //! Constructor using a scene
    CWorldTerrainMap(C3DScene* pScene);

    //! Destructor
    virtual ~CWorldTerrainMap();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setTerrain(QSP<CWorldTerrain> pTerrain);

    //!
    void setImageSize(QSize sSize);

    //!
    void setCenter(CGeoloc gCenter);

    //!
    void setScale(double dScale);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    const QImage& image() const;

    //-------------------------------------------------------------------------------------------------
    // Overridden methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CWorldTerrainMap; }

    //! Loads this object's parameters
    virtual void loadParameters(const QString& sBaseFile, const CXMLNode& xComponent) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void updateImage();

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    CInterpolator<Math::CVector3>   m_iColors;
    QSP<CWorldTerrain>              m_pTerrain;
    CGeoloc                         m_gCenter;
    QImage*                         m_pImage;
    QSize                           m_sImageSize;
    double                          m_dScale;
};
