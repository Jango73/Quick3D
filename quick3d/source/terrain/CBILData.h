
#pragma once

#include "quick3d_global.h"

// Qt
#include <QMutex>
#include <QStringList>
#include <QDateTime>

// Fondations
#include "CVector3.h"
#include "CGeoloc.h"
#include "CXMLNode.h"
#include "CExpendable.h"

/*

    BIL tile data storage class
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

//! BIL tile data storage class
class QUICK3D_EXPORT CBILData : public CExpendable
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Default constructor
    CBILData(double dValueForNoData = 0.0);

    //! Constuctor with parameters
    CBILData(CGeoloc gGeoloc, CGeoloc gSize, int iNumCellsWidth, int iNumCellsHeight, qint16 ui16NoDataValue, const qint16* vData, double dValueForNoData = -2000.0);

    //! Copy constructor
    CBILData(const CBILData& target);

    //! Destructor
    virtual ~CBILData();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! Defines the file name of the tile
    void setFileName(QString value);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns the geolocalized center of the tile
    CGeoloc geoloc() const { return m_gGeoloc; }

    //! Returns the size of the tile in decimal degrees
    CGeoloc size() const { return m_gSize; }

    //! Returns the altitude at the specified geolocation
    double getHeightAt(const CGeoloc& gPosition, double* pRigidness = nullptr);

    //! Returns the number of cells on the width axis
    int numCellsWidth() const { return m_iNumCellsWidth; }

    //! Returns the number of cells on the height axis
    int numCellsHeight() const { return m_iNumCellsHeight; }

    //! Returns a constant pointer to the elevation data
    const qint16* data() const { return m_vData; }

    //! Returns a pointer to the elevation data
    qint16* data() { return m_vData; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Returns \c true if the tile contains the specified geolocation
    bool contains(const CGeoloc& gPosition) const;

    //! Assign operator
    CBILData& operator = (const CBILData& target);

    //!
    void clearData();

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //! Reads the header of the m_sFileName file
    QStringList readHeader();

    //! Reads the data of the m_sFileName file
    void readData();

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QMutex          m_tMutex;
    double          m_dValueForNoData;
    QString         m_sFileName;
    QString         m_sBILFileName;
    CGeoloc         m_gGeoloc;
    CGeoloc         m_gSize;
    int             m_iNumCellsWidth;
    int             m_iNumCellsHeight;
    short           m_ui16NoDataValue;
    qint16*         m_vData;
};
