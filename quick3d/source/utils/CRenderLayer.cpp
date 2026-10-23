
// Application
#include "CRenderLayer.h"
#include "CComponent.h"

//-------------------------------------------------------------------------------------------------

/*!
    Builds an empty range, invalid until conditioned.
*/
SRenderDepthRange::SRenderDepthRange()
    : m_dNear(0.0)
    , m_dFar(0.0)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Builds the range \a dNear to \a dFar.
*/
SRenderDepthRange::SRenderDepthRange(double dNear, double dFar)
    : m_dNear(dNear)
    , m_dFar(dFar)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns true when far exceeds near.
*/
bool SRenderDepthRange::isValid() const
{
    return m_dFar > m_dNear;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns true when \a dDistance falls inside the range.
*/
bool SRenderDepthRange::covers(double dDistance) const
{
    return dDistance >= m_dNear && dDistance <= m_dFar;
}

//-------------------------------------------------------------------------------------------------

/*!
    Builds a context for \a dObserverAltitude with no reference range.
*/
SDepthConditioningContext::SDepthConditioningContext(double dObserverAltitude)
    : m_dObserverAltitude(dObserverAltitude)
    , m_dReferenceNear(0.0)
    , m_dReferenceFar(0.0)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys the filter.
*/
CRenderFilter::~CRenderFilter()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Builds a filter accepting \a iLayer.
*/
CLayerMaskFilter::CLayerMaskFilter(int iLayer)
    : m_iLayer(iLayer)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the mask bit of \a iLayer.
*/
int CLayerMaskFilter::layerBit(int iLayer)
{
    return (1 << iLayer);
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns true when \a pComponent declares the filtered layer.
*/
bool CLayerMaskFilter::accept(const CComponent* pComponent) const
{
    if (pComponent == nullptr) return false;

    return (pComponent->renderLayers() & layerBit(m_iLayer)) != 0;
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys the conditioner.
*/
CDepthRangeConditioner::~CDepthRangeConditioner()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Builds a conditioner with the given tuning, defaults suit the far layer. \br\br
    \a dNearAltitudeDivisor scales the observer altitude to the near plane. \br
    \a dFarAltitudeFactor scales the observer altitude to the far plane. \br
    \a dMinimumNear clamps the near plane above zero. \br
    \a dMinimumFar clamps the far plane for ground level views.
*/
CAltitudeRatioDepthConditioner::CAltitudeRatioDepthConditioner(
        double dNearAltitudeDivisor,
        double dFarAltitudeFactor,
        double dMinimumNear,
        double dMinimumFar
        )
    : m_dNearAltitudeDivisor(dNearAltitudeDivisor)
    , m_dFarAltitudeFactor(dFarAltitudeFactor)
    , m_dMinimumNear(dMinimumNear)
    , m_dMinimumFar(dMinimumFar)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the altitude driven range for \a tContext.
*/
SRenderDepthRange CAltitudeRatioDepthConditioner::condition(const SDepthConditioningContext& tContext) const
{
    double dNear = tContext.m_dObserverAltitude / m_dNearAltitudeDivisor;

    if (dNear < m_dMinimumNear) dNear = m_dMinimumNear;

    double dFar = tContext.m_dObserverAltitude * m_dFarAltitudeFactor;

    if (dFar < m_dMinimumFar) dFar = m_dMinimumFar;

    if (dFar < dNear * 2.0) dFar = dNear * 2.0;

    return SRenderDepthRange(dNear, dFar);
}

//-------------------------------------------------------------------------------------------------

/*!
    Builds a conditioner with the given tuning, defaults suit the close layer. \br\br
    \a dNearDistance is the fixed foreground near plane. \br
    \a dOverlapFactor scales the reference near plane to the foreground far plane.
*/
COverlapDepthConditioner::COverlapDepthConditioner(double dNearDistance, double dOverlapFactor)
    : m_dNearDistance(dNearDistance)
    , m_dOverlapFactor(dOverlapFactor)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the foreground range overlapping \a tContext reference near plane.
*/
SRenderDepthRange COverlapDepthConditioner::condition(const SDepthConditioningContext& tContext) const
{
    double dFar = tContext.m_dReferenceNear * m_dOverlapFactor;

    if (dFar < m_dNearDistance * 2.0) dFar = m_dNearDistance * 2.0;

    return SRenderDepthRange(m_dNearDistance, dFar);
}

//-------------------------------------------------------------------------------------------------

/*!
    Builds the layer \a iLayer named \a sName with \a pFilter and \a pConditioner.
*/
CRenderLayer::CRenderLayer(
        int iLayer,
        const QString& sName,
        QSharedPointer<CRenderFilter> pFilter,
        QSharedPointer<CDepthRangeConditioner> pConditioner,
        int iMaxLights
        )
    : m_iLayer(iLayer)
    , m_sName(sName)
    , m_pFilter(pFilter)
    , m_pConditioner(pConditioner)
    , m_iMaxLights(iMaxLights)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Builds an empty stack, layers are added with addLayer().
*/
CRenderLayerStack::CRenderLayerStack()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Builds the standard close/far stack with default tuning.
*/
CRenderLayerStack CRenderLayerStack::createDefault()
{
    CRenderLayerStack tStack;

    tStack.addLayer(CRenderLayer(
        RENDER_LAYER_FAR,
        QString("Far"),
        QSharedPointer<CRenderFilter>(new CLayerMaskFilter(RENDER_LAYER_FAR)),
        QSharedPointer<CDepthRangeConditioner>(new CAltitudeRatioDepthConditioner()),
        RENDER_FAR_LAYER_MAX_LIGHTS
        ));

    tStack.addLayer(CRenderLayer(
        RENDER_LAYER_CLOSE,
        QString("Close"),
        QSharedPointer<CRenderFilter>(new CLayerMaskFilter(RENDER_LAYER_CLOSE)),
        QSharedPointer<CDepthRangeConditioner>(new COverlapDepthConditioner())
        ));

    return tStack;
}

//-------------------------------------------------------------------------------------------------

/*!
    Appends \a tLayer to the stack in paint order.
*/
void CRenderLayerStack::addLayer(const CRenderLayer& tLayer)
{
    m_vLayers.append(tLayer);
}

//-------------------------------------------------------------------------------------------------

/*!
    Conditions every layer for \a dObserverAltitude, farthest first. \br\br
    Each layer receives the nearest near plane conditioned so far as
    reference, so foreground layers overlap the background and no distance
    falls between two ranges.
*/
void CRenderLayerStack::conditionLayers(double dObserverAltitude)
{
    SDepthConditioningContext tContext(dObserverAltitude);

    for (int iIndex = 0; iIndex < m_vLayers.count(); iIndex++)
    {
        SRenderDepthRange tRange = m_vLayers[iIndex].conditioner()->condition(tContext);

        m_vLayers[iIndex].setRange(tRange);

        if (iIndex == 0 || tRange.m_dNear < tContext.m_dReferenceNear || tContext.m_dReferenceNear <= 0.0)
        {
            tContext.m_dReferenceNear = tRange.m_dNear;
            tContext.m_dReferenceFar = tRange.m_dFar;
        }
    }
}

//-------------------------------------------------------------------------------------------------
