
#pragma once

// Qt
#include <QMutex>
#include <QStringList>

// qt-plus
#include "CXMLNode.h"

// Application
#include "quick3d_global.h"
#include "CVector3.h"
#include "CGeoloc.h"

/*

    SRTM tile data storage class
    Tile coordinate reference

    m_gSize.Longitude
    <-------------->

    ----------------  ^
    |              |  |
    |              |  |
    |  m_gGeoloc   |  |
    |      .       |  | m_gSize.Latitude
    |              |  |
    |              |  |
    |              |  |
    ----------------  v

*/

//! SRTM tile data storage class
class QUICK3D_EXPORT CSRTMData
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Default constructor
    CSRTMData(double dValueForNoData = 0.0);

    //! Constuctor with parameters
    CSRTMData(CGeoloc gGeoloc, CGeoloc gSize, int iNumCellsWidth, int iNumCellsHeight, qint16 ui16NoDataValue, const qint16* vData, double dValueForNoData = -2000.0);

    //! Copy constructor
    CSRTMData(const CSRTMData& target);

    //! Destructor
    virtual ~CSRTMData();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! Defines the file name of the tile
    void setFileName(QString value);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns the geolocalized center of the tile
    CGeoloc getGeoloc() const { return m_gGeoloc; }

    //! Returns the size of the tile in decimal degrees
    CGeoloc getSize() const { return m_gSize; }

    //! Returns the altitude at the specified geolocation
    double getHeightAt(const CGeoloc& gPosition, double* pRigidness = nullptr);

    //! Returns the number of cells on the width axis
    int getNumCellsWidth() const { return m_iNumCellsWidth; }

    //! Returns the number of cells on the height axis
    int getNumCellsHeight() const { return m_iNumCellsHeight; }

    //! Returns a constant pointer to the elevation data
    const qint16* getData() const { return m_vData; }

    //! Returns a pointer to the elevation data
    qint16* getData() { return m_vData; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Returns \c true if the tile contains the specified geolocation
    bool contains(const CGeoloc& gPosition) const;

    //! Assign operator
    CSRTMData& operator = (const CSRTMData& target);

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //! Reads the header of the m_sFileName file
    QStringList readSRTMHeader();

    //! Reads the data of the m_sFileName file
    void readSRTMData();

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QMutex      m_tMutex;
    double      m_dValueForNoData;
    QString     m_sFileName;
    CGeoloc     m_gGeoloc;
    CGeoloc     m_gSize;
    int         m_iNumCellsWidth;
    int         m_iNumCellsHeight;
    short       m_ui16NoDataValue;
    qint16*     m_vData;
};
