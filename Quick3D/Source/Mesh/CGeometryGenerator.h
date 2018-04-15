
#pragma once

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CComponent.h"
#include "CGenerateFunction.h"
#include "CBoundedMeshInstances.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;
class CWorldChunk;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CGeometryGenerator : public CComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CGeometryGenerator(C3DScene* pScene);

    //!
    virtual ~CGeometryGenerator();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CGeometryGenerator; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Loads this object's parameters
    virtual void loadParameters(const QString& sBaseFile, const CXMLNode& xComponent, CXMLNode xFunctions);

    //! Generates the geometry for the given world chunk
    virtual void generate(QSP<CWorldChunk> pChunk) = 0;

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:
};
