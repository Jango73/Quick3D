
// qt-plus
#include "CLogger.h"

// Application
#include "CGeoTree.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

#define LAT_MAX  90.0

//-------------------------------------------------------------------------------------------------

template<class T>
CGeoTreeItem<T>::CGeoTreeItem(CGeoloc gGeoloc, CGeoloc gSize, int iMaxLevel, int iLevel)
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

//-------------------------------------------------------------------------------------------------

template<class T>
CGeoTreeItem<T>::~CGeoTreeItem()
{
    DELETE_VECTOR_ITEMS(m_vChildren);
}

//-------------------------------------------------------------------------------------------------

template<class T>
void CGeoTreeItem<T>::append(const CGeoloc& gGeoloc, T data)
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

//-------------------------------------------------------------------------------------------------

template<class T>
void CGeoTreeItem<T>::query(QVector<T>& vResults, const CGeoloc& gPosition, double dRadius_m)
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

//-------------------------------------------------------------------------------------------------

/*!
    \class CGeoTree
    \brief A geodetic tree.
    \inmodule Quick3D
*/

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CGeoTree \br\br.
    \a iLevels
*/
template<class T>
CGeoTree<T>::CGeoTree(int iLevels)
    : m_iLevels(iLevels)
    , m_pRoot(nullptr)
{
    build();
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys a CGeoTree.
*/
template<class T>
CGeoTree<T>::~CGeoTree()
{
    if (m_pRoot != nullptr)
    {
        delete m_pRoot;
    }
}

//-------------------------------------------------------------------------------------------------

template<class T>
void CGeoTree<T>::append(const CGeoloc& gGeoloc, T data)
{
}

//-------------------------------------------------------------------------------------------------

template<class T>
QVector<T> CGeoTree<T>::query(const CGeoloc& gGeoloc, double dRadius_m)
{
    QVector<T> vResults;

    if (m_pRoot != nullptr)
    {
        m_pRoot->queryRecurse(vResults, gGeoloc, dRadius_m);
    }

    return vResults;
}

//-------------------------------------------------------------------------------------------------

/*!
    Builds the geodetic tree.
*/
template<class T>
void CGeoTree<T>::build()
{
    if (m_pRoot == nullptr)
    {
        double startLat = 0.0;
        double startLon = 180.0;

        CGeoloc gGeoloc(startLat, startLon, 0.0);
        CGeoloc gSize(CGeoloc(LAT_MAX * 2.0, 360.0, 0.0));

        m_pRoot = new CGeoTreeItem<T>(gGeoloc, gSize, m_iLevels, 0);
    }
}
