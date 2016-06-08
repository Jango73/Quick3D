
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
    CMeshInstance(QSharedPointer<CMesh> pMesh);

    //! Constructeur avec une liste de meshs cibles
    CMeshInstance(const QVector<QSharedPointer<CMesh> >& vMeshes);

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
    void addMesh(QSharedPointer<CMesh> pMesh);

    //!
    CMeshInstance* clone();

    //-------------------------------------------------------------------------------------------------
    // M�thodes h�rit�es
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CMeshInstance; }

    //!
    virtual CBoundingBox getBounds() const;

    //!
    virtual CBoundingBox getWorldBounds() const;

    //! Renders the mesh
    virtual void paint(CRenderContext* pContext);

protected:

    QVector<QSharedPointer<CMesh> >	m_vMeshes;
};
