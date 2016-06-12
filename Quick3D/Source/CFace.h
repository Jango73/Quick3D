
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CVector3.h"
#include "CRay3.h"

//-------------------------------------------------------------------------------------------------
// D�clarations avanc�es

class CMesh;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CEdge
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Constructeur par d�faut
    CEdge()
    {
        m_aIndices[0] = 0;
        m_aIndices[1] = 0;
        m_iNewVertex = 0;
    }

    //! Constructeur avec deux indices de sommet
    CEdge(int iV1, int iV2)
    {
        m_aIndices[0] = iV1;
        m_aIndices[1] = iV2;
        m_iNewVertex = 0;
    }

    //! Constructeur avec deux indices de sommet et un indice de nouveau sommet
    CEdge(int iV1, int iV2, int iN)
    {
        m_aIndices[0] = iV1;
        m_aIndices[1] = iV2;
        m_iNewVertex = iN;
    }

    //-------------------------------------------------------------------------------------------------
    // M�thodes de contr�le
    //-------------------------------------------------------------------------------------------------

    //! Op�rateur de comparaison
    bool operator == (const CEdge& target) const
    {
        return	(m_aIndices[0] == target.m_aIndices[0] && m_aIndices[1] == target.m_aIndices[1]) ||
                (m_aIndices[0] == target.m_aIndices[1] && m_aIndices[1] == target.m_aIndices[0]);
    }

    //-------------------------------------------------------------------------------------------------
    // Propri�t�s
    //-------------------------------------------------------------------------------------------------

    int m_aIndices[2];
    int m_iNewVertex;
};

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CFace
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Constructeur par d�faut
    CFace();

    //! Constructeur avec parent
    CFace(CMesh* pParent);

    //! Constructeur de copie
    CFace(CMesh* pParent, const CFace&);

    //! Constructeur avec trois indices de sommet (triangle)
    CFace(CMesh* pParent, int iV1, int iV2, int iV3);

    //! Constructeur avec quatre indices de sommet (quad)
    CFace(CMesh* pParent, int iV1, int iV2, int iV3, int iV4);

    //! Constructeur avec n indices de sommet (polygone)
    CFace(CMesh* pParent, QVector<int> vIndices);

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setSmoothingGroup(int iGroup) { m_iSmoothingGroup = iGroup; }

    //!
    void setMaterialIndex(int iIndex) { m_iMaterialIndex = iIndex; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Retourne la liste des indice de sommets
    QVector<int>& getIndices() { return m_vIndices; }

    //! Retourne la liste des indice de sommets
    const QVector<int>& getIndices() const { return m_vIndices; }

    //! Retourne le vecteur normal de la facette
    Math::CVector3& getNormal() { return m_vNormal; }

    //! Retourne le vecteur normal de la facette
    Math::CVector3 getNormal() const { return m_vNormal; }

    //! Retourne le vecteur tangent de la facette
    Math::CVector3 getTangent() const { return m_vTangent; }

    //!
    int getSmoothingGroup() const { return m_iSmoothingGroup; }

    //!
    int getMaterialIndex() const { return m_iMaterialIndex; }

    //-------------------------------------------------------------------------------------------------
    // M�thodes de contr�le
    //-------------------------------------------------------------------------------------------------

    //! Op�rateur de copie
    CFace& operator = (const CFace& target);

    //! Op�rateur de tri
    bool operator < (const CFace& target) const;

    //! Calcule le vecteur normal de la facette
    void computeNormal();

    //! Calcul l'intersection entre le rayon fourni et la facette form�e par les trois points fournis
    static Math::RayTracingResult intersectTriangle(
            const Math::CRay3& ray,
            Math::CVector3 v0,
            Math::CVector3 v1,
            Math::CVector3 v2
            );

    //! Calcul l'intersection entre le rayon fourni et cette facette
    //! Retourne syst�matiquement l'infini pour l'instant
    Math::RayTracingResult intersect(Math::CRay3 ray);

    //-------------------------------------------------------------------------------------------------
    // Propri�t�s
    //-------------------------------------------------------------------------------------------------

protected:

    CMesh*          m_pParent;
    QVector<int>    m_vIndices;
    Math::CVector3  m_vNormal;
    Math::CVector3  m_vTangent;
    int             m_iSmoothingGroup;
    int             m_iMaterialIndex;
};
