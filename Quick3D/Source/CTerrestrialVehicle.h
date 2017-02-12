
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CLogManager.h"

// Application
#include "CQ3DConstants.h"
#include "CVehicle.h"

class C3DScene;

class QUICK3D_EXPORT CTerrestrialVehicle : public CVehicle
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instanciator(C3DScene* pScene);

    //!
    CTerrestrialVehicle(C3DScene* pScene);

    //!
    virtual ~CTerrestrialVehicle();

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CTerrestrialVehicle; }

    //!
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------
};
