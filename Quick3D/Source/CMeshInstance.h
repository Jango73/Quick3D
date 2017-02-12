
#pragma once

#include "quick3d_global.h"

// Qt
#include <QSharedPointer>

// Application
#include "CQ3DConstants.h"
#include "CComponent.h"
#include "CMesh.h"

//-------------------------------------------------------------------------------------------------
// Déclarations avancées

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CMeshInstance : public CComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructeur avec un mesh cible
    CMeshInstance(QSP<CMesh> pMesh);

    //! Constructeur avec une liste de meshs cibles
    CMeshInstance(const QVector<QSP<CMesh> >& vMeshes);

    //! Destructor
    virtual ~CMeshInstance();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void addMesh(QSP<CMesh> pMesh);

    //!
    CMeshInstance* clone();

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CMeshInstance; }

    //!
    virtual CBoundingBox bounds();

    //!
    virtual CBoundingBox worldBounds();

    //! Renders the mesh
    virtual void paint(CRenderContext* pContext) Q_DECL_OVERRIDE;

protected:

    QVector<QSP<CMesh> >    m_vMeshes;
};
