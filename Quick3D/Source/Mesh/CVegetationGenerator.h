
#pragma once

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CGeometryGenerator.h"
#include "CMeshInstance.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;
class CWorldChunk;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CVegetation : public QSharedData
{
public:

    enum EVegetationType
    {
        evtNone,
        evtTree,
        evtBush
    };

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    CVegetation()
        : m_eType(evtNone)
        , m_dSpread(0.0)
        , m_pFunction(nullptr)
        , m_pMesh(nullptr)
        , m_pMaterial(nullptr)
    {
    }

    CVegetation(EVegetationType eType, double dSpread, CGenerateFunction* pFunction, CMeshInstance* pMesh, CMaterial* pMaterial)
        : m_eType(eType)
        , m_dSpread(dSpread)
        , m_pFunction(pFunction)
        , m_pMesh(pMesh)
        , m_pMaterial(QSP<CMaterial>(pMaterial))
    {
    }

    virtual ~CVegetation()
    {
        if (m_pFunction != nullptr) delete m_pFunction;
        if (m_pMesh != nullptr) delete m_pMesh;
    }

    EVegetationType     m_eType;
    double              m_dSpread;
    CGenerateFunction*  m_pFunction;
    CMeshInstance*      m_pMesh;
    QSP<CMaterial>      m_pMaterial;
};

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CVegetationGenerator : public CGeometryGenerator
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instantiator(C3DScene* pScene);

    //!
    CVegetationGenerator(C3DScene* pScene);

    //!
    virtual ~CVegetationGenerator();

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
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CVegetationGenerator; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Loads this object's parameters
    virtual void loadParameters(const QString& sBaseFile, const CXMLNode& xComponent, CXMLNode xFunctions) Q_DECL_OVERRIDE;

    //!
    virtual void generate(QSP<CWorldChunk> pChunk) Q_DECL_OVERRIDE;

    //!
    void placeTree(QSP<CWorldChunk> pChunk, CGeoloc gPosition, double dRadius, int iVegetIndex);

    //!
    void placeBush(QSP<CWorldChunk> pChunk, CGeoloc gPosition, double dRadius, int iVegetIndex);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QVector<QSP<CVegetation> >      m_vVegetation;
};
