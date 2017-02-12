
#pragma once

//-------------------------------------------------------------------------------------------------

class CGeoZone
{
public:

    enum EGeoZoneFlag
    {
        gzfAll,
        gzfUnknown,
        gzfPedestrian,
        gzfTerrestrialVehicle,
        gzfSmallSeaVehicle,
        gzfBigSeaVehicle
    };

    //!
    CGeoZone()
        : m_eFlag(gzfUnknown)
    {
    }

    //!
    CGeoZone(const CGeoZone& target)
    {
        *this = target;
    }

    //!
    CGeoZone(EGeoZoneFlag eFlag)
        : m_eFlag(eFlag)
    {
    }

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setFlag(EGeoZoneFlag value) { m_eFlag = value; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    EGeoZoneFlag getFlag() const { return m_eFlag; }

    //!
    QVector<CGeoloc>& getPoints() { return m_vPoints; }

    //!
    const QVector<CGeoloc>& getPoints() const { return m_vPoints; }

    //!
    QVector<Math::CVector2>& getLocalPoints() { return m_vLocalPoints; }

    //!
    const QVector<Math::CVector2>& getLocalPoints() const { return m_vLocalPoints; }

    //-------------------------------------------------------------------------------------------------
    // Méthode de contrôle
    //-------------------------------------------------------------------------------------------------

    //!
    CGeoZone& operator = (const CGeoZone& target)
    {
        m_eFlag			= target.m_eFlag;
        m_vPoints		= target.m_vPoints;
        m_vLocalPoints	= target.m_vLocalPoints;

        return *this;
    }

    //!
    void addPoint(const CGeoloc& gPoint)
    {
        m_vPoints.append(gPoint);
    }

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    EGeoZoneFlag			m_eFlag;
    QVector<CGeoloc>	m_vPoints;
    QVector<Math::CVector2>	m_vLocalPoints;
};
