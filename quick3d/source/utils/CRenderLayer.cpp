
// qt-plus
#include "CLogger.h"

// Application
#include "CRenderLayer.h"
#include "CComponent.h"
#include "CQ3DConstants.h"

//-------------------------------------------------------------------------------------------------

/*!
    Reads the \a sName attribute of \a xNode as a double. \br\br
    Missing attributes resolve to \a dDefault, present but non-numeric attributes
    fail. The outcome lands in \a dValue.
*/
static bool readDoubleAttribute(const CXMLNode& xNode, const QString& sName, double dDefault, double& dValue)
{
    QString sText = xNode.attributes()[sName];

    if (sText.isEmpty() == true)
    {
        dValue = dDefault;

        return true;
    }

    bool bConverted = false;
    double dParsed = sText.toDouble(&bConverted);

    if (bConverted == false)
    {
        return false;
    }

    dValue = dParsed;

    return true;
}

//-------------------------------------------------------------------------------------------------

/*!
    Reads the \a sName attribute of \a xNode as a boolean. \br\br
    Missing attributes resolve to \a bDefault. \c true, \c 1, \c false and \c 0
    are accepted (case-insensitive for the words), any other text fails.
    The outcome lands in \a bValue.
*/
static bool readBoolAttribute(const CXMLNode& xNode, const QString& sName, bool bDefault, bool& bValue)
{
    QString sText = xNode.attributes()[sName];

    if (sText.isEmpty() == true)
    {
        bValue = bDefault;

        return true;
    }

    if (sText.compare(QString("true"), Qt::CaseInsensitive) == 0 || sText == QString("1"))
    {
        bValue = true;

        return true;
    }

    if (sText.compare(QString("false"), Qt::CaseInsensitive) == 0 || sText == QString("0"))
    {
        bValue = false;

        return true;
    }

    return false;
}

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
    Builds a filter.
*/
CRenderFilter::CRenderFilter()
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
    Builds a membership filter.
*/
CLayersMembershipFilter::CLayersMembershipFilter()
    : CRenderFilter()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys the filter.
*/
CLayersMembershipFilter::~CLayersMembershipFilter()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns true when \a pComponent declares \a sLayerName or declares no layer.
*/
bool CLayersMembershipFilter::accept(const CComponent* pComponent, const QString& sLayerName) const
{
    if (pComponent == nullptr) return false;

    if (pComponent->layerNames().isEmpty() == true) return true;

    return pComponent->layerNames().contains(sLayerName);
}

//-------------------------------------------------------------------------------------------------

/*!
    Builds a filter accepting the \a sTag tag.
*/
CTagRenderFilter::CTagRenderFilter(const QString& sTag)
    : CRenderFilter()
    , m_sTag(sTag)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys the filter.
*/
CTagRenderFilter::~CTagRenderFilter()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns true when \a pComponent carries the filtered tag.
*/
bool CTagRenderFilter::accept(const CComponent* pComponent, const QString& sLayerName) const
{
    Q_UNUSED(sLayerName);

    if (pComponent == nullptr) return false;

    return pComponent->tag() == m_sTag;
}

//-------------------------------------------------------------------------------------------------

/*!
    Builds a filter accepting the \a sClassName class name.
*/
CClassNameRenderFilter::CClassNameRenderFilter(const QString& sClassName)
    : CRenderFilter()
    , m_sClassName(sClassName)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys the filter.
*/
CClassNameRenderFilter::~CClassNameRenderFilter()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns true when \a pComponent class name matches the filtered class name.
*/
bool CClassNameRenderFilter::accept(const CComponent* pComponent, const QString& sLayerName) const
{
    Q_UNUSED(sLayerName);

    if (pComponent == nullptr) return false;

    return pComponent->getClassName() == m_sClassName;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the filter described by \a xLayer, null when the Filter attribute names an unknown type.
*/
QSharedPointer<CRenderFilter> CRenderFilterFactory::create(const CXMLNode& xLayer)
{
    QString sType = xLayer.attributes()[ParamName_Filter];

    if (sType.isEmpty() == true)
    {
        sType = QString(LayerFilterType_Layers);
    }

    if (sType == QString(LayerFilterType_Layers))
    {
        return QSharedPointer<CRenderFilter>(new CLayersMembershipFilter());
    }

    if (sType == QString(LayerFilterType_Tag))
    {
        QString sTag = xLayer.attributes()[ParamName_Tag];

        if (sTag.isEmpty() == true)
        {
            LOG_ERROR(QString("Layer %1 uses a Tag filter without a Tag attribute").arg(xLayer.attributes()[ParamName_Name]));
            LOG_DEBUG(QString("Layer node: %1").arg(xLayer.toString()));

            return QSharedPointer<CRenderFilter>(nullptr);
        }

        return QSharedPointer<CRenderFilter>(new CTagRenderFilter(sTag));
    }

    if (sType == QString(LayerFilterType_Class))
    {
        QString sClassName = xLayer.attributes()[ParamName_Class];

        if (sClassName.isEmpty() == true)
        {
            LOG_ERROR(QString("Layer %1 uses a Class filter without a Class attribute").arg(xLayer.attributes()[ParamName_Name]));
            LOG_DEBUG(QString("Layer node: %1").arg(xLayer.toString()));

            return QSharedPointer<CRenderFilter>(nullptr);
        }

        return QSharedPointer<CRenderFilter>(new CClassNameRenderFilter(sClassName));
    }

    LOG_ERROR(QString("Unknown layer filter type %1").arg(sType));
    LOG_DEBUG(QString("Layer node: %1").arg(xLayer.toString()));

    return QSharedPointer<CRenderFilter>(nullptr);
}

//-------------------------------------------------------------------------------------------------

/*!
    Builds a conditioner.
*/
CDepthRangeConditioner::CDepthRangeConditioner()
{
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
    Builds a conditioner with the given tuning. \br\br
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
    : CDepthRangeConditioner()
    , m_dNearAltitudeDivisor(dNearAltitudeDivisor)
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
    Builds a conditioner with the given tuning. \br\br
    \a dNearDistance is the fixed foreground near plane. \br
    \a dOverlapFactor scales the reference near plane to the foreground far plane.
*/
COverlapDepthConditioner::COverlapDepthConditioner(double dNearDistance, double dOverlapFactor)
    : CDepthRangeConditioner()
    , m_dNearDistance(dNearDistance)
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
    Builds a conditioner always returning the range \a dNear to \a dFar.
*/
CFixedDepthConditioner::CFixedDepthConditioner(double dNear, double dFar)
    : CDepthRangeConditioner()
    , m_dNear(dNear)
    , m_dFar(dFar)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the fixed range, whatever \a tContext holds.
*/
SRenderDepthRange CFixedDepthConditioner::condition(const SDepthConditioningContext& tContext) const
{
    Q_UNUSED(tContext);

    return SRenderDepthRange(m_dNear, m_dFar);
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the conditioner described by \a xLayer, null when the Conditioner attribute names an unknown type.
*/
QSharedPointer<CDepthRangeConditioner> CDepthConditionerFactory::create(const CXMLNode& xLayer)
{
    QString sType = xLayer.attributes()[ParamName_Conditioner];

    if (sType.isEmpty() == true)
    {
        sType = QString(LayerConditionerType_AltitudeRatio);
    }

    if (sType == QString(LayerConditionerType_AltitudeRatio))
    {
        double dNearDivisor = RENDER_FAR_NEAR_ALTITUDE_DIVISOR;
        double dFarFactor = RENDER_FAR_FAR_ALTITUDE_FACTOR;
        double dMinimumNear = RENDER_CLOSE_NEAR_DISTANCE;
        double dMinimumFar = RENDER_FAR_FAR_MINIMUM;

        if (readDoubleAttribute(xLayer, ParamName_NearDivisor, dNearDivisor, dNearDivisor) == false
                || readDoubleAttribute(xLayer, ParamName_FarFactor, dFarFactor, dFarFactor) == false
                || readDoubleAttribute(xLayer, ParamName_MinNear, dMinimumNear, dMinimumNear) == false
                || readDoubleAttribute(xLayer, ParamName_MinFar, dMinimumFar, dMinimumFar) == false)
        {
            LOG_ERROR(QString("Layer %1 holds a non-numeric altitude conditioner attribute").arg(xLayer.attributes()[ParamName_Name]));
            LOG_DEBUG(QString("Layer node: %1").arg(xLayer.toString()));

            return QSharedPointer<CDepthRangeConditioner>(nullptr);
        }

        return QSharedPointer<CDepthRangeConditioner>(new CAltitudeRatioDepthConditioner(dNearDivisor, dFarFactor, dMinimumNear, dMinimumFar));
    }

    if (sType == QString(LayerConditionerType_OverlapReference))
    {
        double dNear = RENDER_CLOSE_NEAR_DISTANCE;
        double dOverlapFactor = RENDER_CLOSE_FAR_OVERLAP_FACTOR;

        if (readDoubleAttribute(xLayer, ParamName_Near, dNear, dNear) == false
                || readDoubleAttribute(xLayer, ParamName_OverlapFactor, dOverlapFactor, dOverlapFactor) == false)
        {
            LOG_ERROR(QString("Layer %1 holds a non-numeric overlap conditioner attribute").arg(xLayer.attributes()[ParamName_Name]));
            LOG_DEBUG(QString("Layer node: %1").arg(xLayer.toString()));

            return QSharedPointer<CDepthRangeConditioner>(nullptr);
        }

        return QSharedPointer<CDepthRangeConditioner>(new COverlapDepthConditioner(dNear, dOverlapFactor));
    }

    if (sType == QString(LayerConditionerType_Fixed))
    {
        double dNear = 0.0;
        double dFar = 0.0;

        bool bNearPresent = xLayer.attributes().contains(ParamName_Near);
        bool bFarPresent = xLayer.attributes().contains(ParamName_Far);

        if (bNearPresent == false || bFarPresent == false
                || readDoubleAttribute(xLayer, ParamName_Near, dNear, dNear) == false
                || readDoubleAttribute(xLayer, ParamName_Far, dFar, dFar) == false
                || dFar <= dNear)
        {
            LOG_ERROR(QString("Layer %1 needs numeric Near and Far attributes with Far above Near").arg(xLayer.attributes()[ParamName_Name]));
            LOG_DEBUG(QString("Layer node: %1").arg(xLayer.toString()));

            return QSharedPointer<CDepthRangeConditioner>(nullptr);
        }

        return QSharedPointer<CDepthRangeConditioner>(new CFixedDepthConditioner(dNear, dFar));
    }

    LOG_ERROR(QString("Unknown layer conditioner type %1").arg(sType));
    LOG_DEBUG(QString("Layer node: %1").arg(xLayer.toString()));

    return QSharedPointer<CDepthRangeConditioner>(nullptr);
}

//-------------------------------------------------------------------------------------------------

/*!
    Builds the layer named \a sName with \a pFilter, \a pConditioner and the given paint tuning.
*/
CRenderLayer::CRenderLayer(
        const QString& sName,
        QSharedPointer<CRenderFilter> pFilter,
        QSharedPointer<CDepthRangeConditioner> pConditioner,
        int iMaxLights,
        bool bClearDepth,
        bool bDepthTest,
        bool bDepthWrite,
        double dScale
        )
    : m_sName(sName)
    , m_pFilter(pFilter)
    , m_pConditioner(pConditioner)
    , m_iMaxLights(iMaxLights)
    , m_bClearDepth(bClearDepth)
    , m_bDepthTest(bDepthTest)
    , m_bDepthWrite(bDepthWrite)
    , m_dScale(dScale)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns true when \a pComponent is painted in this layer, filter predicate or explicit member.
*/
bool CRenderLayer::accepts(const CComponent* pComponent) const
{
    if (pComponent == nullptr) return false;

    if (m_sMembers.contains(pComponent->name()) == true) return true;

    if (m_pFilter == nullptr) return false;

    return m_pFilter->accept(pComponent, m_sName);
}

//-------------------------------------------------------------------------------------------------

/*!
    Loads the layer from the \a xLayer XML node, false when the node is invalid.
*/
bool CRenderLayer::loadFromXml(const CXMLNode& xLayer)
{
    m_sName = xLayer.attributes()[ParamName_Name];

    if (m_sName.isEmpty() == true)
    {
        LOG_METHOD_ERROR(QString("Layer without a Name attribute"));
        LOG_METHOD_DEBUG(QString("Layer node: %1").arg(xLayer.toString()));

        return false;
    }

    m_pFilter = CRenderFilterFactory::create(xLayer);

    if (m_pFilter == nullptr)
    {
        return false;
    }

    m_pConditioner = CDepthConditionerFactory::create(xLayer);

    if (m_pConditioner == nullptr)
    {
        return false;
    }

    QString sMaxLights = xLayer.attributes()[ParamName_MaxLights];

    if (sMaxLights.isEmpty() == true)
    {
        m_iMaxLights = RENDER_LAYER_UNLIMITED_LIGHTS;
    }
    else
    {
        bool bConverted = false;
        int iMaxLights = sMaxLights.toInt(&bConverted);

        if (bConverted == false)
        {
            LOG_METHOD_ERROR(QString("Layer %1 holds a non-numeric MaxLights attribute").arg(m_sName));
            LOG_METHOD_DEBUG(QString("Layer node: %1").arg(xLayer.toString()));

            return false;
        }

        m_iMaxLights = iMaxLights;
    }

    if (readBoolAttribute(xLayer, ParamName_ClearDepth, true, m_bClearDepth) == false
            || readBoolAttribute(xLayer, ParamName_DepthTest, true, m_bDepthTest) == false
            || readBoolAttribute(xLayer, ParamName_DepthWrite, true, m_bDepthWrite) == false)
    {
        LOG_METHOD_ERROR(QString("Layer %1 holds a non-boolean depth attribute").arg(m_sName));
        LOG_METHOD_DEBUG(QString("Layer node: %1").arg(xLayer.toString()));

        return false;
    }

    if (readDoubleAttribute(xLayer, ParamName_Scale, RENDER_LAYER_SCALE_DEFAULT, m_dScale) == false
            || m_dScale <= RENDER_LAYER_SCALE_MINIMUM
            || m_dScale > RENDER_LAYER_SCALE_MAXIMUM)
    {
        LOG_METHOD_ERROR(QString("Layer %1 needs a Scale attribute inside (0, 1]").arg(m_sName));
        LOG_METHOD_DEBUG(QString("Layer node: %1").arg(xLayer.toString()));

        return false;
    }

    m_sMembers.clear();

    QVector<CXMLNode> xMembers = xLayer.getNodesByTagName(ParamName_Member);

    foreach (const CXMLNode& xMember, xMembers)
    {
        QString sMember = xMember.attributes()[ParamName_Name];

        if (sMember.isEmpty() == true)
        {
            LOG_METHOD_ERROR(QString("Layer %1 holds a Member without a Name attribute").arg(m_sName));
            LOG_METHOD_DEBUG(QString("Layer node: %1").arg(xLayer.toString()));

            return false;
        }

        m_sMembers.append(sMember);
    }

    return true;
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
    Returns the layer names in paint order, farthest first.
*/
QStringList CRenderLayerStack::layerNames() const
{
    QStringList sNames;

    foreach (const CRenderLayer& tLayer, m_vLayers)
    {
        sNames.append(tLayer.name());
    }

    return sNames;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the layer named \a sName, null when the stack holds no such layer.
*/
const CRenderLayer* CRenderLayerStack::findLayer(const QString& sName) const
{
    foreach (const CRenderLayer& tLayer, m_vLayers)
    {
        if (tLayer.name() == sName)
        {
            return &tLayer;
        }
    }

    return nullptr;
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
    Loads the layers from the \a xLayers XML node, false when a layer is invalid or duplicated.
*/
bool CRenderLayerStack::loadFromXml(const CXMLNode& xLayers)
{
    QVector<CXMLNode> xLayerNodes = xLayers.getNodesByTagName(ParamName_Layer);

    if (xLayerNodes.isEmpty() == true)
    {
        LOG_METHOD_ERROR(QString("Layers node holds no Layer child"));
        LOG_METHOD_DEBUG(QString("Layers node: %1").arg(xLayers.toString()));

        return false;
    }

    CRenderLayerStack tStack;

    foreach (const CXMLNode& xLayerNode, xLayerNodes)
    {
        CRenderLayer tLayer(
                QString(""),
                QSharedPointer<CRenderFilter>(nullptr),
                QSharedPointer<CDepthRangeConditioner>(nullptr)
                );

        if (tLayer.loadFromXml(xLayerNode) == false)
        {
            return false;
        }

        if (tStack.findLayer(tLayer.name()) != nullptr)
        {
            LOG_METHOD_ERROR(QString("Duplicated layer name %1").arg(tLayer.name()));
            LOG_METHOD_DEBUG(QString("Layers node: %1").arg(xLayers.toString()));

            return false;
        }

        tStack.addLayer(tLayer);
    }

    m_vLayers = tStack.layers();

    return true;
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
