
#pragma once

// Qt
#include <QSharedPointer>

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CComponent.h"
#include "CMesh.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

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

    //! Dumps contents to a stream
    virtual void dump(QTextStream& stream, int iIdent) Q_DECL_OVERRIDE;

protected:

    QVector<QSP<CMesh> >    m_vMeshes;
};
