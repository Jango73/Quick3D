
#pragma once

// Qt
#include <QImage>
#include <QDateTime>

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CGeoloc.h"
#include "CBoundingBox.h"

//-------------------------------------------------------------------------------------------------

template<class T>
class CGeoTreeItem
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CGeoTreeItem(CGeoloc gGeoloc, CGeoloc gSize, int iMaxLevel, int iLevel)
        : m_iLevel(iLevel)
        , m_gGeoloc(gGeoloc)
        , m_gSize(gSize)
    {
        if (iLevel < iMaxLevel)
        {
            CGeoloc gStart(
                        m_gGeoloc.Latitude - m_gSize.Latitude * 0.5,
                        m_gGeoloc.Longitude - m_gSize.Longitude * 0.5,
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

            m_vChildren << new CGeoTreeItem<T>(g1, gSize, iMaxLevel, iLevel + 1);
            m_vChildren << new CGeoTreeItem<T>(g2, gSize, iMaxLevel, iLevel + 1);
            m_vChildren << new CGeoTreeItem<T>(g3, gSize, iMaxLevel, iLevel + 1);
            m_vChildren << new CGeoTreeItem<T>(g4, gSize, iMaxLevel, iLevel + 1);
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
    CGeoloc geoloc() const { return m_gGeoloc; }

    //!
    CGeoloc size() const { return m_gSize; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void append(const CGeoloc& gGeoloc, T data)
    {
        if (m_vChildren.count() == 0)
        {
            m_vData << data;
        }
        else
        {
            foreach (CGeoTreeItem<T>* pChild, m_vChildren)
            {
                CGeoloc gMin(
                            pChild->geoloc().Latitude - pChild->size().Latitude * 0.5,
                            pChild->geoloc().Longitude - pChild->size().Longitude * 0.5
                            );

                CGeoloc gMax(
                            pChild->geoloc().Latitude + pChild->size().Latitude * 0.5,
                            pChild->geoloc().Longitude + pChild->size().Longitude * 0.5
                            );

                if (
                        gGeoloc.Latitude >= gMin.Latitude &&
                        gGeoloc.Latitude <= gMax.Latitude &&
                        gGeoloc.Longitude >= gMin.Longitude &&
                        gGeoloc.Longitude <= gMax.Longitude
                        )
                {
                    pChild->append(gGeoloc, data);
                    return;
                }
            }
        }
    }

    //!
    void query(QVector<T>& vResults, const CGeoloc& gPosition, double dRadius_m)
    {
        if (m_vChildren.count() == 0)
        {
            for (auto data : m_vData)
            {
                vResults << data;
            }
        }
        else
        {
            foreach (CGeoTreeItem<T>* pChild, m_vChildren)
            {
                CGeoloc gMin(
                            pChild->geoloc().Latitude - pChild->size().Latitude * 0.5,
                            pChild->geoloc().Longitude - pChild->size().Longitude * 0.5
                            );

                CGeoloc gMax(
                            pChild->geoloc().Latitude + pChild->size().Latitude * 0.5,
                            pChild->geoloc().Longitude + pChild->size().Longitude * 0.5
                            );

                if (
                        gPosition.Latitude >= gMin.Latitude &&
                        gPosition.Latitude <= gMax.Latitude &&
                        gPosition.Longitude >= gMin.Longitude &&
                        gPosition.Longitude <= gMax.Longitude
                        )
                {
                    pChild->query(vResults, gPosition, dRadius_m);
                }
            }
        }
    }

protected:

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

    int                         m_iLevel;
    CGeoloc                     m_gGeoloc;
    CGeoloc                     m_gSize;
    QVector<T>                  m_vData;
    QVector<CGeoTreeItem<T>*>   m_vChildren;
};

//-------------------------------------------------------------------------------------------------

template<class T>
class CGeoTree
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
        if (m_pRoot != nullptr)
        {
            delete m_pRoot;
        }
    }

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    int levels() const { return m_iLevels; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void append(const CGeoloc& gGeoloc, T data)
    {
    }

    //!
    QVector<T> query(const CGeoloc& gGeoloc, double dRadius_m)
    {
        QVector<T> vResults;

        if (m_pRoot != nullptr)
        {
            m_pRoot->queryRecurse(vResults, gGeoloc, dRadius_m);
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

            m_pRoot = new CGeoTreeItem<T>(gGeoloc, gSize, m_iLevels, 0);
        }
    }

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    int                 m_iLevels;
    CGeoTreeItem<T>*    m_pRoot;
};
