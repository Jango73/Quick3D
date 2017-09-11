
#pragma once

// Qt
#include <QObject>
#include <QImage>
#include <QMutex>
#include <QMutexLocker>

// Application
#include "quick3d_global.h"
#include "CWorker.h"
#include "CComponent.h"
#include "CBoundedMeshInstances.h"
#include "CMaterial.h"
#include "CHeightField.h"
#include "CTerrain.h"
#include "CBox.h"
#include "CGeometryGenerator.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;
class CWorldTerrain;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CWorldChunk : public CWorker, public CComponent, public CHeightField
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CWorldChunk(C3DScene* pScene, CWorldTerrain* pAutoTerrain, CHeightField* pContainer);

    //!
    virtual ~CWorldChunk();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    virtual void setGeoloc(CGeoloc value);

    //!
    virtual void setSize(CGeoloc value);

    //!
    void setTerrain(QSP<CTerrain> value, bool bGenerateNow = false);

    //!
    void setWater(QSP<CTerrain> value);

    //!
    void setDistance(double value) { m_dDistance = value; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    CGeoloc originalGeoloc() const { return m_gOriginalGeoloc; }

    //!
    CGeoloc originalSize() const { return m_gOriginalSize; }

    //!
    CGeoloc size() const { return m_gSize; }

    //!
    QSP<CTerrain> terrain() const { return m_pTerrain; }

    //!
    QSP<CTerrain> water() const { return m_pWater; }

    //!
    QVector<CBoundedMeshInstances*>& meshes() { return m_vBoundedMeshes; }

    //!
    QMap<QString, QSP<CMeshGeometry> >& bushMeshes() { return m_vBushMeshes; }

    //!
    bool isOK() const { return m_bOK; }

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual CBoundingBox bounds() Q_DECL_OVERRIDE;

    //!
    virtual CBoundingBox worldBounds() Q_DECL_OVERRIDE;

    //!
    virtual CBoundingBox buildWorldBounds();

    //! Deletes this object's links
    virtual void clearLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //!
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //!
    virtual double getHeightAt(const CGeoloc& gPosition, double* pRigidness = nullptr) Q_DECL_OVERRIDE;

    //!
    virtual void flatten(const CGeoloc& gPosition, double dRadius);

    //! Ray intersection
    virtual Math::RayTracingResult intersect(Math::CRay3 ray) Q_DECL_OVERRIDE;

    //!
    virtual bool isReallyExpendable() const Q_DECL_OVERRIDE;

    //! Dumps contents to a stream
    virtual void dump(QTextStream& stream, int iIdent) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    bool operator < (const CWorldChunk& other) const;

    //!
    void paint(CRenderContext* pContext);

    //!
    bool drawable();

    //!
    bool isEmpty();

    //!
    CBoundingBox createBoundsForTerrain(CGeoloc gPosition, CGeoloc gSize, double dAltitudeFactor = 1.0);

    //!
    void build();

    //!
    void clearTerrain();

    //!
    virtual void work();

    //!
    bool checkPositionFree(CGeoloc gPosition, double dRadius);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QMutex                              m_mMutex;
    CWorldTerrain*                      m_pAutoTerrain;
    CHeightField*                       m_pContainer;
    CGeoloc                             m_gOriginalGeoloc;
    CGeoloc                             m_gOriginalSize;
    CGeoloc                             m_gSize;
    CBoundingBox                        m_bWorldBounds;
    QVector<CBoundedMeshInstances*>     m_vBoundedMeshes;
    double                              m_dDistance;
    bool                                m_bOK;

    // Shared data

    QSP<CTerrain>                       m_pTerrain;
    QSP<CTerrain>                       m_pWater;
    QMap<QString, QSP<CMeshGeometry> >  m_vBushMeshes;
};
