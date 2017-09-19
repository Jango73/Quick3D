
// qt-plus
#include "CLogger.h"

// Application
#include "COBJLoader.h"
#include "C3DScene.h"
#include "CMesh.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

IMPLEMENT_MEMORY_MONITORED(CMeshGeometry, "CMeshGeometry")

//-------------------------------------------------------------------------------------------------

CMeshGeometry::CMeshGeometry(C3DScene* pScene, double dMaxDistance, bool bUseSpacePartitionning)
    : m_pScene(pScene)
    , m_mMutex(QMutex::Recursive)
    , m_dMaxDistance(dMaxDistance)
    , m_iGLType(GL_TRIANGLES)
    , m_bUseSpacePartitionning(bUseSpacePartitionning)
    , m_bAutomaticBounds(true)
    , m_bGeometryDirty(true)
{
}

//-------------------------------------------------------------------------------------------------

CMeshGeometry::~CMeshGeometry()
{
    DELETE_VECTOR_ITEMS(m_vGLMeshData);

    deleteMaterials();
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::deleteMaterials()
{
    QMutexLocker locker(&m_mMutex);

    m_vMaterials.clear();
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::setURL(const QString& sURL)
{
    m_sURL = sURL;
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::setMaterial(QSP<CMaterial> pMaterial)
{
    deleteMaterials();

    m_vMaterials.append(m_pScene->ressourcesManager()->shareMaterial(pMaterial));
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::setBounds(CBoundingBox bBox)
{
    m_bAutomaticBounds = false;
    m_bBounds = bBox;
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::setGLType(int iGLType)
{
    m_iGLType = iGLType;
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::setGeometryDirty(bool bDirty)
{
    m_bGeometryDirty = bDirty;
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::clear()
{
    m_vVertices.clear();
    m_vFaces.clear();
    m_vVertexGroups.clear();

    m_bGeometryDirty = true;
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CMeshGeometry::bounds()
{
    checkAndUpdateGeometry();

    return m_bBounds;
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CMeshGeometry::worldBounds(CComponent* pContainer)
{
    CBoundingBox bBounds = bounds();

    // Get world position
    CVector3 vWorldPosition = pContainer->worldPosition();

    // Return the world bouding box
    return CBoundingBox(vWorldPosition + bBounds.minimum(), vWorldPosition + bBounds.maximum());
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::update(double dDeltaTime)
{
    foreach (QSP<CMaterial> pMaterial, m_vMaterials)
    {
        if (pMaterial != nullptr)
        {
            pMaterial->update(dDeltaTime);
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::checkAndUpdateGeometry()
{
    if (m_bGeometryDirty)
    {
        QMutexLocker locker(&m_mMutex);

        if (m_vVertices.count() > 0)
        {
            // Sort faces by material
            qSort(m_vFaces);

            // Compute normal vectors
            computeNormals();

            if (m_vMaterials.count() != m_vGLMeshData.count())
            {
                // Destroy OpenGL geometry buffers
                foreach (CGLMeshData* data, m_vGLMeshData)
                {
                    delete data;
                }

                m_vGLMeshData.clear();

                for (int iMaterialIndex = 0; iMaterialIndex < m_vMaterials.count(); iMaterialIndex++)
                {
                    CGLMeshData* pData = new CGLMeshData(m_pScene);
                    pData->m_iGLType = m_iGLType;
                    m_vGLMeshData.append(pData);
                }
            }

            // Reset the bouding box
            if (m_bAutomaticBounds)
            {
                m_bBounds.prepare();
            }

            for (int iMaterialIndex = 0; iMaterialIndex < m_vMaterials.count(); iMaterialIndex++)
            {
                CGLMeshData* pGLMeshData = m_vGLMeshData[iMaterialIndex];

                // Buffers must be transmitted to OpenGL
                pGLMeshData->m_bNeedTransferBuffers = true;

                if (m_iGLType == GL_POINTS || m_iGLType == GL_LINES)
                {
                    if (iMaterialIndex == 0)
                    {
                        pGLMeshData->m_iNumRenderPoints = m_vVertices.count();
                        pGLMeshData->m_iNumRenderIndices = m_vVertices.count();

                        // Create OpenGL geometry buffers
                        pGLMeshData->m_vRenderPoints = new CVertex[pGLMeshData->m_iNumRenderPoints];
                        pGLMeshData->m_vRenderIndices = new GLuint[pGLMeshData->m_iNumRenderIndices];

                        for (int iVertexIndex = 0; iVertexIndex < m_vVertices.count(); iVertexIndex++)
                        {
                            pGLMeshData->m_vRenderPoints[iVertexIndex] = m_vVertices[iVertexIndex];
                            pGLMeshData->m_vRenderIndices[iVertexIndex] = iVertexIndex;

                            // Check bounding box limits
                            if (m_bAutomaticBounds)
                            {
                                if (m_vVertices[iVertexIndex].position().X < m_bBounds.minimum().X) m_bBounds.minimum().X = m_vVertices[iVertexIndex].position().X;
                                if (m_vVertices[iVertexIndex].position().Y < m_bBounds.minimum().Y) m_bBounds.minimum().Y = m_vVertices[iVertexIndex].position().Y;
                                if (m_vVertices[iVertexIndex].position().Z < m_bBounds.minimum().Z) m_bBounds.minimum().Z = m_vVertices[iVertexIndex].position().Z;
                                if (m_vVertices[iVertexIndex].position().X > m_bBounds.maximum().X) m_bBounds.maximum().X = m_vVertices[iVertexIndex].position().X;
                                if (m_vVertices[iVertexIndex].position().Y > m_bBounds.maximum().Y) m_bBounds.maximum().Y = m_vVertices[iVertexIndex].position().Y;
                                if (m_vVertices[iVertexIndex].position().Z > m_bBounds.maximum().Z) m_bBounds.maximum().Z = m_vVertices[iVertexIndex].position().Z;
                            }
                        }
                    }
                }
                else
                {
                    QVector<int> vFaceIndices;

                    // Get faces for currrent material
                    for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
                    {
                        if (m_vFaces[iFaceIndex].materialIndex() == iMaterialIndex)
                        {
                            vFaceIndices.append(iFaceIndex);
                        }
                    }

                    if (vFaceIndices.count() > 0)
                    {
                        // Define quantities
                        pGLMeshData->m_iNumRenderPoints = m_vVertices.count();

                        if (m_iGLType == GL_QUADS)
                        {
                            pGLMeshData->m_iNumRenderIndices = vFaceIndices.count() * 4;
                        }
                        else
                        {
                            pGLMeshData->m_iNumRenderIndices = triangleCountForFaces(vFaceIndices) * 3;
                        }

                        if (pGLMeshData->m_iNumRenderPoints > 0 && pGLMeshData->m_iNumRenderIndices > 0)
                        {
                            /*
                            LOG_METHOD_DEBUG(QString("Allocating %1 vertices and %2 indices for %3")
                                .arg(pGLMeshData->m_iNumRenderPoints)
                                .arg(pGLMeshData->m_iNumRenderIndices)
                                .arg(m_sName)
                                );
                                */

                            // Création des buffers de géométrie OpenGL
                            pGLMeshData->m_vRenderPoints = new CVertex[pGLMeshData->m_iNumRenderPoints];
                            pGLMeshData->m_vRenderIndices = new GLuint[pGLMeshData->m_iNumRenderIndices];

                            // Fill OpenGL vertex buffer
                            for (int iVertex = 0; iVertex < m_vVertices.count(); iVertex++)
                            {
                                pGLMeshData->m_vRenderPoints[iVertex] = m_vVertices[iVertex];

                                // Check bounding box limits
                                if (m_vVertices[iVertex].position().X < m_bBounds.minimum().X) m_bBounds.minimum().X = m_vVertices[iVertex].position().X;
                                if (m_vVertices[iVertex].position().Y < m_bBounds.minimum().Y) m_bBounds.minimum().Y = m_vVertices[iVertex].position().Y;
                                if (m_vVertices[iVertex].position().Z < m_bBounds.minimum().Z) m_bBounds.minimum().Z = m_vVertices[iVertex].position().Z;
                                if (m_vVertices[iVertex].position().X > m_bBounds.maximum().X) m_bBounds.maximum().X = m_vVertices[iVertex].position().X;
                                if (m_vVertices[iVertex].position().Y > m_bBounds.maximum().Y) m_bBounds.maximum().Y = m_vVertices[iVertex].position().Y;
                                if (m_vVertices[iVertex].position().Z > m_bBounds.maximum().Z) m_bBounds.maximum().Z = m_vVertices[iVertex].position().Z;
                            }

                            m_bBounds.expand(CVector3(0.1, 0.1, 0.1));

                            // Fill OpenGL index buffer
                            int iIndiceIndex = 0;

                            if (m_iGLType == GL_QUADS)
                            {
                                for (int iFaceIndex = 0; iFaceIndex < vFaceIndices.count(); iFaceIndex++)
                                {
                                    CFace* pFace = &(m_vFaces[vFaceIndices[iFaceIndex]]);

                                    pGLMeshData->m_vRenderIndices[iIndiceIndex++] = pFace->indices()[0];
                                    pGLMeshData->m_vRenderIndices[iIndiceIndex++] = pFace->indices()[1];
                                    pGLMeshData->m_vRenderIndices[iIndiceIndex++] = pFace->indices()[2];
                                    pGLMeshData->m_vRenderIndices[iIndiceIndex++] = pFace->indices()[3];
                                }
                            }
                            else
                            {
                                for (int iFaceIndex = 0; iFaceIndex < vFaceIndices.count(); iFaceIndex++)
                                {
                                    CFace* pFace = &(m_vFaces[vFaceIndices[iFaceIndex]]);

                                    if (pFace->indices().count() > 2)
                                    {
                                        for (int iIndex = 2; iIndex < pFace->indices().count(); iIndex++)
                                        {
                                            pGLMeshData->m_vRenderIndices[iIndiceIndex++] = pFace->indices()[0];
                                            pGLMeshData->m_vRenderIndices[iIndiceIndex++] = pFace->indices()[iIndex - 1];
                                            pGLMeshData->m_vRenderIndices[iIndiceIndex++] = pFace->indices()[iIndex];
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // Spatial partitioning
            if (m_bUseSpacePartitionning)
            {
                createPartitions(m_bBounds);
            }
        }

        m_bGeometryDirty = false;
    }
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::addDataForPartition(CBoundPartition<int>& partition)
{
    // Iterate through polygons
    for (int iFace = 0; iFace < m_vFaces.count(); iFace++)
    {
        // Iterate through current polygon vertices
        for (int iVertex = 0; iVertex < m_vFaces[iFace].indices().count(); iVertex++)
        {
            // Get current vertex position
            CVector3 vPosition = m_vVertices[m_vFaces[iFace].indices()[iVertex]].position();

            // Do the partition bounds contain the vertex?
            if (partition.bounds().contains(vPosition))
            {
                // Yes, add the face to the partition and stop vertex iteration
                partition.append(iFace);
                break;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::isolateVertices()
{
    // Copy the vertex vector
    QVector<CVertex> vVertices(m_vVertices);

    // Clear the vertex vector
    m_vVertices.clear();

    // Iterate through polygons
    for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
    {
        for (int iVertexInFaceIndex = 0; iVertexInFaceIndex < m_vFaces[iFaceIndex].indices().count(); iVertexInFaceIndex++)
        {
            int iVertexIndex = m_vFaces[iFaceIndex].indices()[iVertexInFaceIndex];

            m_vFaces[iFaceIndex].indices()[iVertexInFaceIndex] = m_vVertices.count();

            m_vVertices.append(CVertex(vVertices[iVertexIndex]));
        }
    }

    m_bGeometryDirty = true;
}

//-------------------------------------------------------------------------------------------------

// Polygon spltting using smoothing groups
// Algorithm (c) 2015 Jonathan Guyomard
void CMeshGeometry::splitVerticesBySmoothingGroup()
{
    // Reset smoothing group sorting data
    {
        for (int iVertexIndex = 0; iVertexIndex < m_vVertices.count(); iVertexIndex++)
        {
            m_vVertices[iVertexIndex].m_vSmoothingGroups.clear();
            m_vVertices[iVertexIndex].m_vVertexIndicesForGroup.clear();
        }
    }

    // Retrieve smoothing groups by vertex
    // Iterate through polygons
    //   Iterate through the polygon's vertex
    //     In the vertex, associate to the polygon's smoothing group the vertex itself
    {
        for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
        {
            int iSmoothingGroup = m_vFaces[iFaceIndex].smoothingGroup();

            for (int iVertexInFaceIndex = 0; iVertexInFaceIndex < m_vFaces[iFaceIndex].indices().count(); iVertexInFaceIndex++)
            {
                int iVertexInMeshIndex = m_vFaces[iFaceIndex].indices()[iVertexInFaceIndex];

                if (m_vVertices[iVertexInMeshIndex].m_vSmoothingGroups.contains(iSmoothingGroup) == false)
                {
                    m_vVertices[iVertexInMeshIndex].m_vSmoothingGroups.append(iSmoothingGroup);
                    m_vVertices[iVertexInMeshIndex].m_vVertexIndicesForGroup.append(iVertexInMeshIndex);
                }
            }
        }
    }

    // Duplicate the vertices
    // Iterate through vertices
    //   If the vertex is associated to more than one smoothing group
    {
        int iCurrentVertexCount = m_vVertices.count();

        for (int iVertexInMeshIndex = 0; iVertexInMeshIndex < iCurrentVertexCount; iVertexInMeshIndex++)
        {
            if (m_vVertices[iVertexInMeshIndex].m_vSmoothingGroups.count() > 1)
            {
                if (m_vVertices[iVertexInMeshIndex].m_vSmoothingGroups.count() == m_vVertices[iVertexInMeshIndex].m_vVertexIndicesForGroup.count())
                {
                    for (int iGroupIndex = 1; iGroupIndex < m_vVertices[iVertexInMeshIndex].m_vSmoothingGroups.count(); iGroupIndex++)
                    {
                        int iNewVertexInMeshIndex = m_vVertices.count();
                        CVertex vNewVertex(m_vVertices[iVertexInMeshIndex]);
                        m_vVertices.append(vNewVertex);
                        m_vVertices[iVertexInMeshIndex].m_vVertexIndicesForGroup[iGroupIndex] = iNewVertexInMeshIndex;
                    }
                }
                else
                {
                    LOG_WARNING("CMesh::splitVerticesBySmoothingGroup() : Inconsistent count of m_vSmoothingGroups");
                }
            }
        }
    }

    // Assign the new vertices to polygons
    {
        for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
        {
            int iSmoothingGroup = m_vFaces[iFaceIndex].smoothingGroup();

            for (int iVertexInFaceIndex = 0; iVertexInFaceIndex < m_vFaces[iFaceIndex].indices().count(); iVertexInFaceIndex++)
            {
                int iVertexInMeshIndex = m_vFaces[iFaceIndex].indices()[iVertexInFaceIndex];

                if (m_vVertices[iVertexInMeshIndex].m_vSmoothingGroups.contains(iSmoothingGroup))
                {
                    int iGroupIndex = m_vVertices[iVertexInMeshIndex].m_vSmoothingGroups.indexOf(iSmoothingGroup);
                    m_vFaces[iFaceIndex].indices()[iVertexInFaceIndex] = m_vVertices[iVertexInMeshIndex].m_vVertexIndicesForGroup[iGroupIndex];
                }
            }
        }
    }

    // Reset data related to smoothing group sorting
    {
        for (int iVertexIndex = 0; iVertexIndex < m_vVertices.count(); iVertexIndex++)
        {
            m_vVertices[iVertexIndex].m_vSmoothingGroups.clear();
            m_vVertices[iVertexIndex].m_vVertexIndicesForGroup.clear();
        }
    }

    m_bGeometryDirty = true;
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::flipNormals()
{
    // Iterate through each polygon
    for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
    {
        QVector<int> vIndices;

        // Get vertex indices in reverse order
        for (int iVertexIndex = 0; iVertexIndex < m_vFaces[iFaceIndex].indices().count(); iVertexIndex++)
        {
            vIndices.prepend(m_vFaces[iFaceIndex].indices()[iVertexIndex]);
        }

        // Store vertex indices
        for (int iVertexIndex = 0; iVertexIndex < m_vFaces[iFaceIndex].indices().count(); iVertexIndex++)
        {
            m_vFaces[iFaceIndex].indices()[iVertexIndex] = vIndices[iVertexIndex];
        }
    }

    m_bGeometryDirty = true;
}

//-------------------------------------------------------------------------------------------------

int CMeshGeometry::triangleCount()
{
    int iNumTriangles = 0;

    foreach (const CFace& face, m_vFaces)
    {
        if (face.indices().count() > 2)
        {
            iNumTriangles += face.indices().count() - 2;
        }
    }

    return iNumTriangles;
}

//-------------------------------------------------------------------------------------------------

int CMeshGeometry::triangleCountForFaces(const QVector<int>& vFaceIndices)
{
    int iNumTriangles = 0;

    for (int iFaceIndex = 0; iFaceIndex < vFaceIndices.count(); iFaceIndex++)
    {
        CFace* pFace = &(m_vFaces[vFaceIndices[iFaceIndex]]);

        if (pFace->indices().count() > 2)
        {
            iNumTriangles += pFace->indices().count() - 2;
        }
    }

    return iNumTriangles;
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::computeNormals()
{
    if (m_iGLType == GL_POINTS)
    {
    }
    else
    {
        // Remise à zéro des normales de polygone
        for (int Index = 0; Index < m_vVertices.count(); Index++)
        {
            m_vVertices[Index].normal() = CVector3(0.0, 0.0, 0.0);
            m_vVertices[Index].tangent() = CVector3(0.0, 0.0, 0.0);
            m_vVertices[Index].normalDivider() = 0.0;
        }

        // Parcours de tous les polygones
        for (int FaceIndex = 0; FaceIndex < m_vFaces.count(); FaceIndex++)
        {
            // Calcul du vecteur normal du polygone
            m_vFaces[FaceIndex].computeNormal();

            // Traitement de chaque vertex du polygone
            foreach (int VertexIndex, m_vFaces[FaceIndex].indices())
            {
                m_vVertices[VertexIndex].normal() += m_vFaces[FaceIndex].normal();
                m_vVertices[VertexIndex].tangent() += m_vFaces[FaceIndex].tangent();
                m_vVertices[VertexIndex].normalDivider() += 1.0;
            }
        }

        // Division des normales et tangentes de chaque vertex
        for (int Index = 0; Index < m_vVertices.count(); Index++)
        {
            if (m_vVertices[Index].normalDivider() > 0.0)
            {
                m_vVertices[Index].normal() /= m_vVertices[Index].normalDivider();
                m_vVertices[Index].tangent() /= m_vVertices[Index].normalDivider();
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

#define COORD_STEP	((float) (1.0 / 4.0))

void CMeshGeometry::createBox(Math::CVector3 vMinimum, Math::CVector3 vMaximum)
{
    clear();

    m_iGLType = GL_QUADS;

    // Front
    m_vVertices.append(CVertex(CVector3(vMinimum.X, vMaximum.Y, vMaximum.Z), CVector2(COORD_STEP * 1.0, COORD_STEP * 2.0)));
    m_vVertices.append(CVertex(CVector3(vMaximum.X, vMaximum.Y, vMaximum.Z), CVector2(COORD_STEP * 2.0, COORD_STEP * 2.0)));
    m_vVertices.append(CVertex(CVector3(vMaximum.X, vMinimum.Y, vMaximum.Z), CVector2(COORD_STEP * 2.0, COORD_STEP * 1.0)));
    m_vVertices.append(CVertex(CVector3(vMinimum.X, vMinimum.Y, vMaximum.Z), CVector2(COORD_STEP * 1.0, COORD_STEP * 1.0)));

    // Top
    m_vVertices.append(CVertex(CVector3(vMaximum.X, vMaximum.Y, vMaximum.Z), CVector2(COORD_STEP * 2.0, COORD_STEP * 2.0)));
    m_vVertices.append(CVertex(CVector3(vMinimum.X, vMaximum.Y, vMaximum.Z), CVector2(COORD_STEP * 1.0, COORD_STEP * 2.0)));
    m_vVertices.append(CVertex(CVector3(vMinimum.X, vMaximum.Y, vMinimum.Z), CVector2(COORD_STEP * 1.0, COORD_STEP * 3.0)));
    m_vVertices.append(CVertex(CVector3(vMaximum.X, vMaximum.Y, vMinimum.Z), CVector2(COORD_STEP * 2.0, COORD_STEP * 3.0)));

    // Right
    m_vVertices.append(CVertex(CVector3(vMaximum.X, vMaximum.Y, vMaximum.Z), CVector2(COORD_STEP * 2.0, COORD_STEP * 2.0)));
    m_vVertices.append(CVertex(CVector3(vMaximum.X, vMaximum.Y, vMinimum.Z), CVector2(COORD_STEP * 3.0, COORD_STEP * 2.0)));
    m_vVertices.append(CVertex(CVector3(vMaximum.X, vMinimum.Y, vMinimum.Z), CVector2(COORD_STEP * 3.0, COORD_STEP * 1.0)));
    m_vVertices.append(CVertex(CVector3(vMaximum.X, vMinimum.Y, vMaximum.Z), CVector2(COORD_STEP * 2.0, COORD_STEP * 1.0)));

    // Left
    m_vVertices.append(CVertex(CVector3(vMinimum.X, vMaximum.Y, vMinimum.Z), CVector2(COORD_STEP * 0.0, COORD_STEP * 2.0)));
    m_vVertices.append(CVertex(CVector3(vMinimum.X, vMaximum.Y, vMaximum.Z), CVector2(COORD_STEP * 1.0, COORD_STEP * 2.0)));
    m_vVertices.append(CVertex(CVector3(vMinimum.X, vMinimum.Y, vMaximum.Z), CVector2(COORD_STEP * 1.0, COORD_STEP * 1.0)));
    m_vVertices.append(CVertex(CVector3(vMinimum.X, vMinimum.Y, vMinimum.Z), CVector2(COORD_STEP * 0.0, COORD_STEP * 1.0)));

    // Back
    m_vVertices.append(CVertex(CVector3(vMaximum.X, vMaximum.Y, vMinimum.Z), CVector2(COORD_STEP * 2.0, COORD_STEP * 3.0)));
    m_vVertices.append(CVertex(CVector3(vMinimum.X, vMaximum.Y, vMinimum.Z), CVector2(COORD_STEP * 1.0, COORD_STEP * 3.0)));
    m_vVertices.append(CVertex(CVector3(vMinimum.X, vMinimum.Y, vMinimum.Z), CVector2(COORD_STEP * 1.0, COORD_STEP * 4.0)));
    m_vVertices.append(CVertex(CVector3(vMaximum.X, vMinimum.Y, vMinimum.Z), CVector2(COORD_STEP * 2.0, COORD_STEP * 4.0)));

    // Bottom
    m_vVertices.append(CVertex(CVector3(vMinimum.X, vMinimum.Y, vMaximum.Z), CVector2(COORD_STEP * 1.0, COORD_STEP * 1.0)));
    m_vVertices.append(CVertex(CVector3(vMaximum.X, vMinimum.Y, vMaximum.Z), CVector2(COORD_STEP * 2.0, COORD_STEP * 1.0)));
    m_vVertices.append(CVertex(CVector3(vMaximum.X, vMinimum.Y, vMinimum.Z), CVector2(COORD_STEP * 2.0, COORD_STEP * 0.0)));
    m_vVertices.append(CVertex(CVector3(vMinimum.X, vMinimum.Y, vMinimum.Z), CVector2(COORD_STEP * 1.0, COORD_STEP * 0.0)));

    m_vFaces.append(CFace(this,  0,  1,  2,  3));
    m_vFaces.append(CFace(this,  4,  5,  6,  7));
    m_vFaces.append(CFace(this,  8,  9, 10, 11));
    m_vFaces.append(CFace(this, 12, 13, 14, 15));
    m_vFaces.append(CFace(this, 16, 17, 18, 19));
    m_vFaces.append(CFace(this, 20, 21, 22, 23));
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::createSphere(int iNumSegments)
{
    clear();

    // Le nombre de segments doit être paire
    if ((iNumSegments & 1) == 1)
    {
        iNumSegments++;
    }

    // et supérieur à 3
    if (iNumSegments < 4)
    {
        iNumSegments = 4;
    }

    int iPanSegments = iNumSegments;
    int iTiltSegments = iNumSegments / 2;

    // Sommet du dessus
    m_vVertices.append(CVertex(CVector3(0.0, 1.0, 0.0), CVector2(0.0, 0.0)));

    // Sommets intermédiaires
    for (int iTiltIndex = 1; iTiltIndex < iTiltSegments; iTiltIndex++)
    {
        double dCurrentTilt = (((double) iTiltIndex / (double) iTiltSegments) * Math::Pi);
        CMatrix4 mRotationTilt = CMatrix4().makeRotation(CVector3(dCurrentTilt, 0.0, 0.0));

        for (int iPanIndex = 0; iPanIndex < iPanSegments; iPanIndex++)
        {
            double dCurrentPan = ((double) iPanIndex / (double) iPanSegments) * (Math::Pi * 2.0);
            CMatrix4 mRotationPan = CMatrix4().makeRotation(CVector3(0.0, dCurrentPan, 0.0));

            CVector3 vPosition = mRotationPan * (mRotationTilt * CVector3(0.0, 1.0, 0.0));

            m_vVertices.append(CVertex(vPosition, CVector2(0.0, 0.0)));
        }
    }

    // Sommet du dessous
    m_vVertices.append(CVertex(CVector3(0.0, -1.0, 0.0), CVector2(0.0, 0.0)));

    int iVertexOffset = 0;

    // Top faces
    for (int iPanIndex = 0; iPanIndex < iPanSegments; iPanIndex++)
    {
        int v1 = iVertexOffset + 0;
        int v2 = iVertexOffset + 1 + iPanIndex + 1;
        int v3 = iVertexOffset + 1 + iPanIndex;

        if (iPanIndex == iPanSegments - 1)
        {
            v2 -= iPanSegments;
        }

        m_vFaces.append(CFace(this, v1, v2, v3));
    }

    iVertexOffset++;

    // Middle faces
    for (int iTiltIndex = 1; iTiltIndex < iTiltSegments - 1; iTiltIndex++)
    {
        for (int iPanIndex = 0; iPanIndex < iPanSegments; iPanIndex++)
        {
            int v1 = iVertexOffset + 0 + iPanIndex;
            int v2 = iVertexOffset + 1 + iPanIndex;
            int v3 = iVertexOffset + 1 + iPanIndex + iPanSegments;
            int v4 = iVertexOffset + 0 + iPanIndex + iPanSegments;

            if (iPanIndex == iPanSegments - 1)
            {
                v2 -= iPanSegments;
                v3 -= iPanSegments;
            }

            m_vFaces.append(CFace(this, v1, v2, v3, v4));
        }

        iVertexOffset += iPanSegments;
    }

    // Bottom faces
    for (int iPanIndex = 0; iPanIndex < iPanSegments; iPanIndex++)
    {
        int v1 = iVertexOffset + 1 + iPanIndex;
        int v2 = iVertexOffset + 1 + iPanIndex + 1;
        int v3 = iVertexOffset + 0 + iPanSegments;

        if (iPanIndex == iPanSegments - 1)
        {
            v2 -= iPanSegments;
        }

        m_vFaces.append(CFace(this, v1, v2, v3));
    }
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::createSpherePart(
        int iPanSegments,
        int iTiltSegments,
        double dStartPanDegrees,
        double dEndPanDegrees,
        double dStartTiltDegrees,
        double dEndTiltDegrees
        )
{
    clear();

    // Le nombre de segments en gisement doit être paire
    if ((iPanSegments & 1) == 1)
    {
        iPanSegments++;
    }

    // Le nombre de segments en site doit être paire
    if ((iTiltSegments & 1) == 1)
    {
        iTiltSegments++;
    }

    // L'angle gisement de début doit être inférieur à l'angle de fin
    if (dStartPanDegrees > dEndPanDegrees)
    {
        double dTemp = dStartPanDegrees;
        dStartPanDegrees = dEndPanDegrees;
        dEndPanDegrees = dTemp;
    }

    // L'angle site de début doit être inférieur à l'angle de fin
    if (dStartTiltDegrees > dEndTiltDegrees)
    {
        double dTemp = dStartTiltDegrees;
        dStartTiltDegrees = dEndTiltDegrees;
        dEndTiltDegrees = dTemp;
    }

    // Calcul de la course des angles gisement et site
    double dPanSpanDegrees = dEndPanDegrees - dStartPanDegrees;
    double dTiltSpanDegrees = dEndTiltDegrees - dStartTiltDegrees;

    // Sommets
    for (int iTiltIndex = 0; iTiltIndex < (iTiltSegments + 1); iTiltIndex++)
    {
        double dCurrentTiltNormalized = (double) iTiltIndex / (double) iTiltSegments;
        double dCurrentTilt = dStartTiltDegrees + ((dCurrentTiltNormalized) * dTiltSpanDegrees);

        // Création de la matrice de rotation en site
        CMatrix4 mRotationTilt = CMatrix4().makeRotation(CVector3(Math::Angles::toRad(dCurrentTilt), 0.0, 0.0));

        for (int iPanIndex = 0; iPanIndex < (iPanSegments + 1); iPanIndex++)
        {
            double dCurrentPanNormalized = (double) iPanIndex / (double) iPanSegments;
            double dCurrentPan = dStartPanDegrees + ((dCurrentPanNormalized) * dPanSpanDegrees);

            // Création de la matrice de rotation en gisement
            CMatrix4 mRotationPan = CMatrix4().makeRotation(CVector3(0.0, Math::Angles::toRad(dCurrentPan), 0.0));

            // Création du vecteur de position du sommet
            CVector3 vPosition = mRotationPan * (mRotationTilt * CVector3(0.0, 1.0, 0.0));

            // Ajout du sommet
            m_vVertices.append(CVertex(
                                   vPosition,
                                   CVector2(dCurrentPanNormalized, 1.0 - dCurrentTiltNormalized))
                               );
        }
    }

    int iVertexOffset = 0;

    // Facettes
    for (int iTiltIndex = 0; iTiltIndex < iTiltSegments; iTiltIndex++)
    {
        for (int iPanIndex = 0; iPanIndex < iPanSegments; iPanIndex++)
        {
            int v1 = iVertexOffset + 0 + iPanIndex;
            int v2 = iVertexOffset + 1 + iPanIndex;
            int v3 = iVertexOffset + 1 + iPanIndex + (iPanSegments + 1);
            int v4 = iVertexOffset + 0 + iPanIndex + (iPanSegments + 1);

            m_vFaces.append(CFace(this, v1, v2, v3, v4));
        }

        iVertexOffset += (iPanSegments + 1);
    }
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::createCone(int iNumSegments, int iNumHeightSegments, double dHeight, double dBaseRadius, double dApexRadius)
{
    clear();

    m_iGLType = GL_QUADS;

    if (iNumSegments < 2) iNumSegments = 2;

    double dRadiusRange = dApexRadius - dBaseRadius;

    for (int iHeightIndex = 0; iHeightIndex <= iNumHeightSegments; iHeightIndex++)
    {
        double dCurrentHeight = ((double) iHeightIndex / (double) iNumHeightSegments) * dHeight;

        for (int iIndex = 0; iIndex < iNumSegments; iIndex++)
        {
            double dCurrentRadius = dBaseRadius + ((double) iHeightIndex / (double) iNumHeightSegments) * dRadiusRange;
            CVector3 v1(0.0, dCurrentHeight, dCurrentRadius);

            double dU = (double) iIndex / (double) iNumSegments;

            double dAngle = dU * (Math::Pi * 2.0);

            CMatrix4 mRotate = CMatrix4().makeRotation(CVector3(0.0, dAngle, 0.0));

            double dTexU = (double) iIndex / (double) (iNumSegments - 1);
            double dTexV = (double) iHeightIndex / (double) (iNumHeightSegments - 1);

            m_vVertices.append(CVertex(mRotate * v1, CVector2(dTexU, dTexV)));
        }
    }

    for (int iHeightIndex = 0; iHeightIndex < iNumHeightSegments; iHeightIndex++)
    {
        for (int iIndex = 0; iIndex < iNumSegments; iIndex++)
        {
            int iV1 = ((iHeightIndex + 0) * iNumSegments) + (iIndex + 0);
            int iV2 = ((iHeightIndex + 0) * iNumSegments) + (iIndex + 1);
            int iV3 = ((iHeightIndex + 1) * iNumSegments) + (iIndex + 1);
            int iV4 = ((iHeightIndex + 1) * iNumSegments) + (iIndex + 0);

            if (iIndex == iNumSegments - 1)
            {
                iV2 = ((iHeightIndex + 0) * iNumSegments) + 0;
                iV3 = ((iHeightIndex + 1) * iNumSegments) + 0;
            }

            m_vFaces.append(CFace(this, iV4, iV3, iV2, iV1));
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::createAdaptiveTriPatch(Math::CVector3 vCenter, int iNumIterations)
{
    clear();

    // Create initial vertices and faces
    m_vVertices.append(CVertex(CVector3( 0.0, 0.0,  0.0), CVector2( 0.5,  0.5)));
    m_vVertices.append(CVertex(CVector3(-1.0, 0.0,  0.0), CVector2( 0.0,  0.0)));
    m_vVertices.append(CVertex(CVector3( 1.0, 0.0,  0.0), CVector2( 1.0,  0.0)));
    m_vVertices.append(CVertex(CVector3( 1.0, 0.0,  1.0), CVector2( 1.0,  1.0)));
    m_vVertices.append(CVertex(CVector3(-1.0, 0.0,  1.0), CVector2( 0.0,  1.0)));

    // m_vFaces.append(CFace(1, 2, 0));
    m_vFaces.append(CFace(this, 2, 3, 0));
    m_vFaces.append(CFace(this, 3, 4, 0));
    m_vFaces.append(CFace(this, 4, 1, 0));

    double dDistance = 1.0;

    for (int iIteration = 0; iIteration < iNumIterations; iIteration++)
    {
        QVector<CEdge> vEdges;

        for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
        {
            // Get vertex indices
            int iV1 = m_vFaces[iFaceIndex].indices()[0];
            int iV2 = m_vFaces[iFaceIndex].indices()[1];
            int iV3 = m_vFaces[iFaceIndex].indices()[2];

            // Get vertex positions
            CVector3 pV1 = m_vVertices[iV1].position();
            CVector3 pV2 = m_vVertices[iV2].position();
            CVector3 pV3 = m_vVertices[iV3].position();

            // Get face centroid
            CVector3 vCenter = (pV1 + pV2 + pV3) / 3.0;

            // Subdivide the face if centroid is less than current distance
            if (vCenter.magnitude() < dDistance)
            {
                // Get edge lengths
                double dEdgeAB = (pV2 - pV1).magnitude();
                double dEdgeBC = (pV3 - pV2).magnitude();
                double dEdgeCA = (pV1 - pV3).magnitude();

                if (dEdgeAB >= dEdgeBC && dEdgeAB >= dEdgeCA)
                {
                    vEdges.append(CEdge(iV1, iV2));
                }
                else
                    if (dEdgeBC >= dEdgeAB && dEdgeBC >= dEdgeCA)
                    {
                        vEdges.append(CEdge(iV2, iV3));
                    }
                    else
                    {
                        vEdges.append(CEdge(iV3, iV1));
                    }
            }
        }

        QVector<CFace> vNewFaces;

        for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
        {
            // Get vertex indices
            int iV1 = m_vFaces[iFaceIndex].indices()[0];
            int iV2 = m_vFaces[iFaceIndex].indices()[1];
            int iV3 = m_vFaces[iFaceIndex].indices()[2];

            // Get vertex positions
            CVector3 pV1 = m_vVertices[iV1].position();
            CVector3 pV2 = m_vVertices[iV2].position();
            CVector3 pV3 = m_vVertices[iV3].position();

            int iVN = m_vVertices.count();

            if (vEdges.contains(CEdge(iV1, iV2)))
            {
                m_vVertices.append(CVertex((pV1 + pV2) / 2.0, CVector2( 0.0,  0.0)));

                vNewFaces.append(CFace(this, iV1, iVN, iV3));
                vNewFaces.append(CFace(this, iVN, iV2, iV3));

                m_vFaces.remove(iFaceIndex); iFaceIndex--;
            }
            else
                if (vEdges.contains(CEdge(iV2, iV3)))
                {
                    m_vVertices.append(CVertex((pV2 + pV3) / 2.0, CVector2( 0.0,  0.0)));

                    vNewFaces.append(CFace(this, iV1, iV2, iVN));
                    vNewFaces.append(CFace(this, iVN, iV3, iV1));

                    m_vFaces.remove(iFaceIndex); iFaceIndex--;
                }
                else
                    if (vEdges.contains(CEdge(iV3, iV1)))
                    {
                        m_vVertices.append(CVertex((pV3 + pV1) / 2.0, CVector2( 0.0,  0.0)));

                        vNewFaces.append(CFace(this, iV1, iV2, iVN));
                        vNewFaces.append(CFace(this, iV2, iV3, iVN));

                        m_vFaces.remove(iFaceIndex); iFaceIndex--;
                    }
        }

        foreach (const CFace& face, vNewFaces)
        {
            m_vFaces.append(face);
        }

        dDistance *= 0.8;
    }
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::createAdaptiveQuadPatch(Math::CVector3 vCenter, int iNumIterations)
{
    clear();

    m_iGLType = GL_QUADS;

    // Create initial vertices and faces
    m_vVertices.append(CVertex(CVector3(-1.0, 0.0, -0.05), CVector2( 0.0,  0.0)));
    m_vVertices.append(CVertex(CVector3( 1.0, 0.0, -0.05), CVector2( 1.0,  0.0)));
    m_vVertices.append(CVertex(CVector3( 1.0, 0.0,  1.00), CVector2( 1.0,  1.0)));
    m_vVertices.append(CVertex(CVector3(-1.0, 0.0,  1.00), CVector2( 0.0,  1.0)));

    m_vFaces.append(CFace(this, 0, 1, 2, 3));

    double dDistance = 1.0;

    for (int iIteration = 0; iIteration < iNumIterations; iIteration++)
    {
        QVector<CEdge> vEdges;

        for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
        {
            // Get vertex indices
            int iVA = m_vFaces[iFaceIndex].indices()[0];
            int iVB = m_vFaces[iFaceIndex].indices()[1];
            int iVC = m_vFaces[iFaceIndex].indices()[2];
            int iVD = m_vFaces[iFaceIndex].indices()[3];

            // Get vertex positions
            CVector3 pVA = m_vVertices[iVA].position();
            CVector3 pVB = m_vVertices[iVB].position();
            CVector3 pVC = m_vVertices[iVC].position();
            CVector3 pVD = m_vVertices[iVD].position();

            // Get face centroid
            CVector3 vCenter = (pVA + pVB + pVC + pVD) / 4.0;

            // Subdivide the face if centroid is less than current distance
            if (vCenter.magnitude() < dDistance)
            {
                if (!(vEdges.contains(CEdge(iVA, iVB)) || vEdges.contains(CEdge(iVB, iVA))))
                {
                    vEdges.append(CEdge(iVA, iVB, m_vVertices.count()));
                    m_vVertices.append(CVertex((pVA + pVB) / 2.0, CVector2( 0.0,  0.0)));
                }

                if (!(vEdges.contains(CEdge(iVB, iVC)) || vEdges.contains(CEdge(iVC, iVB))))
                {
                    vEdges.append(CEdge(iVB, iVC, m_vVertices.count()));
                    m_vVertices.append(CVertex((pVB + pVC) / 2.0, CVector2( 0.0,  0.0)));
                }

                if (!(vEdges.contains(CEdge(iVC, iVD)) || vEdges.contains(CEdge(iVD, iVC))))
                {
                    vEdges.append(CEdge(iVC, iVD, m_vVertices.count()));
                    m_vVertices.append(CVertex((pVC + pVD) / 2.0, CVector2( 0.0,  0.0)));
                }

                if (!(vEdges.contains(CEdge(iVD, iVA)) || vEdges.contains(CEdge(iVA, iVD))))
                {
                    vEdges.append(CEdge(iVD, iVA, m_vVertices.count()));
                    m_vVertices.append(CVertex((pVD + pVA) / 2.0, CVector2( 0.0,  0.0)));
                }
            }
        }

        QVector<CFace> vNewFaces;

        for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
        {
            // Get vertex indices
            int iVA = m_vFaces[iFaceIndex].indices()[0];
            int iVB = m_vFaces[iFaceIndex].indices()[1];
            int iVC = m_vFaces[iFaceIndex].indices()[2];
            int iVD = m_vFaces[iFaceIndex].indices()[3];

            if (
                    vEdges.contains(CEdge(iVA, iVB)) && vEdges.contains(CEdge(iVB, iVC)) &&
                    vEdges.contains(CEdge(iVC, iVD)) && vEdges.contains(CEdge(iVD, iVA))
                    )
            {
                int iEAB = vEdges.indexOf(CEdge(iVA, iVB));
                int iEBC = vEdges.indexOf(CEdge(iVB, iVC));
                int iECD = vEdges.indexOf(CEdge(iVC, iVD));
                int iEDA = vEdges.indexOf(CEdge(iVD, iVA));

                // Get vertex positions
                CVector3 pVA = m_vVertices[iVA].position();
                CVector3 pVB = m_vVertices[iVB].position();
                CVector3 pVC = m_vVertices[iVC].position();
                CVector3 pVD = m_vVertices[iVD].position();

                int iVN = m_vVertices.count();

                m_vVertices.append(CVertex((pVA + pVB + pVC + pVD) / 4.0, CVector2( 0.0,  0.0)));

                // Subdivide
                vNewFaces.append(CFace(this, iVA, vEdges[iEAB].m_iNewVertex, iVN, vEdges[iEDA].m_iNewVertex));
                vNewFaces.append(CFace(this, vEdges[iEAB].m_iNewVertex, iVB, vEdges[iEBC].m_iNewVertex, iVN));
                vNewFaces.append(CFace(this, iVN, vEdges[iEBC].m_iNewVertex, iVC, vEdges[iECD].m_iNewVertex));
                vNewFaces.append(CFace(this, vEdges[iEDA].m_iNewVertex, iVN, vEdges[iECD].m_iNewVertex, iVD));

                m_vFaces.remove(iFaceIndex); iFaceIndex--;
            }
        }

        foreach (const CFace& face, vNewFaces)
        {
            m_vFaces.append(face);
        }

        dDistance *= 0.65;
    }
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::createQuadPatch(int iNumVerts, int iNumSkirtVerts, bool bDoubleSided)
{
    clear();

    m_iGLType = GL_QUADS;

    int iNumSkirtVertsX2 = iNumSkirtVerts * 2;
    double dSpan = (1.0 / (double) iNumVerts) * ((double) iNumVerts + (double) iNumSkirtVertsX2);
    double dHalf = dSpan * 0.5;
    int iTotalVerts = iNumVerts + iNumSkirtVertsX2;

    for (int z = 0; z < iTotalVerts; z++)
    {
        for (int x = 0; x < iTotalVerts; x++)
        {
            double dx = (((double) x / (double) (iTotalVerts - 1)) * dSpan) - dHalf;
            double dz = (((double) z / (double) (iTotalVerts - 1)) * dSpan) - dHalf;

            double u = (double) x / (double) (iTotalVerts - 1);
            double v = (double) z / (double) (iTotalVerts - 1);

            m_vVertices.append(CVertex(CVector3(dx, 0.0, dz), CVector2((double) u, (double) v)));
        }
    }

    for (int z = 0; z < iTotalVerts - 1; z++)
    {
        for (int x = 0; x < iTotalVerts - 1; x++)
        {
            int v1 = ((z + 0) * iTotalVerts) + x + 0;
            int v2 = ((z + 0) * iTotalVerts) + x + 1;
            int v3 = ((z + 1) * iTotalVerts) + x + 1;
            int v4 = ((z + 1) * iTotalVerts) + x + 0;

            m_vFaces.append(CFace(this, v1, v2, v3, v4));
        }
    }

    if (bDoubleSided)
    {
        int iVertexOffset = m_vVertices.count();

        for (int z = 0; z < iTotalVerts; z++)
        {
            for (int x = 0; x < iTotalVerts; x++)
            {
                double dx = (((double) x / (double) (iTotalVerts - 1)) * dSpan) - dHalf;
                double dz = (((double) z / (double) (iTotalVerts - 1)) * dSpan) - dHalf;

                double u = (double) x / (double) (iTotalVerts - 1);
                double v = (double) z / (double) (iTotalVerts - 1);

                m_vVertices.append(CVertex(CVector3(dx, 0.0, dz), CVector2((double) u, (double) v)));
            }
        }

        for (int z = 0; z < iTotalVerts - 1; z++)
        {
            for (int x = 0; x < iTotalVerts - 1; x++)
            {
                int v1 = iVertexOffset + ((z + 0) * iTotalVerts) + x + 0;
                int v2 = iVertexOffset + ((z + 0) * iTotalVerts) + x + 1;
                int v3 = iVertexOffset + ((z + 1) * iTotalVerts) + x + 1;
                int v4 = iVertexOffset + ((z + 1) * iTotalVerts) + x + 0;

                m_vFaces.append(CFace(this, v4, v3, v2, v1));
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::createCircularQuadPatch(Math::CVector3 vCenter, int iNumVerts)
{
    clear();

    m_iGLType = GL_QUADS;

    for (int z = 0; z < iNumVerts; z++)
    {
        for (int x = 0; x < iNumVerts; x++)
        {
            double dx = (double) x / (double) (iNumVerts - 1);
            double dz = (double) z / (double) (iNumVerts - 1);

            double ax = (-0.5 + pow(dx, 3.0)) * 0.05;
            // double az = pow(dz, 3.0);
            double az = (-0.5 + pow(dz, 3.0)) * 0.05;

            /*
            QMatrix4x4 mMatrix;
            mMatrix.setToIdentity();
            mMatrix.rotate((-0.5 + dx) * -45.0, QVector3D(0, 1, 0));
            */

            QVector3D vertex(ax, 0.0, az);
            // vertex = vertex * mMatrix;

            m_vVertices.append(CVertex(CVector3(vertex.x(), vertex.y(), vertex.z()), CVector2(0.0, 0.0)));
        }
    }

    for (int z = 0; z < iNumVerts - 1; z++)
    {
        for (int x = 0; x < iNumVerts - 1; x++)
        {
            int v1 = ((z + 0) * iNumVerts) + x + 0;
            int v2 = ((z + 0) * iNumVerts) + x + 1;
            int v3 = ((z + 1) * iNumVerts) + x + 1;
            int v4 = ((z + 1) * iNumVerts) + x + 0;

            m_vFaces.append(CFace(this, v1, v2, v3, v4));
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::createSurfaceFromFFD(const QVector<Math::CVector3>& vFFDFrom, const QVector<Math::CVector3>& vFFDTo, int iNumVerts)
{
    if (vFFDFrom.count() == vFFDTo.count())
    {
        if (iNumVerts < 2) iNumVerts = 2;

        createQuadPatch(iNumVerts, 0, true);

        // Move vertices
        for (int iVertexIndex = 0; iVertexIndex < m_vVertices.count(); iVertexIndex++)
        {
            CVector3 vNewPosition = m_vVertices[iVertexIndex].position();

            for (int iFFDIndex = 0; iFFDIndex < vFFDFrom.count(); iFFDIndex++)
            {
                CVector3 vOriginalPosition = vFFDFrom[iFFDIndex];
                CVector3 vTargetPosition = vFFDTo[iFFDIndex];
                CVector3 vMovement = vTargetPosition - vOriginalPosition;

                double dDistance = (m_vVertices[iVertexIndex].position() - vOriginalPosition).magnitude();
                double dWeight = pow(1.0 - Angles::clipDouble(dDistance, 0.0, 1.0), 2.0);

                vNewPosition += vMovement * dWeight;
            }

            m_vVertices[iVertexIndex].position() = vNewPosition;
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::subdivideQuads(int iNumIterations)
{
    for (int iIteration = 0; iIteration < iNumIterations; iIteration++)
    {
        QVector<CEdge> vEdges;

        for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
        {
            // Get vertex indices
            int iVA = m_vFaces[iFaceIndex].indices()[0];
            int iVB = m_vFaces[iFaceIndex].indices()[1];
            int iVC = m_vFaces[iFaceIndex].indices()[2];
            int iVD = m_vFaces[iFaceIndex].indices()[3];

            // Get vertex positions
            CVector3 pVA = m_vVertices[iVA].position();
            CVector3 pVB = m_vVertices[iVB].position();
            CVector3 pVC = m_vVertices[iVC].position();
            CVector3 pVD = m_vVertices[iVD].position();

            // Get face centroid
            // CVector3 vCenter = (pVA + pVB + pVC + pVD) / 4.0;

            // Subdivide the face
            if (!(vEdges.contains(CEdge(iVA, iVB)) || vEdges.contains(CEdge(iVB, iVA))))
            {
                vEdges.append(CEdge(iVA, iVB, m_vVertices.count()));
                m_vVertices.append(CVertex((pVA + pVB) / 2.0, CVector2( 0.0,  0.0)));
            }

            if (!(vEdges.contains(CEdge(iVB, iVC)) || vEdges.contains(CEdge(iVC, iVB))))
            {
                vEdges.append(CEdge(iVB, iVC, m_vVertices.count()));
                m_vVertices.append(CVertex((pVB + pVC) / 2.0, CVector2( 0.0,  0.0)));
            }

            if (!(vEdges.contains(CEdge(iVC, iVD)) || vEdges.contains(CEdge(iVD, iVC))))
            {
                vEdges.append(CEdge(iVC, iVD, m_vVertices.count()));
                m_vVertices.append(CVertex((pVC + pVD) / 2.0, CVector2( 0.0,  0.0)));
            }

            if (!(vEdges.contains(CEdge(iVD, iVA)) || vEdges.contains(CEdge(iVA, iVD))))
            {
                vEdges.append(CEdge(iVD, iVA, m_vVertices.count()));
                m_vVertices.append(CVertex((pVD + pVA) / 2.0, CVector2( 0.0,  0.0)));
            }
        }

        QVector<CFace> vNewFaces;

        for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
        {
            // Get vertex indices
            int iVA = m_vFaces[iFaceIndex].indices()[0];
            int iVB = m_vFaces[iFaceIndex].indices()[1];
            int iVC = m_vFaces[iFaceIndex].indices()[2];
            int iVD = m_vFaces[iFaceIndex].indices()[3];

            if (
                    vEdges.contains(CEdge(iVA, iVB)) && vEdges.contains(CEdge(iVB, iVC)) &&
                    vEdges.contains(CEdge(iVC, iVD)) && vEdges.contains(CEdge(iVD, iVA))
                    )
            {
                int iEAB = vEdges.indexOf(CEdge(iVA, iVB));
                int iEBC = vEdges.indexOf(CEdge(iVB, iVC));
                int iECD = vEdges.indexOf(CEdge(iVC, iVD));
                int iEDA = vEdges.indexOf(CEdge(iVD, iVA));

                // Get vertex positions
                CVector3 pVA = m_vVertices[iVA].position();
                CVector3 pVB = m_vVertices[iVB].position();
                CVector3 pVC = m_vVertices[iVC].position();
                CVector3 pVD = m_vVertices[iVD].position();

                int iVN = m_vVertices.count();

                m_vVertices.append(CVertex((pVA + pVB + pVC + pVD) / 4.0, CVector2( 0.0,  0.0)));

                // Subdivide
                vNewFaces.append(CFace(this, iVA, vEdges[iEAB].m_iNewVertex, iVN, vEdges[iEDA].m_iNewVertex));
                vNewFaces.append(CFace(this, vEdges[iEAB].m_iNewVertex, iVB, vEdges[iEBC].m_iNewVertex, iVN));
                vNewFaces.append(CFace(this, iVN, vEdges[iEBC].m_iNewVertex, iVC, vEdges[iECD].m_iNewVertex));
                vNewFaces.append(CFace(this, vEdges[iEDA].m_iNewVertex, iVN, vEdges[iECD].m_iNewVertex, iVD));

                m_vFaces.remove(iFaceIndex); iFaceIndex--;
            }
        }

        foreach (const CFace& face, vNewFaces)
        {
            m_vFaces.append(face);
        }
    }

    m_bGeometryDirty = true;
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::merge(const CMeshGeometry &other)
{
    int iVertexOffset = m_vVertices.count();

    foreach (const CVertex& vert, other.m_vVertices)
    {
        m_vVertices.append(vert);
    }

    foreach (const CFace& face, other.m_vFaces)
    {
        QVector<int> vNewIndices = face.indices();

        for (int iIndex = 0; iIndex < vNewIndices.count(); iIndex++)
        {
            vNewIndices[iIndex] += iVertexOffset;
        }

        m_vFaces.append(CFace(this, vNewIndices));
    }

    m_bGeometryDirty = true;
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::transformVertices(const CMatrix4& matrix)
{
    for (int iIndex = 0; iIndex < m_vVertices.count(); iIndex++)
    {
        m_vVertices[iIndex].position() = matrix * m_vVertices[iIndex].position();
    }

    m_bGeometryDirty = true;
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::translateUVs(CVector2 vTranslate)
{
    for (int iIndex = 0; iIndex < m_vVertices.count(); iIndex++)
    {
        m_vVertices[iIndex].texCoord() = m_vVertices[iIndex].texCoord() + vTranslate;
    }

    m_bGeometryDirty = true;
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::scaleUVs(CVector2 vScale)
{
    for (int iIndex = 0; iIndex < m_vVertices.count(); iIndex++)
    {
        m_vVertices[iIndex].texCoord() = m_vVertices[iIndex].texCoord() * vScale;
    }

    m_bGeometryDirty = true;
}

//-------------------------------------------------------------------------------------------------

RayTracingResult CMeshGeometry::intersect(CComponent* pContainer, CRay3 rGlobalRay)
{
    RayTracingResult dReturnResult(Q3D_INFINITY, pContainer);

    // Transform ray to local space
    CRay3 rLocalray = pContainer->worldTransformInverse() * rGlobalRay;

    if (m_bBounds.intersect(rLocalray).m_dDistance < Q3D_INFINITY)
    {
        if (m_bUseSpacePartitionning)
        {
            return intersectPartitions(pContainer, rLocalray);
        }
        else
        {
            for (int iFaceIndex = 0; iFaceIndex < m_vFaces.count(); iFaceIndex++)
            {
                if (m_vFaces[iFaceIndex].indices().count() > 2)
                {
                    for (int iVertIndex = 2; iVertIndex < m_vFaces[iFaceIndex].indices().count(); iVertIndex++)
                    {
                        int i1 = m_vFaces[iFaceIndex].indices()[0];
                        int i2 = m_vFaces[iFaceIndex].indices()[iVertIndex - 1];
                        int i3 = m_vFaces[iFaceIndex].indices()[iVertIndex];

                        CVector3 v1 = m_vVertices[i1].position();
                        CVector3 v2 = m_vVertices[i2].position();
                        CVector3 v3 = m_vVertices[i3].position();

                        RayTracingResult dNewResult = CFace::intersectTriangle(
                                    rLocalray,
                                    v1, v2, v3
                                    );

                        if (dNewResult.m_dDistance < dReturnResult.m_dDistance)
                        {
                            dReturnResult.m_dDistance = dNewResult.m_dDistance;
                            dReturnResult.m_vNormal = dNewResult.m_vNormal;
                        }
                    }
                }
            }
        }
    }

    return dReturnResult;
}

//-------------------------------------------------------------------------------------------------

Math::RayTracingResult CMeshGeometry::intersectPartitionData(CComponent* pContainer, const CBoundPartition<int>& partition, Math::CRay3 rLocalray)
{
    RayTracingResult dReturnResult(Q3D_INFINITY, pContainer);

    for (int iPartFaceIndex = 0; iPartFaceIndex < partition.data().count(); iPartFaceIndex++)
    {
        int iFaceIndex = partition.data()[iPartFaceIndex];

        if (iFaceIndex < m_vFaces.count() && m_vFaces[iFaceIndex].indices().count() > 2)
        {
            for (int iVertIndex = 2; iVertIndex < m_vFaces[iFaceIndex].indices().count(); iVertIndex++)
            {
                int i1 = m_vFaces[iFaceIndex].indices()[0];
                int i2 = m_vFaces[iFaceIndex].indices()[iVertIndex - 1];
                int i3 = m_vFaces[iFaceIndex].indices()[iVertIndex];

                CVector3 v1 = m_vVertices[i1].position();
                CVector3 v2 = m_vVertices[i2].position();
                CVector3 v3 = m_vVertices[i3].position();

                RayTracingResult dNewResult = CFace::intersectTriangle(
                            rLocalray,
                            v1, v2, v3
                            );

                if (dNewResult.m_dDistance < dReturnResult.m_dDistance)
                {
                    dReturnResult.m_dDistance = dNewResult.m_dDistance;
                    dReturnResult.m_vNormal = dNewResult.m_vNormal;
                }
            }
        }
    }

    return dReturnResult;
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::paint(CRenderContext* pContext, CComponent* pContainer)
{
    QMutexLocker locker(&m_mMutex);

    checkAndUpdateGeometry();

    if (m_vGLMeshData.count() > 0 && m_vGLMeshData.count() == m_vMaterials.count())
    {
        bool bFrustumCheck = false;

        if (pContainer != nullptr && pContext->scene()->frustumCheck())
        {
            CBoundingBox bWorldBounds = pContainer->worldBounds();
            CVector3 vPosition = pContext->internalCameraMatrix() * bWorldBounds.center();
            double dRadius = bWorldBounds.radius();

            pContext->tStatistics.m_iNumFrustumTests++;

            if (
                    pContext->camera()->contains(vPosition, dRadius) &&
                    vPosition.magnitude() < m_dMaxDistance
                    )
            {
                bFrustumCheck = true;
            }
        }
        else
        {
            bFrustumCheck = true;
        }

        if (bFrustumCheck)
        {
#ifdef DRAW_BY_MATERIAL
            pContext->addGeometry(pContainer, this);
#else

            for (int iIndex = 0; iIndex < m_vMaterials.count(); iIndex++)
            {
                CGLMeshData* pData = m_vGLMeshData[iIndex];
                CMaterial* pMaterial = m_vMaterials[iIndex].data();

                // Get a program from object material
                QGLShaderProgram* pProgram = pMaterial->activate(pContext);

                // If program ok...
                if (pProgram != nullptr)
                {
                    QMatrix4x4 mModelAbsolute;
                    mModelAbsolute.setToIdentity();

                    if (pContainer != nullptr)
                    {
                        // Set transform matrix
                        CVector3 WorldPosition = pContainer->worldPosition() - pContext->scene()->worldOrigin();
                        CVector3 WorldRotation = pContainer->worldRotation();
                        CVector3 WorldScale = pContainer->worldScale();

                        mModelAbsolute.translate(WorldPosition.X, WorldPosition.Y, WorldPosition.Z);
                        mModelAbsolute.rotate(Math::Angles::toDeg(WorldRotation.Y), QVector3D(0, 1, 0));
                        mModelAbsolute.rotate(Math::Angles::toDeg(WorldRotation.X), QVector3D(1, 0, 0));
                        mModelAbsolute.rotate(Math::Angles::toDeg(WorldRotation.Z), QVector3D(0, 0, 1));
                        mModelAbsolute.scale(WorldScale.X, WorldScale.Y, WorldScale.Z);
                    }

                    pData->paint(pContext, mModelAbsolute, pProgram, pData->m_iGLType);
                }
            }

#endif
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::dump(QTextStream& stream, int iIdent)
{
    dumpIdent(stream, iIdent, QString("[CMeshGeometry]"));
    dumpIdent(stream, iIdent, QString("Num vertices : %1").arg(m_vVertices.count()));
    dumpIdent(stream, iIdent, QString("Num faces : %1").arg(m_vFaces.count()));
    dumpIdent(stream, iIdent, QString("Max distance : %1").arg(m_dMaxDistance));
    dumpIdent(stream, iIdent, QString("GL type : %1").arg(m_iGLType));
    dumpIdent(stream, iIdent, QString("Use partitioning : %1").arg(m_bUseSpacePartitionning));
    dumpIdent(stream, iIdent, QString("Bounds min : %1").arg(m_bBounds.minimum().toString()));
    dumpIdent(stream, iIdent, QString("Bounds max : %1").arg(m_bBounds.maximum().toString()));

    /*
    dumpIdent(stream, iIdent, QString("Vertices :"));
    dumpOpenBlock(stream, iIdent); iIdent++;
    for (int iIndex = 0; iIndex < m_vVertices.count(); iIndex++)
    {
        dumpIdent(stream, iIdent, QString("Vertex %1 : %2").arg(iIndex).arg(m_vVertices[iIndex].position().toString()));
    }
    iIdent--; dumpCloseBlock(stream, iIdent);
    */
}
