
#pragma once

// Qt
#include <QMap>

// qt-plus
#include "CXMLNode.h"

// Application
#include "CVector3.h"
#include "CNamed.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CVertexGroup : public CNamed
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CVertexGroup();

    //!
    virtual ~CVertexGroup();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    const QMap<int, double>& weights() const { return m_vWeights; }

    //!
    QMap<int, double>& weights() { return m_vWeights; }

    //!
    double vertexWeight(int iIndex) const;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void clear() { m_vWeights.clear(); }

protected:

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QMap<int, double>		m_vWeights;
};
