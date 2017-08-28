
#pragma once

// Qt
#include <QImage>
#include <QMutex>
#include <QMutexLocker>
#include <QVector>

// Application
#include "quick3d_global.h"
#include "CVector3.h"
#include "CBoundingBox.h"
#include "CComponent.h"

//-------------------------------------------------------------------------------------------------

#define DEFAULT_MAX_PARTITION_LEVEL     1

//-------------------------------------------------------------------------------------------------

template<class T>
class CBoundPartition
{
public:

    CBoundPartition()
    {
    }

    CBoundPartition(CBoundingBox bBounds)
        : m_bBounds(bBounds)
    {
    }

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns the bounds of this partition
    CBoundingBox& bounds() { return m_bBounds; }

    //! Returns the bounds of this partition
    const CBoundingBox& bounds() const { return m_bBounds; }

    //! Returns the child partitions
    QVector<CBoundPartition<T> >& children() { return m_vChildren; }

    //!
    QVector<T>& data() { return m_vData; }

    //!
    const QVector<T>& data() const { return m_vData; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    CBoundPartition& operator = (const CBoundPartition& target)
    {
        m_bBounds = target.m_bBounds;
        m_vChildren = target.m_vChildren;
        m_vData = target.m_vData;

        return *this;
    }

    //!
    void clear()
    {
        m_vChildren.clear();
        m_vData.clear();
    }

    //!
    void append(const T& data)
    {
        m_vData.append(data);
    }

    //!
    void addChild(CBoundPartition<T> mpChild)
    {
        m_vChildren.append(mpChild);
    }

protected:

    CBoundingBox                    m_bBounds;
    QVector<CBoundPartition<T> >    m_vChildren;
    QVector<T>                      m_vData;
};

//-------------------------------------------------------------------------------------------------

template<class T>
class CBoundPartitioned
{
public:

    CBoundPartitioned()
        : m_iMaxLevel(DEFAULT_MAX_PARTITION_LEVEL)
    {
    }

    CBoundPartitioned(CBoundingBox bBounds, int iMaxLevel = DEFAULT_MAX_PARTITION_LEVEL)
        : m_iMaxLevel(iMaxLevel)
        , m_bBounds(bBounds)
    {
    }

    //!
    void createPartitions(const CBoundingBox& bBounds)
    {
        m_tPartition.clear();
        m_bBounds = bBounds;
        createPartitionsInternal(m_tPartition, 0);
    }

    Math::RayTracingResult intersectPartitions(CComponent* pContainer, Math::CRay3 ray)
    {
        return intersectPartitionsInternal(pContainer, m_tPartition, ray);
    }

    //!
    virtual void addDataForPartition(CBoundPartition<T>& partition)
    {
        Q_UNUSED(partition);
    }

    //!
    virtual Math::RayTracingResult intersectPartitionData(CComponent* pContainer, const CBoundPartition<T>& partition, Math::CRay3 ray)
    {
        Q_UNUSED(pContainer);
        Q_UNUSED(ray);
    }

protected:

    //!
    void createPartitionsInternal(CBoundPartition<T>& partition, int iLevel)
    {
        if (iLevel < m_iMaxLevel)
        {
            // Divide the cube

            // Define the child partition bounds
            double dSizeX = partition.bounds().maximum().X - partition.bounds().minimum().X;
            double dSizeY = partition.bounds().maximum().Y - partition.bounds().minimum().Y;
            double dSizeZ = partition.bounds().maximum().Z - partition.bounds().minimum().Z;

            double dNormalizedSizeX = dSizeX / 2.0;
            double dNormalizedSizeY = dSizeY / 2.0;
            double dNormalizedSizeZ = dSizeZ / 2.0;

            // Iterate through all partitions
            for (int iZ = 0; iZ < 2; iZ++)
            {
                for (int iY = 0; iY < 2; iY++)
                {
                    for (int iX = 0; iX < 2; iX++)
                    {
                        // Définition de la boite englobante de la partition enfant
                        Math::CVector3 vMinimum;
                        Math::CVector3 vMaximum;

                        vMinimum.X = partition.bounds().minimum().X + ((double) (iX + 0) * dNormalizedSizeX);
                        vMinimum.Y = partition.bounds().minimum().Y + ((double) (iY + 0) * dNormalizedSizeY);
                        vMinimum.Z = partition.bounds().minimum().Z + ((double) (iZ + 0) * dNormalizedSizeZ);

                        vMaximum.X = partition.bounds().minimum().X + ((double) (iX + 1) * dNormalizedSizeX);
                        vMaximum.Y = partition.bounds().minimum().Y + ((double) (iY + 1) * dNormalizedSizeY);
                        vMaximum.Z = partition.bounds().minimum().Z + ((double) (iZ + 1) * dNormalizedSizeZ);

                        // Ajout de la partition enfant
                        CBoundPartition<T> bpChild = CBoundPartition<T>(CBoundingBox(vMinimum, vMaximum));

                        createPartitionsInternal(bpChild, iLevel + 1);

                        partition.addChild(bpChild);
                    }
                }
            }
        }
        else
        {
            addDataForPartition(partition);
        }
    }

    //! Ray intersection
    Math::RayTracingResult intersectPartitionsInternal(CComponent* pContainer, CBoundPartition<T>& tPartition, Math::CRay3 ray)
    {
        Math::RayTracingResult dReturnResult(Q3D_INFINITY, pContainer);
        double dDistanceToBox = tPartition.bounds().intersect(ray).m_dDistance;

        if (dDistanceToBox < Q3D_INFINITY)
        {
            if (tPartition.children().count() > 0)
            {
                foreach (CBoundPartition<T> mpChild, tPartition.children())
                {
                    Math::RayTracingResult dNewResult = intersectPartitionsInternal(pContainer, mpChild, ray);

                    if (dNewResult.m_dDistance < dReturnResult.m_dDistance)
                    {
                        dReturnResult.m_dDistance = dNewResult.m_dDistance;
                        dReturnResult.m_vNormal = dNewResult.m_vNormal;
                        dReturnResult.m_pObject = dNewResult.m_pObject;
                    }
                }
            }
            else
            {
                Math::RayTracingResult dNewResult = intersectPartitionData(pContainer, tPartition, ray);

                if (dNewResult.m_dDistance < dReturnResult.m_dDistance)
                {
                    dReturnResult.m_dDistance = dNewResult.m_dDistance;
                    dReturnResult.m_vNormal = dNewResult.m_vNormal;
                    dReturnResult.m_pObject = dNewResult.m_pObject;
                }
            }
        }

        return dReturnResult;
    }

protected:

    int                         m_iMaxLevel;
    CBoundingBox                m_bBounds;
    CBoundPartition<T>          m_tPartition;
};
