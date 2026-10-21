
#pragma once

// Application
#include "CTestFixture.h"

//-------------------------------------------------------------------------------------------------

class CTestRenderLayer : public CTestFixture
{
    Q_OBJECT

public:

    explicit CTestRenderLayer();

private slots:

    //! Layer bits match their identifiers and combine into the all mask
    void testLayerBitMatchesIdentifier();

    //! Default components span both layers, terrain near the camera is never filtered out
    void testDefaultComponentSpansBothLayers();

    //! A far-only component is rejected by the close filter
    void testFarOnlyComponentSkippedByCloseFilter();

    //! A close-only component is rejected by the far filter
    void testCloseOnlyComponentSkippedByFarFilter();

    //! The altitude conditioner keeps a tight far/near ratio at altitude
    void testAltitudeConditionerKeepsTightRatio();

    //! The altitude conditioner clamps to minimums at ground level
    void testConditionerClampsToMinimums();

    //! The conditioned stack covers every distance without gap
    void testStackCoversEveryDistanceWithoutGap();

    //! The stack paints the farthest layer first
    void testStackPaintOrderIsFarFirst();
};

//-------------------------------------------------------------------------------------------------
