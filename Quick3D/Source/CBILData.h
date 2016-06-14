
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

/**
 * Classe de stockage d'une tuile SRTM.
 *
 * Description du r�f�rentiel de la tuile.
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

class QUICK3D_EXPORT CBILData : public CExpendable
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //!
    CBILData(double dValueForNoData = 0.0);

    //! Constructeur avec param�tres
    CBILData(CGeoloc gGeoloc, CGeoloc gSize, int iNumCellsWidth, int iNumCellsHeight, qint16 ui16NoDataValue, const qint16* vData, double dValueForNoData = -2000.0);

    //! Constructeur de copie
    CBILData(const CBILData& target);

    //!
    virtual ~CBILData();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setFileName(QString value);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Retourne le centre g�olocalis� de la tuile
    CGeoloc getGeoloc() const { return m_gGeoloc; }

    //! Retourne la taille g�olocalis�e de la tuile
    CGeoloc getSize() const { return m_gSize; }

    //! Retourne l'altitude � la g�olocalisation sp�cifi�e
    double getHeightAt(const CGeoloc& gPosition, double* pRigidness = NULL);

    //! Retourne le nombre de cellules en largeur
    int getNumCellsWidth() const { return m_iNumCellsWidth; }

    //! Retourne le nombre de cellules en hauteur
    int getNumCellsHeight() const { return m_iNumCellsHeight; }

    //! Retourne les donn�es d'�l�vation constantes
    const qint16* getData() const { return m_vData; }

    //! Retourne les donn�es d'�l�vation
    qint16* getData() { return m_vData; }

    //-------------------------------------------------------------------------------------------------
    // M�thodes de contr�le
    //-------------------------------------------------------------------------------------------------

    //! Retourne vrai si la tuile contient le point g�olocalis� sp�cifi�
    bool contains(const CGeoloc& gPosition) const;

    //! Op�rateur de copie
    CBILData& operator = (const CBILData& target);

    //!
    void clearData();

    //-------------------------------------------------------------------------------------------------
    // M�thodes prot�g�es
    //-------------------------------------------------------------------------------------------------

protected:

    //! Lit l'ent�te du fichier m_sFileName
    QStringList readHeader();

    //! Lit les donn�es du fichier m_sFileName
    void readData();

    //-------------------------------------------------------------------------------------------------
    // Propri�t�s
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
