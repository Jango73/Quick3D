
#pragma once

//-------------------------------------------------------------------------------------------------

// Qt
#include <QMutex>
#include <QFile>

// qt-plus
#include "CXMLNode.h"

// Application
#include "quick3d_global.h"
#include "CVector3.h"
#include "CGeoloc.h"

//-------------------------------------------------------------------------------------------------

/*

    HGT tile data storage class
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

//! HGT tile data storage class
class QUICK3D_EXPORT CHGTData
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Constuctor with parameters
    CHGTData(const CGeoloc& gGeoloc, const CGeoloc& gSize);

    //! Destructor
    virtual ~CHGTData();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! Defines the file name of the tile
    void setFileName(QString sFileName);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns the altitude at the specified geolocation
    double getHeightAt(const CGeoloc& gPosition, double* pRigidness = nullptr);

    //! Returns the number of cells on the width axis
    int numCellsWidth() { return m_iNumCellsWidth; }

    //! Returns the number of cells on the height axis
    int numCellsHeight() { return m_iNumCellsHeight; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Returns \c true if the tile contains the specified geolocation
    bool contains(const CGeoloc& gPosition) const;

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QMutex      m_tMutex;
    QFile*      m_pFile;
    QString     m_sFileName;
    CGeoloc     m_gGeoloc;
    CGeoloc     m_gSize;
    int         m_iNumCellsWidth;
    int         m_iNumCellsHeight;
};
