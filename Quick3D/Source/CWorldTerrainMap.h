
#pragma once

// Qt
#include <QImage>

// Application
#include "quick3d_global.h"
#include "CWorldTerrain.h"

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
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void updateImage();

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QSP<CWorldTerrain>  m_pTerrain;
    QImage*             m_pImage;
    QSize               m_sImageSize;
};
