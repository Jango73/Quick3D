
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
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    //! Recherche les liens de cet objet
    void solveLinks(CArmature* pArmature);

    //-------------------------------------------------------------------------------------------------
    // Méthodes protégés
    //-------------------------------------------------------------------------------------------------

protected:

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    QString					m_sName;
    QString					m_sParentName;
    CBone*					m_pParent;

    Math::CVector3			m_vOriginPosition;				// Position de l'objet dans le repère affine de son parent
    Math::CVector3			m_vOriginRotation;				// Rotation de l'objet (euleur) dans le repère affine de son parent
    Math::CVector3			m_vOriginScale;					// Echelle de l'objet dans le repère affine de son parent
    Math::CVector3			m_vPosition;					// Position animée de l'objet dans son repère local
    Math::CVector3			m_vRotation;					// Rotation animée de l'objet (euleur) dans son repère local
    Math::CVector3			m_vScale;						// Echelle animée de l'objet dans son repère local
};
