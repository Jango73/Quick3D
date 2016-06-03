
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Application
#include "CVector3.h"
#include "CMeshInstance.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CTreeGenerator
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
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
    // M�thodes de contr�le
    //-------------------------------------------------------------------------------------------------

    //!
    CMesh* createTree(
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
    // M�thodes prot�g�es
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    void addBranches(
            CMesh* pMesh,
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
            CMesh* pMesh,
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
    // Propri�t�s
    //-------------------------------------------------------------------------------------------------

protected:

    QMutex						m_mMutex;
    C3DScene*					m_pScene;
};
