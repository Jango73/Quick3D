
#pragma once

#include "quick3d_global.h"

// Qt
#include <QSharedPointer>

// Application
#include "CQ3DConstants.h"
#include "CComponent.h"
#include "CMesh.h"

//-------------------------------------------------------------------------------------------------
// D�clarations avanc�es

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CMeshInstance : public CComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Constructeur avec un mesh cible
    CMeshInstance(QSP<CMesh> pMesh);

    //! Constructeur avec une liste de meshs cibles
    CMeshInstance(const QVector<QSP<CMesh> >& vMeshes);

    //! Destructeur
    virtual ~CMeshInstance();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // M�thodes de contr�le
    //-------------------------------------------------------------------------------------------------

    //!
    void addMesh(QSP<CMesh> pMesh);

    //!
    CMeshInstance* clone();

    //-------------------------------------------------------------------------------------------------
    // M�thodes h�rit�es
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CMeshInstance; }

    //!
    virtual CBoundingBox getBounds();

    //!
    virtual CBoundingBox getWorldBounds();

    //! Renders the mesh
    virtual void paint(CRenderContext* pContext) Q_DECL_OVERRIDE;

protected:

    QVector<QSP<CMesh> >    m_vMeshes;
};
