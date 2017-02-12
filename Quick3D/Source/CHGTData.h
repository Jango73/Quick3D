
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Qt
#include <QMutex>
#include <QFile>

// Fondations
#include "CVector3.h"
#include "CGeoloc.h"
#include "CXMLNode.h"

//-------------------------------------------------------------------------------------------------

/*

    Description du référentiel de la tuile
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

//! Classe de stockage d'une tuile HGT
class QUICK3D_EXPORT CHGTData
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CHGTData(const CGeoloc& gGeoloc, const CGeoloc& gSize);

    //!
    virtual ~CHGTData();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setFileName(QString sFileName);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    double getHeightAt(const CGeoloc& gPosition, double* pRigidness = NULL);

    //!
    int numCellsWidth() { return m_iNumCellsWidth; }

    //!
    int numCellsHeight() { return m_iNumCellsHeight; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
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
