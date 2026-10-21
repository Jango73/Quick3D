
// Application
#include "CTestRenderLayer.h"
#include "CComponent.h"
#include "CRenderLayer.h"

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Constructor.
*/
/***************************************************************************/
CTestRenderLayer::CTestRenderLayer()
    : CTestFixture()
{
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Component stub declaring a single render layer.
*/
/***************************************************************************/
class CTestSingleLayerComponent : public CComponent
{
public:

    explicit CTestSingleLayerComponent(int iLayers)
        : CComponent(nullptr)
        , m_iLayers(iLayers)
    {
    }

    virtual int renderLayers() const Q_DECL_OVERRIDE { return m_iLayers; }

private:

    int m_iLayers;
};

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Layer bits match their identifiers and combine into the all mask.
*/
/***************************************************************************/
void CTestRenderLayer::testLayerBitMatchesIdentifier()
{
    QCOMPARE(CLayerMaskFilter::layerBit(RENDER_LAYER_CLOSE), RENDER_LAYER_BIT_CLOSE);
    QCOMPARE(CLayerMaskFilter::layerBit(RENDER_LAYER_FAR), RENDER_LAYER_BIT_FAR);
    QCOMPARE(RENDER_LAYER_BIT_CLOSE | RENDER_LAYER_BIT_FAR, RENDER_LAYER_ALL);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Default components span both layers, terrain near the camera is never filtered out.
*/
/***************************************************************************/
void CTestRenderLayer::testDefaultComponentSpansBothLayers()
{
    QSP<CComponent> pComponent(new CComponent(nullptr));

    QCOMPARE(pComponent->renderLayers(), RENDER_LAYER_ALL);

    CLayerMaskFilter tClose(RENDER_LAYER_CLOSE);
    CLayerMaskFilter tFar(RENDER_LAYER_FAR);

    QVERIFY(tClose.accept(pComponent.data()));
    QVERIFY(tFar.accept(pComponent.data()));
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A far-only component is rejected by the close filter.
*/
/***************************************************************************/
void CTestRenderLayer::testFarOnlyComponentSkippedByCloseFilter()
{
    QSP<CComponent> pComponent(new CTestSingleLayerComponent(RENDER_LAYER_BIT_FAR));

    CLayerMaskFilter tClose(RENDER_LAYER_CLOSE);
    CLayerMaskFilter tFar(RENDER_LAYER_FAR);

    QVERIFY(tFar.accept(pComponent.data()));
    QVERIFY(tClose.accept(pComponent.data()) == false);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A close-only component is rejected by the far filter.
*/
/***************************************************************************/
void CTestRenderLayer::testCloseOnlyComponentSkippedByFarFilter()
{
    QSP<CComponent> pComponent(new CTestSingleLayerComponent(RENDER_LAYER_BIT_CLOSE));

    CLayerMaskFilter tClose(RENDER_LAYER_CLOSE);
    CLayerMaskFilter tFar(RENDER_LAYER_FAR);

    QVERIFY(tClose.accept(pComponent.data()));
    QVERIFY(tFar.accept(pComponent.data()) == false);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief The altitude conditioner keeps a tight far/near ratio at altitude.
*/
/***************************************************************************/
void CTestRenderLayer::testAltitudeConditionerKeepsTightRatio()
{
    CAltitudeRatioDepthConditioner tConditioner;

    const double dAltitudes[] = { 25000.0, 100000.0, 400000.0 };

    for (unsigned int iIndex = 0; iIndex < sizeof(dAltitudes) / sizeof(dAltitudes[0]); iIndex++)
    {
        SDepthConditioningContext tContext(dAltitudes[iIndex]);

        SRenderDepthRange tRange = tConditioner.condition(tContext);

        QVERIFY(tRange.isValid());
        QVERIFY(qFuzzyCompare(tRange.m_dFar / tRange.m_dNear, RENDER_FAR_FAR_ALTITUDE_FACTOR * RENDER_FAR_NEAR_ALTITUDE_DIVISOR));
    }
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief The altitude conditioner clamps to minimums at ground level.
*/
/***************************************************************************/
void CTestRenderLayer::testConditionerClampsToMinimums()
{
    CAltitudeRatioDepthConditioner tConditioner;

    SDepthConditioningContext tContext(0.0);

    SRenderDepthRange tRange = tConditioner.condition(tContext);

    QVERIFY(tRange.isValid());
    QCOMPARE(tRange.m_dNear, RENDER_CLOSE_NEAR_DISTANCE);
    QCOMPARE(tRange.m_dFar, RENDER_FAR_FAR_MINIMUM);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief The conditioned stack covers every distance without gap.
*/
/***************************************************************************/
void CTestRenderLayer::testStackCoversEveryDistanceWithoutGap()
{
    const double dAltitudes[] = { 0.0, 10.0, 100.0, 5000.0, 25000.0, 200000.0 };

    for (unsigned int iIndex = 0; iIndex < sizeof(dAltitudes) / sizeof(dAltitudes[0]); iIndex++)
    {
        CRenderLayerStack tStack = CRenderLayerStack::createDefault();
        tStack.conditionLayers(dAltitudes[iIndex]);

        QCOMPARE(tStack.layers().count(), RENDER_LAYER_COUNT);

        SRenderDepthRange tFar = tStack.layers()[0].range();
        SRenderDepthRange tClose = tStack.layers()[1].range();

        QVERIFY(tFar.isValid());
        QVERIFY(tClose.isValid());

        // Foreground starts at the cockpit distance
        QCOMPARE(tClose.m_dNear, RENDER_CLOSE_NEAR_DISTANCE);

        // Foreground overlaps the background near plane, no distance is left out
        QVERIFY(tClose.m_dFar > tFar.m_dNear);

        // Union covers the full span continuously from the cockpit outward
        QVERIFY(tClose.covers(RENDER_CLOSE_NEAR_DISTANCE));
        QVERIFY(tFar.covers(tFar.m_dFar));
        QVERIFY(tClose.m_dFar >= tFar.m_dNear);
    }
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief The stack paints the farthest layer first.
*/
/***************************************************************************/
void CTestRenderLayer::testStackPaintOrderIsFarFirst()
{
    CRenderLayerStack tStack = CRenderLayerStack::createDefault();

    QCOMPARE(tStack.layers()[0].layer(), RENDER_LAYER_FAR);
    QCOMPARE(tStack.layers()[1].layer(), RENDER_LAYER_CLOSE);
}

//-------------------------------------------------------------------------------------------------
