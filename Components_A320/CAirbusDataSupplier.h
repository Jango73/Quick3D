
#pragma once

// Quick3D
#include "CXMLNode.h"
#include "CComponent.h"
#include "ILoadable.h"

// Application
#include "components_a320_global.h"
#include "Constants.h"
#include "CAirbusData.h"

//-------------------------------------------------------------------------------------------------

class COMPONENTS_A320_EXPORT CAirbusDataSupplier : public ILoadable
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CAirbusDataSupplier();

    //!
    virtual ~CAirbusDataSupplier();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    CAirbusData* getData(EAirbusData eID);

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Loads this object's parameters
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) Q_DECL_OVERRIDE;

    //! Solves the links of this object
    virtual void solveLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //! Deletes this object's links
    virtual void clearLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //! Solves the links of this object
    virtual void solveLinks(C3DScene* pScene, CComponent* pCaller);

    //!
    void pushData(CAirbusData data);

    //!
    void sendData();

    //!
    void sendDataToConsumer(CAirbusDataSupplier* pConsumer);

    //!
    void receiveData(CAirbusData data);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QVector<QString>                m_vDataInputNames;
    QVector<CAirbusDataSupplier*>   m_vDataOutputs;
    QVector<CAirbusDataSupplier*>   m_vDataInputs;
    QVector<CAirbusData>            m_vData;
};
