
//-------------------------------------------------------------------------------------------------
/*!
    \file CRenderLayer.h
    \brief Render layers with pluggable filters and depth conditioners.
    \inmodule Quick3D
*/
//-------------------------------------------------------------------------------------------------

#pragma once

// Qt
#include <QSharedPointer>
#include <QString>
#include <QVector>

// Application
#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class CComponent;

//-------------------------------------------------------------------------------------------------
// Layer identifiers, one bit per layer in component masks

constexpr int RENDER_LAYER_CLOSE = 0;
constexpr int RENDER_LAYER_FAR = 1;
constexpr int RENDER_LAYER_COUNT = 2;
constexpr int RENDER_LAYER_BIT_CLOSE = (1 << RENDER_LAYER_CLOSE);
constexpr int RENDER_LAYER_BIT_FAR = (1 << RENDER_LAYER_FAR);
constexpr int RENDER_LAYER_ALL = (RENDER_LAYER_BIT_CLOSE | RENDER_LAYER_BIT_FAR);

//-------------------------------------------------------------------------------------------------
// Default depth tuning, shared by the standard close/far layer stack

constexpr double RENDER_FAR_NEAR_ALTITUDE_DIVISOR = 8.0;
constexpr double RENDER_FAR_FAR_ALTITUDE_FACTOR = 40.0;
constexpr double RENDER_FAR_FAR_MINIMUM = 100000.0;
constexpr double RENDER_CLOSE_NEAR_DISTANCE = 0.1;
constexpr double RENDER_CLOSE_FAR_OVERLAP_FACTOR = 4.0;

//-------------------------------------------------------------------------------------------------

/*!
    \class SRenderDepthRange
    \brief Near and far distances of one render layer.
    \inmodule Quick3D
*/
class QUICK3D_EXPORT SRenderDepthRange
{
public:

    double m_dNear;
    double m_dFar;

    //! Builds an empty range, invalid until conditioned
    SRenderDepthRange();

    //! Builds the range \a dNear to \a dFar
    SRenderDepthRange(double dNear, double dFar);

    //! Returns true when far exceeds near
    bool isValid() const;

    //! Returns true when \a dDistance falls inside the range
    bool covers(double dDistance) const;
};

//-------------------------------------------------------------------------------------------------

/*!
    \class SDepthConditioningContext
    \brief Input of a depth conditioner for one frame.
    \inmodule Quick3D
*/
class QUICK3D_EXPORT SDepthConditioningContext
{
public:

    double m_dObserverAltitude;
    double m_dReferenceNear;
    double m_dReferenceFar;

    //! Builds a context for \a dObserverAltitude with no reference range
    SDepthConditioningContext(double dObserverAltitude);
};

//-------------------------------------------------------------------------------------------------

/*!
    \class CRenderFilter
    \brief Decides whether a component belongs to a render layer.
    \inmodule Quick3D
    \sa CLayerMaskFilter
*/
class QUICK3D_EXPORT CRenderFilter
{
public:

    //! Destroys the filter
    virtual ~CRenderFilter();

    //! Returns true when \a pComponent is painted in the filtered layer
    virtual bool accept(const CComponent* pComponent) const = 0;
};

//-------------------------------------------------------------------------------------------------

/*!
    \class CLayerMaskFilter
    \brief Accepts the components whose layer mask contains one layer.
    \inmodule Quick3D
    The membership policy lives in the components through
    CComponent::renderLayers(), this filter only reads the mask so layers
    never hardcode component types.
*/
class QUICK3D_EXPORT CLayerMaskFilter : public CRenderFilter
{
public:

    //! Builds a filter accepting \a iLayer
    explicit CLayerMaskFilter(int iLayer);

    //! Returns the mask bit of \a iLayer
    static int layerBit(int iLayer);

    //! Returns true when \a pComponent declares the filtered layer
    virtual bool accept(const CComponent* pComponent) const Q_DECL_OVERRIDE;

    //! Returns the filtered layer identifier
    int layer() const { return m_iLayer; }

private:

    int m_iLayer;
};

//-------------------------------------------------------------------------------------------------

/*!
    \class CDepthRangeConditioner
    \brief Computes the depth range of one render layer for one frame.
    \inmodule Quick3D
    Conditioners only map an observation context to near/far distances,
    they never select components.
*/
class QUICK3D_EXPORT CDepthRangeConditioner
{
public:

    //! Destroys the conditioner
    virtual ~CDepthRangeConditioner();

    //! Returns the depth range for \a tContext
    virtual SRenderDepthRange condition(const SDepthConditioningContext& tContext) const = 0;
};

//-------------------------------------------------------------------------------------------------

/*!
    \class CAltitudeRatioDepthConditioner
    \brief Keeps a constant far/near ratio driven by observer altitude.
    \inmodule Quick3D
    The near plane follows the altitude through the divisor and the far plane
    through the factor, so the depth precision stays constant at any altitude
    instead of z-fighting.
*/
class QUICK3D_EXPORT CAltitudeRatioDepthConditioner : public CDepthRangeConditioner
{
public:

    //! Builds a conditioner with the given tuning, defaults suit the far layer
    explicit CAltitudeRatioDepthConditioner(
            double dNearAltitudeDivisor = RENDER_FAR_NEAR_ALTITUDE_DIVISOR,
            double dFarAltitudeFactor = RENDER_FAR_FAR_ALTITUDE_FACTOR,
            double dMinimumNear = RENDER_CLOSE_NEAR_DISTANCE,
            double dMinimumFar = RENDER_FAR_FAR_MINIMUM
            );

    //! Returns the altitude driven range for \a tContext
    virtual SRenderDepthRange condition(const SDepthConditioningContext& tContext) const Q_DECL_OVERRIDE;

private:

    double m_dNearAltitudeDivisor;
    double m_dFarAltitudeFactor;
    double m_dMinimumNear;
    double m_dMinimumFar;
};

//-------------------------------------------------------------------------------------------------

/*!
    \class COverlapDepthConditioner
    \brief Covers the foreground up to a multiple of the reference near plane.
    \inmodule Quick3D
    The reference near plane is the far layer near plane conditioned for the
    same frame: the close far plane always exceeds it, so both ranges overlap
    and no distance is left without a layer.
*/
class QUICK3D_EXPORT COverlapDepthConditioner : public CDepthRangeConditioner
{
public:

    //! Builds a conditioner with the given tuning, defaults suit the close layer
    explicit COverlapDepthConditioner(
            double dNearDistance = RENDER_CLOSE_NEAR_DISTANCE,
            double dOverlapFactor = RENDER_CLOSE_FAR_OVERLAP_FACTOR
            );

    //! Returns the foreground range overlapping \a tContext reference near plane
    virtual SRenderDepthRange condition(const SDepthConditioningContext& tContext) const Q_DECL_OVERRIDE;

private:

    double m_dNearDistance;
    double m_dOverlapFactor;
};

//-------------------------------------------------------------------------------------------------

/*!
    \class CRenderLayer
    \brief One paint pass: a filter plus a depth conditioner.
    \inmodule Quick3D
    The layer holds no component policy and no depth policy, both are
    injected as objects.
*/
class QUICK3D_EXPORT CRenderLayer
{
public:

    //! Builds the layer \a iLayer named \a sName with \a pFilter and \a pConditioner
    CRenderLayer(
            int iLayer,
            const QString& sName,
            QSharedPointer<CRenderFilter> pFilter,
            QSharedPointer<CDepthRangeConditioner> pConditioner
            );

    //! Returns the layer identifier, 0 is the closest layer
    int layer() const { return m_iLayer; }

    //! Returns the layer name
    QString name() const { return m_sName; }

    //! Returns the membership filter of the layer
    QSharedPointer<CRenderFilter> filter() const { return m_pFilter; }

    //! Returns the depth conditioner of the layer
    QSharedPointer<CDepthRangeConditioner> conditioner() const { return m_pConditioner; }

    //! Returns the depth range conditioned for the current frame
    SRenderDepthRange range() const { return m_tRange; }

    //! Records \a tRange as the depth range of the current frame
    void setRange(const SRenderDepthRange& tRange) { m_tRange = tRange; }

private:

    int                                     m_iLayer;
    QString                                 m_sName;
    QSharedPointer<CRenderFilter>           m_pFilter;
    QSharedPointer<CDepthRangeConditioner>  m_pConditioner;
    SRenderDepthRange                       m_tRange;
};

//-------------------------------------------------------------------------------------------------

/*!
    \class CRenderLayerStack
    \brief Ordered close/far layers with a joint depth conditioning.
    \inmodule Quick3D
    The stack conditions the far layer first, then the close layer with the
    nearest conditioned near plane as reference, which guarantees the
    overlap: every distance is covered by at least one layer, so nothing is
    ever clipped between the planes.
*/
class QUICK3D_EXPORT CRenderLayerStack
{
public:

    //! Builds an empty stack, layers are added with addLayer()
    CRenderLayerStack();

    //! Builds the standard close/far stack with default tuning
    static CRenderLayerStack createDefault();

    //! Appends \a tLayer to the stack in paint order
    void addLayer(const CRenderLayer& tLayer);

    //! Returns the layers in paint order, farthest first
    QVector<CRenderLayer>& layers() { return m_vLayers; }

    //! Returns the layers in paint order, farthest first
    const QVector<CRenderLayer>& layers() const { return m_vLayers; }

    //! Conditions every layer for \a dObserverAltitude, farthest first
    void conditionLayers(double dObserverAltitude);

private:

    QVector<CRenderLayer> m_vLayers;
};

//-------------------------------------------------------------------------------------------------
