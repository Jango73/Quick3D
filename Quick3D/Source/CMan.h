
#pragma once

// qt-plus
#include "CLogger.h"

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CTrajectorable.h"

class C3DScene;

class QUICK3D_EXPORT CMan : public CTrajectorable
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instanciator(C3DScene* pScene);

    //!
    CMan(C3DScene* pScene);

    //!
    virtual ~CMan();

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CMan; }

    //!
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------
};
