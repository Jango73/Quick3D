
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Fondations
#include "CVector3.h"
#include "CGeoloc.h"
#include "CXMLNode.h"

// Application
#include "CHeightField.h"
#include "CHGTData.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CHGTField : public CHeightField
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CHGTField(CXMLNode xParameters, const QString& sPathToHGTFiles = "");

    //!
    virtual ~CHGTField();

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

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    void parseFiles();

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    CXMLNode			m_xParameters;
    QString				m_sPath;
    QVector<CHGTData*>	m_vChunks;
};
