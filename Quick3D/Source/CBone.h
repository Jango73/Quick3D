
#pragma once

// Qt
#include <QImage>
#include <QDateTime>

// Fondations
#include "CVector3.h"
#include "CMatrix4.h"
#include "CRay3.h"
#include "CXMLNode.h"

// Application
#include "CQ3DConstants.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;
class CArmature;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CBone
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //!
    CBone();

    //!
    virtual ~CBone();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setName(const QString& sName) { m_sName = sName; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    const QString& getName() const { return m_sName; }

    //-------------------------------------------------------------------------------------------------
    // M�thodes de contr�le
    //-------------------------------------------------------------------------------------------------

    //! Recherche les liens de cet objet
    void solveLinks(CArmature* pArmature);

    //-------------------------------------------------------------------------------------------------
    // M�thodes prot�g�s
    //-------------------------------------------------------------------------------------------------

protected:

    //-------------------------------------------------------------------------------------------------
    // Propri�t�s
    //-------------------------------------------------------------------------------------------------

protected:

    QString					m_sName;
    QString					m_sParentName;
    CBone*					m_pParent;

    Math::CVector3			m_vOriginPosition;				// Position de l'objet dans le rep�re affine de son parent
    Math::CVector3			m_vOriginRotation;				// Rotation de l'objet (euleur) dans le rep�re affine de son parent
    Math::CVector3			m_vOriginScale;					// Echelle de l'objet dans le rep�re affine de son parent
    Math::CVector3			m_vPosition;					// Position anim�e de l'objet dans son rep�re local
    Math::CVector3			m_vRotation;					// Rotation anim�e de l'objet (euleur) dans son rep�re local
    Math::CVector3			m_vScale;						// Echelle anim�e de l'objet dans son rep�re local
};
