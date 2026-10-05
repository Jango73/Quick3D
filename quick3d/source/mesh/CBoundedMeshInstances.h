
#pragma once

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CComponent.h"
#include "CMeshInstance.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;

class QUICK3D_EXPORT CBoundedMeshInstances : public CComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CBoundedMeshInstances(C3DScene* pScene);

    //!
    virtual ~CBoundedMeshInstances();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setBounds(CBoundingBox bBox);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    const QVector<CMeshInstance*>& meshes() { return m_vMeshes; }

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CBoundedMeshInstances; }

    //!
    virtual CBoundingBox bounds();

    //!
    virtual CBoundingBox worldBounds();

    //! Updates this object using the elapsed time since last update
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //! Renders the mesh
    virtual void paint(CRenderContext* pContext) Q_DECL_OVERRIDE;

    //! Dumps contents to a stream
    virtual void dump(QTextStream& stream, int iIdent) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Ajoute un mesh dans ce délimiteur
    void add(CMeshInstance* pMeshInstance);

protected:

    CBoundingBox                m_bBounds;
    QVector<CMeshInstance*>     m_vMeshes;
};
