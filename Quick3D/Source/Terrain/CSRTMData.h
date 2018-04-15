
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

/**
 * Classe de stockage d'une tuile SRTM.
 *
 * Description du référentiel de la tuile.
 *
 *    m_gSize.Longitude
 *    <-------------->
 *    ----------------  ^
 *    |              |  |
 *    |              |  |
 *    |  m_gGeoloc   |  |
 *    |      .       |  | m_gSize.Latitude
 *    |              |  |
 *    |              |  |
 *    |              |  |
 *    ----------------  v
 */
class QUICK3D_EXPORT CSRTMData
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CSRTMData(double dValueForNoData = 0.0);

    //! Constructeur avec paramètres
    CSRTMData(CGeoloc gGeoloc, CGeoloc gSize, int iNumCellsWidth, int iNumCellsHeight, qint16 ui16NoDataValue, const qint16* vData, double dValueForNoData = -2000.0);

    //! Constructeur de copie
    CSRTMData(const CSRTMData& target);

    //!
    virtual ~CSRTMData();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setFileName(QString value);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Retourne le centre géolocalisé de la tuile
    CGeoloc getGeoloc() const { return m_gGeoloc; }

    //! Retourne la taille géolocalisée de la tuile
    CGeoloc getSize() const { return m_gSize; }

    //! Retourne l'altitude à la géolocalisation spécifiée
    double getHeightAt(const CGeoloc& gPosition, double* pRigidness = nullptr);

    //! Retourne le nombre de cellules en largeur
    int getNumCellsWidth() const { return m_iNumCellsWidth; }

    //! Retourne le nombre de cellules en hauteur
    int getNumCellsHeight() const { return m_iNumCellsHeight; }

    //! Retourne les données d'élévation constantes
    const qint16* getData() const { return m_vData; }

    //! Retourne les données d'élévation
    qint16* getData() { return m_vData; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Retourne vrai si la tuile contient le point géolocalisé spécifié
    bool contains(const CGeoloc& gPosition) const;

    //! Opérateur de copie
    CSRTMData& operator = (const CSRTMData& target);

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //! Lit l'entête du fichier m_sFileName
    QStringList readSRTMHeader();

    //! Lit les données du fichier m_sFileName
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
