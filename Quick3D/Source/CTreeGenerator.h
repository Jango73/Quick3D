
#pragma once

// Application
#include "quick3d_global.h"
#include "CVector3.h"
#include "CMeshGeometry.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CTreeGenerator
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CTreeGenerator(C3DScene* pScene);

    //!
    ~CTreeGenerator();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    CMeshGeometry* createTree(
            int iLODLevel,
            Math::CVector3 vNoisePosition,
            int iNumLevels,
            double dTrunkLength,
            double dTrunkRadius,
            double dLengthScale,
            double dRadiusScale,
            double dLeafScale,
            double dGravityFactor,
            const QVector<Math::CVector3>& vFFDFrom,
            const QVector<Math::CVector3>& vFFDTo
            );

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    void addBranches(
            CMeshGeometry* pMesh,
            int iLODLevel,
            Math::CVector3 vNoisePosition,
            int iNumLevels,
            Math::CMatrix4 mAccumTransform,
            double dTrunkLength,
            double dTrunkRadius,
            double dLengthScale,
            double dRadiusScale,
            double dLeafScale,
            double dGravityFactor,
            const QVector<Math::CVector3>& vFFDFrom,
            const QVector<Math::CVector3>& vFFDTo,
            double dCurrentLengthScale,
            double dCurrentRadiusScale,
            int& iTreeLevel
            );

    //!
    void addLeaves(
            CMeshGeometry* pMesh,
            int iLODLevel,
            Math::CMatrix4 mAccumTransform,
            double dTrunkLength,
            double dTrunkRadius,
            double dLeafScale,
            const QVector<Math::CVector3>& vFFDFrom,
            const QVector<Math::CVector3>& vFFDTo,
            double dCurrentLengthScale,
            double dCurrentRadiusScale
            );

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QMutex      m_mMutex;
    C3DScene*   m_pScene;
};
