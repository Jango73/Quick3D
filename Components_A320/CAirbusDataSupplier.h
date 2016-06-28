
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
    // Constructeurs et destructeur
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
    // Méthodes héritées
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    //! Charge les paramètres de cet objet
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) Q_DECL_OVERRIDE;

    //! Recherche les liens de cet objet
    virtual void solveLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //! Efface les liens de cet objet
    virtual void clearLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //! Recherche les liens de cet objet
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
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    QVector<QString>                m_vDataInputNames;
    QVector<CAirbusDataSupplier*>   m_vDataOutputs;
    QVector<CAirbusDataSupplier*>   m_vDataInputs;
    QVector<CAirbusData>            m_vData;
};
