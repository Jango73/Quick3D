
#pragma once

// Qt
#include <QImage>
#include <QDateTime>

// qt-plus
#include "CLogger.h"

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CGeolocalized.h"
#include "CDumpable.h"

//-------------------------------------------------------------------------------------------------

class CGeoTreeItem : public CGeolocalized, public CDumpable
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CGeoTreeItem(CGeoloc gGeoloc, CGeoloc gSize, int iMaxLevel, int iLevel)
        : CGeolocalized(gGeoloc)
        , m_iLevel(iLevel)
        , m_gSize(gSize)
    {
        if (iLevel < iMaxLevel)
        {
            CGeoloc gStart(
                        geoloc().Latitude - m_gSize.Latitude * 0.5,
                        geoloc().Longitude - m_gSize.Longitude * 0.5,
                        0.0
                        );

            CGeoloc gSize(
                        m_gSize.Latitude * 0.5,
                        m_gSize.Longitude * 0.5,
                        0.0
                        );

            CGeoloc g1(gStart.Latitude + m_gSize.Latitude * 0.25, gStart.Longitude + m_gSize.Longitude * 0.25, 0.0);
            CGeoloc g2(gStart.Latitude + m_gSize.Latitude * 0.25, gStart.Longitude + m_gSize.Longitude * 0.75, 0.0);
            CGeoloc g3(gStart.Latitude + m_gSize.Latitude * 0.75, gStart.Longitude + m_gSize.Longitude * 0.25, 0.0);
            CGeoloc g4(gStart.Latitude + m_gSize.Latitude * 0.75, gStart.Longitude + m_gSize.Longitude * 0.75, 0.0);

            m_vChildren << new CGeoTreeItem(g1, gSize, iMaxLevel, iLevel + 1);
            m_vChildren << new CGeoTreeItem(g2, gSize, iMaxLevel, iLevel + 1);
            m_vChildren << new CGeoTreeItem(g3, gSize, iMaxLevel, iLevel + 1);
            m_vChildren << new CGeoTreeItem(g4, gSize, iMaxLevel, iLevel + 1);
        }
    }

    //!
    virtual ~CGeoTreeItem()
    {
        DELETE_VECTOR_ITEMS(m_vChildren);
    }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    CGeoloc size() const { return m_gSize; }

    //-------------------------------------------------------------------------------------------------
    // Overridden methods
    //-------------------------------------------------------------------------------------------------

    //! Dumps contents of this object to \a stream using indent value in \a iIdent
    virtual void dump(QTextStream& stream, int iIdent) Q_DECL_OVERRIDE
    {
        dumpIdent(stream, iIdent, QString("[CGeoTreeItem]"));
        dumpIdent(stream, iIdent, QString("Level : %1").arg(m_iLevel));
        dumpIdent(stream, iIdent, QString("Geoloc : %1").arg(m_gGeoloc.toString()));
        dumpIdent(stream, iIdent, QString("Size : %1").arg(m_gSize.toString()));
        dumpIdent(stream, iIdent, QString("Data count : %1").arg(m_vData.count()));
        dumpIdent(stream, iIdent, QString("Children :"));

        dumpOpenBlock(stream, iIdent);
        for (auto pChild : m_vChildren)
        {
            pChild->dump(stream, iIdent + 1);
        }
        dumpCloseBlock(stream, iIdent);
    }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void append(CGeolocalized* pGeolocalized)
    {
        if (m_vChildren.count() == 0)
        {
            m_vData << pGeolocalized;
        }
        else
        {
            foreach (CGeoTreeItem* pChild, m_vChildren)
            {
                CGeoloc gMin(
                            pChild->geoloc().Latitude - pChild->size().Latitude * 0.5,
                            pChild->geoloc().Longitude - pChild->size().Longitude * 0.5,
                            0.0
                            );

                CGeoloc gMax(
                            pChild->geoloc().Latitude + pChild->size().Latitude * 0.5,
                            pChild->geoloc().Longitude + pChild->size().Longitude * 0.5,
                            0.0
                            );

                if (
                        pGeolocalized->geoloc().Latitude >= gMin.Latitude &&
                        pGeolocalized->geoloc().Latitude <= gMax.Latitude &&
                        pGeolocalized->geoloc().Longitude >= gMin.Longitude &&
                        pGeolocalized->geoloc().Longitude <= gMax.Longitude
                        )
                {
                    pChild->append(pGeolocalized);
                    return;
                }
            }
        }
    }

    //!
    void query(QVector<CGeolocalized*>& vResults, const CGeoloc& gPosition, double dRange_deg)
    {
        if (m_vChildren.count() == 0)
        {
            for (auto data : m_vData)
            {
                double dLatDiff = fabs(Math::Angles::angleDifferenceDegree(gPosition.Latitude, data->geoloc().Latitude));
                double dLonDiff = fabs(Math::Angles::angleDifferenceDegree(gPosition.Longitude, data->geoloc().Longitude));

                if (dLatDiff < dRange_deg && dLonDiff < dRange_deg)
                {
                    vResults << data;
                }
            }
        }
        else
        {
            foreach (CGeoTreeItem* pChild, m_vChildren)
            {
                CGeoloc gMin(
                            pChild->geoloc().Latitude - pChild->size().Latitude * 0.5,
                            pChild->geoloc().Longitude - pChild->size().Longitude * 0.5,
                            0.0
                            );

                CGeoloc gMax(
                            pChild->geoloc().Latitude + pChild->size().Latitude * 0.5,
                            pChild->geoloc().Longitude + pChild->size().Longitude * 0.5,
                            0.0
                            );

                bool bContains = true;

                if (gPosition.Latitude + dRange_deg < gMin.Latitude) bContains = false;
                if (gPosition.Latitude - dRange_deg > gMax.Latitude) bContains = false;
                if (gPosition.Longitude + dRange_deg < gMin.Longitude) bContains = false;
                if (gPosition.Longitude - dRange_deg > gMax.Longitude) bContains = false;

                if (bContains)
                {
                    pChild->query(vResults, gPosition, dRange_deg);
                }
            }
        }
    }

protected:

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

    int                         m_iLevel;
    CGeoloc                     m_gSize;
    QVector<CGeolocalized*>     m_vData;
    QVector<CGeoTreeItem*>      m_vChildren;
};

//-------------------------------------------------------------------------------------------------

class CGeoTree : public CDumpable
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CGeoTree(int iLevels = 5)
        : m_iLevels(iLevels)
        , m_pRoot(nullptr)
    {
        build();
    }

    //!
    virtual ~CGeoTree()
    {
        clear();
    }

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    int levels() const { return m_iLevels; }

    //!
    CGeoTreeItem* root() { return m_pRoot; }

    //!
    const CGeoTreeItem* root() const { return m_pRoot; }

    //-------------------------------------------------------------------------------------------------
    // Overridden methods
    //-------------------------------------------------------------------------------------------------

    //! Dumps contents of this object to \a stream using indentation value in \a iIdent
    virtual void dump(QTextStream& stream, int iIdent) Q_DECL_OVERRIDE
    {
        dumpIdent(stream, iIdent, QString("[CGeoTree]"));
        dumpIdent(stream, iIdent, QString("Levels : %1").arg(m_iLevels));
        dumpIdent(stream, iIdent, QString("Root :"));

        if (m_pRoot != nullptr)
        {
            m_pRoot->dump(stream, iIdent + 1);
        }
    }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void clear()
    {
        if (m_pRoot != nullptr)
        {
            delete m_pRoot;
            m_pRoot = nullptr;
        }
    }

    //!
    void append(CGeolocalized* pGeolocalized)
    {
        if (m_pRoot != nullptr)
        {
            m_pRoot->append(pGeolocalized);
        }
    }

    //!
    QVector<CGeolocalized*> query(const CGeoloc& gGeoloc, double dRadius_m)
    {
        QVector<CGeolocalized*> vResults;

        CGeoloc g2(gGeoloc, Math::CVector3(dRadius_m, 0.0, 0.0));

        if (m_pRoot != nullptr)
        {
            m_pRoot->query(vResults, gGeoloc, fabs(Math::Angles::angleDifferenceDegree(g2.Longitude, gGeoloc.Longitude)));
        }

        return vResults;
    }

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    void build()
    {
        if (m_pRoot == nullptr)
        {
            double startLat = 0.0;
            double startLon = 180.0;

            CGeoloc gGeoloc(startLat, startLon, 0.0);
            CGeoloc gSize(CGeoloc(180.0, 360.0, 0.0));

            m_pRoot = new CGeoTreeItem(gGeoloc, gSize, m_iLevels, 0);
        }
    }

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    int             m_iLevels;
    CGeoTreeItem*   m_pRoot;
};
