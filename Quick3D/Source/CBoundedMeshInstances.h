
#pragma once

#include "quick3d_global.h"

// Application
#include "CQ3DConstants.h"
#include "CComponent.h"
#include "CMeshInstance.h"

class C3DScene;

class QUICK3D_EXPORT CBoundedMeshInstances : public CComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //!
    CBoundedMeshInstances(C3DScene* pScene);

    //!
    ~CBoundedMeshInstances();

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
    // M�thodes h�rit�es
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CBoundedMeshInstances; }

    //!
    virtual CBoundingBox bounds();

    //!
    virtual CBoundingBox worldBounds();

    //!
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //! Renders the mesh
    virtual void paint(CRenderContext* pContext) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // M�thodes de contr�le
    //-------------------------------------------------------------------------------------------------

    //! Ajoute un mesh dans ce d�limiteur
    void add(CMeshInstance* pMeshInstance);

protected:

    CBoundingBox                m_bBounds;
    QVector<CMeshInstance*>     m_vMeshes;
};
