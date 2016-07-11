
#pragma once

#include "quick3d_global.h"

// Application
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
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    CVegetation()
        : m_eType(evtNone)
        , m_dSpread(0.0)
        , m_pFunction(NULL)
        , m_pMesh(NULL)
        , m_pMaterial(NULL)
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
        if (m_pFunction != NULL) delete m_pFunction;
        if (m_pMesh != NULL) delete m_pMesh;
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
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instanciator(C3DScene* pScene);

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
    // Méthodes héritées
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CVegetationGenerator; }

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    //! Charge les paramètres de cet objet
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent, CXMLNode xFunctions) Q_DECL_OVERRIDE;

    //!
    virtual void generate(QSP<CWorldChunk> pChunk) Q_DECL_OVERRIDE;

    //!
    void placeTree(QSP<CWorldChunk> pChunk, CGeoloc gPosition, double dRadius, int iVegetIndex);

    //!
    void placeBush(QSP<CWorldChunk> pChunk, CGeoloc gPosition, double dRadius, int iVegetIndex);

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    QVector<QSP<CVegetation> >      m_vVegetation;
};
