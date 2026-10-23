
#pragma once

// Qt
#include <QString>

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CVector3.h"

//-------------------------------------------------------------------------------------------------

class CXMLNode;
class CSkyModel;

//-------------------------------------------------------------------------------------------------

/*!
    Value object holding the rendering environment of a scene.

    Groups the environment settings (atmosphere altitude, fog distances,
    base colors, sky model, ozone amount, final pass contrast, saturation,
    brightness) so C3DScene stays a scene graph instead of a god-object.
    Attribute parsing, color parsing and validation live here, scene code
    only delegates.
*/
class QUICK3D_EXPORT CEnvironment
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructor, resets to defaults
    CEnvironment();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! Sets the altitude where the atmosphere factor reaches zero to \a value
    void setAtmosphereAltitude(double value) { m_dAtmosphereAltitude = value; }

    //! Sets the fog distance floor applied at maximum fog level to \a value
    void setFogMinimumDistance(double value) { m_dFogMinimumDistance = value; }

    //! Sets the fog distance applied when the camera is underwater to \a value
    void setUnderwaterFogDistance(double value) { m_dUnderwaterFogDistance = value; }

    //! Sets the global ambient light color to \a vColor
    void setGlobalAmbientColor(const Math::CVector3& vColor) { m_vGlobalAmbientColor = vColor; }

    //! Sets the base fog color to \a vColor, the daytime reference tint
    void setFogColor(const Math::CVector3& vColor) { m_vFogBaseColor = vColor; }

    //! Sets the base sky zenith color to \a vColor, the daytime reference tint
    void setSkyColor(const Math::CVector3& vColor) { m_vSkyBaseColor = vColor; }

    //! Sets the sky model matching \a sModel. Unknown names keep the current model
    void setSkyModel(const QString& sModel);

    //! Sets the ozone column amount scaling the nominal ozone absorption to \a value
    //! Negative values are clipped to zero, which disables ozone absorption.
    void setOzoneAmount(double value);

    //! Sets the final pass contrast multiplier to \a value
    void setContrast(double value) { m_dContrast = value; }

    //! Sets the final pass saturation multiplier to \a value
    void setSaturation(double value) { m_dSaturation = value; }

    //! Sets the final pass brightness multiplier to \a value
    void setBrightness(double value) { m_dBrightness = value; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns the altitude where the atmosphere factor reaches zero
    double atmosphereAltitude() const { return m_dAtmosphereAltitude; }

    //! Returns the fog distance floor applied at maximum fog level
    double fogMinimumDistance() const { return m_dFogMinimumDistance; }

    //! Returns the fog distance applied when the camera is underwater
    double underwaterFogDistance() const { return m_dUnderwaterFogDistance; }

    //! Returns the user-set global ambient light color
    Math::CVector3 globalAmbientColor() const { return m_vGlobalAmbientColor; }

    //! Returns the user-set base fog color, before sun intensity is applied
    Math::CVector3 fogColor() const { return m_vFogBaseColor; }

    //! Returns the user-set base sky zenith color, before sun intensity is applied
    Math::CVector3 skyColor() const { return m_vSkyBaseColor; }

    //! Returns the selected sky model, owned by this environment
    QSP<CSkyModel> skyModel() { return m_pSkyModel; }

    //! Returns the selected sky model, owned by this environment
    QSP<const CSkyModel> skyModel() const { return m_pSkyModel; }

    //! Returns the selected sky model name, empty when no model is set
    QString skyModelName() const;

    //! Returns the shader id matching the selected sky model, -1 when none is set
    int skyModelId() const;

    //! Returns the ozone column amount scaling the nominal ozone absorption
    double ozoneAmount() const { return m_dOzoneAmount; }

    //! Returns the final pass contrast multiplier
    double contrast() const { return m_dContrast; }

    //! Returns the final pass saturation multiplier
    double saturation() const { return m_dSaturation; }

    //! Returns the final pass brightness multiplier
    double brightness() const { return m_dBrightness; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Resets the environment to default values
    void resetToDefaults();

    //! Loads the environment from an \a xEnvironment XML node
    void loadFromXml(const CXMLNode& xEnvironment);

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //! Reads the double XML attribute \a sName of \a xNode into \a dValue, returns true when present
    static bool readDoubleAttribute(const CXMLNode& xNode, const QString& sName, double& dValue);

    //! Reads the \a sTag color child of \a xEnvironment into \a vColor
    static void readColorAttribute(const CXMLNode& xEnvironment, const QString& sTag, Math::CVector3& vColor);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    Math::CVector3   m_vGlobalAmbientColor;
    Math::CVector3   m_vFogBaseColor;
    Math::CVector3   m_vSkyBaseColor;
    double           m_dAtmosphereAltitude;
    double           m_dFogMinimumDistance;
    double           m_dUnderwaterFogDistance;
    QSP<CSkyModel>   m_pSkyModel;
    double           m_dOzoneAmount;
    double           m_dContrast;
    double           m_dSaturation;
    double           m_dBrightness;
};
