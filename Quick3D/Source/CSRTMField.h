
#pragma once

//-------------------------------------------------------------------------------------------------

// Application
#include "quick3d_global.h"
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
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructeur avec paramètres
    CSRTMField(CXMLNode xParameters, const QString& sPathToSRTMFiles = "", double dValueForNoData = -2000.0);

    //! Destructor
    virtual ~CSRTMField();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    virtual double getHeightAt(const CGeoloc& gPosition, double* pRigidness = nullptr);

    //!
    virtual double getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, double* pRigidness = nullptr);

    //!
    virtual double getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, bool bForPhysics = true);

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual void flatten(const CGeoloc& gPosition, double dRadius);

    //!
    void addChunk(CSRTMData* pData);

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    void parseSRTMFiles();

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    CXMLNode            m_xParameters;
    double              m_dValueForNoData;
    QString             m_sPath;
    QVector<CSRTMData*> m_vChunks;
};
