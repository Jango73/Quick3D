
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Application
#include "CVector3.h"
#include "CGeoloc.h"
#include "CXMLNode.h"
#include "CHeightField.h"
#include "CSRTMData.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CSRTMField : public CHeightField
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Constructeur avec param�tres
    CSRTMField(CXMLNode xParameters, const QString& sPathToSRTMFiles = "", double dValueForNoData = -2000.0);

    //! Destructeur
    virtual ~CSRTMField();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    virtual double getHeightAt(const CGeoloc& gPosition, double* pRigidness = NULL);

    //!
    virtual double getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, double* pRigidness = NULL);

    //!
    virtual double getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, bool bForPhysics = true);

    //-------------------------------------------------------------------------------------------------
    // M�thodes de contr�le
    //-------------------------------------------------------------------------------------------------

    //!
    virtual void flatten(const CGeoloc& gPosition, double dRadius);

    //!
    void addChunk(CSRTMData* pData);

    //-------------------------------------------------------------------------------------------------
    // M�thodes prot�g�es
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    void parseSRTMFiles();

    //-------------------------------------------------------------------------------------------------
    // Propri�t�s
    //-------------------------------------------------------------------------------------------------

protected:

    CXMLNode            m_xParameters;
    double              m_dValueForNoData;
    QString             m_sPath;
    QVector<CSRTMData*> m_vChunks;
};
