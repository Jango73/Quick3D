
//-------------------------------------------------------------------------------------------------
/*!
    \file CRenderLayer.h
    \brief Scene-declared render layers with pluggable filters and depth conditioners.
    \inmodule Quick3D
*/
//-------------------------------------------------------------------------------------------------

#pragma once

// Qt
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QVector>

// qt-plus
#include "CXMLNode.h"

// Application
#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------
// Layer description vocabulary, values of the Filter and Conditioner attributes

#define LayerFilterType_Layers                  "Layers"
#define LayerFilterType_Tag                     "Tag"
#define LayerFilterType_Class                   "Class"
#define LayerConditionerType_AltitudeRatio      "AltitudeRatio"
#define LayerConditionerType_OverlapReference   "OverlapReference"
#define LayerConditionerType_Fixed              "Fixed"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class CComponent;

//-------------------------------------------------------------------------------------------------

//! Maximum light count declared by a layer without a cap, the full scene light list uploads
constexpr int RENDER_LAYER_UNLIMITED_LIGHTS = -1;

//-------------------------------------------------------------------------------------------------

//! Default near divisor of the altitude driven conditioner, the near plane follows the altitude through this divisor
constexpr double RENDER_FAR_NEAR_ALTITUDE_DIVISOR = 8.0;

//! Default far factor of the altitude driven conditioner, the far plane follows the altitude through this factor
constexpr double RENDER_FAR_FAR_ALTITUDE_FACTOR = 40.0;

//! Default far clamp of the altitude driven conditioner for ground level views
constexpr double RENDER_FAR_FAR_MINIMUM = 100000.0;

//! Default foreground near plane of the overlap conditioner, the cockpit distance
constexpr double RENDER_CLOSE_NEAR_DISTANCE = 0.1;

//! Default overlap factor of the overlap conditioner, scales the background near plane to the foreground far plane
constexpr double RENDER_CLOSE_FAR_OVERLAP_FACTOR = 4.0;

//! Maximum light count declared by a far layer by default, distant patches only need the sun
constexpr int RENDER_FAR_LAYER_MAX_LIGHTS = 1;

//! Lower bound of the layer scale factor, exclusive: a zero scale collapses every vertex onto the camera
constexpr double RENDER_LAYER_SCALE_MINIMUM = 0.0;

//! Upper bound of the layer scale factor, inclusive: full-size rendering
constexpr double RENDER_LAYER_SCALE_MAXIMUM = 1.0;

//! Layer scale factor when the Scale attribute is omitted, full-size rendering
constexpr double RENDER_LAYER_SCALE_DEFAULT = 1.0;

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
    \brief Decides whether a component is painted in a render layer.
    \inmodule Quick3D
    \sa CLayersMembershipFilter
*/
class QUICK3D_EXPORT CRenderFilter
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CRenderFilter();

    //!
    virtual ~CRenderFilter();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Returns true when \a pComponent is painted in the layer named \a sLayerName
    virtual bool accept(const CComponent* pComponent, const QString& sLayerName) const = 0;
};

//-------------------------------------------------------------------------------------------------

/*!
    \class CLayersMembershipFilter
    \brief Accepts the components declaring the painted layer in their layer names.
    \inmodule Quick3D
    The membership policy lives in the components through
    CComponent::layerNames(), this filter only reads the names so layers
    never hardcode component types. A component without layer names belongs
    to every layer.
*/
class QUICK3D_EXPORT CLayersMembershipFilter : public CRenderFilter
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CLayersMembershipFilter();

    //!
    virtual ~CLayersMembershipFilter() Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Overridden methods
    //-------------------------------------------------------------------------------------------------

    //! Returns true when \a pComponent declares \a sLayerName or declares no layer
    virtual bool accept(const CComponent* pComponent, const QString& sLayerName) const Q_DECL_OVERRIDE;
};

//-------------------------------------------------------------------------------------------------

/*!
    \class CTagRenderFilter
    \brief Accepts the components carrying one user defined tag.
    \inmodule Quick3D
*/
class QUICK3D_EXPORT CTagRenderFilter : public CRenderFilter
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Builds a filter accepting the \a sTag tag
    explicit CTagRenderFilter(const QString& sTag);

    //!
    virtual ~CTagRenderFilter() Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Overridden methods
    //-------------------------------------------------------------------------------------------------

    //! Returns true when \a pComponent carries the filtered tag
    virtual bool accept(const CComponent* pComponent, const QString& sLayerName) const Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QString   m_sTag;
};

//-------------------------------------------------------------------------------------------------

/*!
    \class CClassNameRenderFilter
    \brief Accepts the components of one class name.
    \inmodule Quick3D
*/
class QUICK3D_EXPORT CClassNameRenderFilter : public CRenderFilter
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Builds a filter accepting the \a sClassName class name
    explicit CClassNameRenderFilter(const QString& sClassName);

    //!
    virtual ~CClassNameRenderFilter() Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Overridden methods
    //-------------------------------------------------------------------------------------------------

    //! Returns true when \a pComponent class name matches the filtered class name
    virtual bool accept(const CComponent* pComponent, const QString& sLayerName) const Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QString   m_sClassName;
};

//-------------------------------------------------------------------------------------------------

/*!
    \class CRenderFilterFactory
    \brief Instantiates a render filter from a layer XML node.
    \inmodule Quick3D
    The factory returns null for unknown filter types, callers null-check before use.
*/
class QUICK3D_EXPORT CRenderFilterFactory
{
public:

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Returns the filter described by \a xLayer, null when the Filter attribute names an unknown type
    static QSharedPointer<CRenderFilter> create(const CXMLNode& xLayer);
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

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CDepthRangeConditioner();

    //!
    virtual ~CDepthRangeConditioner();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

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

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Builds a conditioner with the given tuning, defaults suit a background layer
    explicit CAltitudeRatioDepthConditioner(
            double dNearAltitudeDivisor = RENDER_FAR_NEAR_ALTITUDE_DIVISOR,
            double dFarAltitudeFactor = RENDER_FAR_FAR_ALTITUDE_FACTOR,
            double dMinimumNear = RENDER_CLOSE_NEAR_DISTANCE,
            double dMinimumFar = RENDER_FAR_FAR_MINIMUM
            );

    //-------------------------------------------------------------------------------------------------
    // Overridden methods
    //-------------------------------------------------------------------------------------------------

    //! Returns the altitude driven range for \a tContext
    virtual SRenderDepthRange condition(const SDepthConditioningContext& tContext) const Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    double    m_dNearAltitudeDivisor;
    double    m_dFarAltitudeFactor;
    double    m_dMinimumNear;
    double    m_dMinimumFar;
};

//-------------------------------------------------------------------------------------------------

/*!
    \class COverlapDepthConditioner
    \brief Covers the foreground up to a multiple of the reference near plane.
    \inmodule Quick3D
    The reference near plane is the background layer near plane conditioned for the
    same frame: the foreground far plane always exceeds it, so both ranges overlap
    and no distance is left without a layer.
*/
class QUICK3D_EXPORT COverlapDepthConditioner : public CDepthRangeConditioner
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Builds a conditioner with the given tuning, defaults suit a foreground layer
    explicit COverlapDepthConditioner(
            double dNearDistance = RENDER_CLOSE_NEAR_DISTANCE,
            double dOverlapFactor = RENDER_CLOSE_FAR_OVERLAP_FACTOR
            );

    //-------------------------------------------------------------------------------------------------
    // Overridden methods
    //-------------------------------------------------------------------------------------------------

    //! Returns the foreground range overlapping \a tContext reference near plane
    virtual SRenderDepthRange condition(const SDepthConditioningContext& tContext) const Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    double    m_dNearDistance;
    double    m_dOverlapFactor;
};

//-------------------------------------------------------------------------------------------------

/*!
    \class CFixedDepthConditioner
    \brief Returns one constant depth range for every frame.
    \inmodule Quick3D
*/
class QUICK3D_EXPORT CFixedDepthConditioner : public CDepthRangeConditioner
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Builds a conditioner always returning the range \a dNear to \a dFar
    CFixedDepthConditioner(double dNear, double dFar);

    //-------------------------------------------------------------------------------------------------
    // Overridden methods
    //-------------------------------------------------------------------------------------------------

    //! Returns the fixed range, whatever \a tContext holds
    virtual SRenderDepthRange condition(const SDepthConditioningContext& tContext) const Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    double    m_dNear;
    double    m_dFar;
};

//-------------------------------------------------------------------------------------------------

/*!
    \class CDepthConditionerFactory
    \brief Instantiates a depth conditioner from a layer XML node.
    \inmodule Quick3D
    The factory returns null for unknown conditioner types, callers null-check before use.
*/
class QUICK3D_EXPORT CDepthConditionerFactory
{
public:

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Returns the conditioner described by \a xLayer, null when the Conditioner attribute names an unknown type
    static QSharedPointer<CDepthRangeConditioner> create(const CXMLNode& xLayer);
};

//-------------------------------------------------------------------------------------------------

/*!
    \class CRenderLayer
    \brief One paint pass: a membership predicate plus a depth conditioner.
    \inmodule Quick3D
    The layer holds no component policy and no depth policy, both are
    injected as objects. Explicit members named in the layer XML node join
    the filter predicate result as a union.
*/
class QUICK3D_EXPORT CRenderLayer
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Builds the layer named \a sName with \a pFilter, \a pConditioner and the given paint tuning
    CRenderLayer(
            const QString& sName,
            QSharedPointer<CRenderFilter> pFilter,
            QSharedPointer<CDepthRangeConditioner> pConditioner,
            int iMaxLights = RENDER_LAYER_UNLIMITED_LIGHTS,
            bool bClearDepth = true,
            bool bDepthTest = true,
            bool bDepthWrite = true,
            double dScale = RENDER_LAYER_SCALE_DEFAULT
            );

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns the layer name, the identifier of the layer in its stack
    QString name() const { return m_sName; }

    //! Returns the membership filter of the layer
    QSharedPointer<CRenderFilter> filter() const { return m_pFilter; }

    //! Returns the depth conditioner of the layer
    QSharedPointer<CDepthRangeConditioner> conditioner() const { return m_pConditioner; }

    //! Returns the maximum light count uploaded while this layer paints, RENDER_LAYER_UNLIMITED_LIGHTS means no cap
    int maxLights() const { return m_iMaxLights; }

    //! Returns true when the depth buffer is cleared before this layer paints
    bool clearDepth() const { return m_bClearDepth; }

    //! Returns true when the depth test runs during this layer pass
    bool depthTest() const { return m_bDepthTest; }

    //! Returns true when this layer pass writes the depth buffer
    bool depthWrite() const { return m_bDepthWrite; }

    //! Returns the reduced-size rendering factor of this layer, 1.0 means full size
    double scale() const { return m_dScale; }

    //! Returns the component names listed as explicit members of this layer
    QStringList members() const { return m_sMembers; }

    //! Returns the depth range conditioned for the current frame
    SRenderDepthRange range() const { return m_tRange; }

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! Records \a tRange as the depth range of the current frame
    void setRange(const SRenderDepthRange& tRange) { m_tRange = tRange; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Returns true when \a pComponent is painted in this layer, filter predicate or explicit member
    bool accepts(const CComponent* pComponent) const;

    //! Loads the layer from the \a xLayer XML node, false when the node is invalid
    bool loadFromXml(const CXMLNode& xLayer);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QString                                 m_sName;
    QSharedPointer<CRenderFilter>           m_pFilter;
    QSharedPointer<CDepthRangeConditioner>  m_pConditioner;
    QStringList                             m_sMembers;
    int                                     m_iMaxLights;
    bool                                    m_bClearDepth;
    bool                                    m_bDepthTest;
    bool                                    m_bDepthWrite;
    double                                  m_dScale;
    SRenderDepthRange                       m_tRange;
};

//-------------------------------------------------------------------------------------------------

/*!
    \class CRenderLayerStack
    \brief Ordered scene layers with a joint depth conditioning.
    \inmodule Quick3D
    The stack conditions every layer in paint order, each layer receiving the
    nearest near plane conditioned so far as reference, which guarantees the
    overlap: every distance is covered by at least one layer, so nothing is
    ever clipped between the planes.
*/
class QUICK3D_EXPORT CRenderLayerStack
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Builds an empty stack, layers are added with addLayer()
    CRenderLayerStack();

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns the layers in paint order, farthest first
    QVector<CRenderLayer>& layers() { return m_vLayers; }

    //! Returns the layers in paint order, farthest first
    const QVector<CRenderLayer>& layers() const { return m_vLayers; }

    //! Returns the layer names in paint order, farthest first
    QStringList layerNames() const;

    //! Returns the layer named \a sName, null when the stack holds no such layer
    const CRenderLayer* findLayer(const QString& sName) const;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Appends \a tLayer to the stack in paint order
    void addLayer(const CRenderLayer& tLayer);

    //! Loads the layers from the \a xLayers XML node, false when a layer is invalid or duplicated
    bool loadFromXml(const CXMLNode& xLayers);

    //! Conditions every layer for \a dObserverAltitude, farthest first
    void conditionLayers(double dObserverAltitude);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QVector<CRenderLayer> m_vLayers;
};

//-------------------------------------------------------------------------------------------------
