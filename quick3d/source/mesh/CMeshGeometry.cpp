
// qt-plus
#include "CLogger.h"

// Std
#include <algorithm>

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
    , m_dMaxDistance(dMaxDistance)
    , m_iGLType(GL_TRIANGLES)
    , m_iCoreVertexCount(-1)
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

/*!
    Records how many leading vertices are core grid data, the rest is patch extension. \br\br
    Ray queries skip faces touching extension vertices so skirts stay invisible to them.
*/
void CMeshGeometry::setCoreVertexCount(int iCount)
{
    m_iCoreVertexCount = iCount;
}

//-------------------------------------------------------------------------------------------------

void CMeshGeometry::clear()
{
    m_vVertices.clear();
    m_vFaces.clear();
    m_vVertexGroups.clear();

    m_iCoreVertexCount = -1;

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
            // Geometry shaders accept points, lines and triangles, never quads :
            // quad-based geometry is triangulated below and rendered as triangles
            int iRenderGLType = m_iGLType;

            if (iRenderGLType == GL_QUADS)
            {
                iRenderGLType = GL_TRIANGLES;
            }

            // Sort faces by material
            std::sort(m_vFaces.begin(), m_vFaces.end());

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
                    pData->m_iGLType = iRenderGLType;
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
                pGLMeshData->m_iGLType = iRenderGLType;

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

                        // Quad faces are triangulated as triangle fans, like other polygons
                        pGLMeshData->m_iNumRenderIndices = triangleCountForFaces(vFaceIndices) * 3;

                        if (pGLMeshData->m_iNumRenderPoints > 0 && pGLMeshData->m_iNumRenderIndices > 0)
                        {
                            /*
                            LOG_METHOD_DEBUG(QString("Allocating %1 vertices and %2 indices for %3")
                                .arg(pGLMeshData->m_iNumRenderPoints)
                                .arg(pGLMeshData->m_iNumRenderIndices)
                                .arg(m_sName)
                                );
                                */

                            // Creation of the OpenGL geometry buffers
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

                            // Fill OpenGL index buffer, triangulating quad faces as triangle fans
                            int iIndiceIndex = 0;

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
        // Reset of the polygon normals
        for (int Index = 0; Index < m_vVertices.count(); Index++)
        {
            m_vVertices[Index].normal() = CVector3(0.0, 0.0, 0.0);
            m_vVertices[Index].tangent() = CVector3(0.0, 0.0, 0.0);
            m_vVertices[Index].normalDivider() = 0.0;
        }

        // Iteration over all polygons
        for (int FaceIndex = 0; FaceIndex < m_vFaces.count(); FaceIndex++)
        {
            // Computation of the polygon normal vector
            m_vFaces[FaceIndex].computeNormal();

            bool bSkirtFace = faceTouchesExtension(m_vFaces[FaceIndex], m_iCoreVertexCount);

            // Processing of each polygon vertex
            foreach (int VertexIndex, m_vFaces[FaceIndex].indices())
            {
                // Extension faces never alter core normals, otherwise patch
                // borders darken from the skirt walls hanging below them
                if (bSkirtFace && VertexIndex < m_iCoreVertexCount) continue;

                m_vVertices[VertexIndex].normal() += m_vFaces[FaceIndex].normal();
                m_vVertices[VertexIndex].tangent() += m_vFaces[FaceIndex].tangent();
                m_vVertices[VertexIndex].normalDivider() += 1.0;
            }
        }

        // Division of the normals and tangents of each vertex
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

            bool bInFrustum = pContext->camera()->contains(vPosition, dRadius);
            bool bInRange = vPosition.magnitude() < m_dMaxDistance;

            if (bInFrustum)
            {
                if (bInRange)
                {
                    pContext->tStatistics.m_iNumFrustumPassed++;
                }
                else
                {
                    pContext->tStatistics.m_iNumFrustumOutOfRange++;
                }
            }

            if (bInFrustum && bInRange)
            {
                bFrustumCheck = true;
            }

            SMeshDrawInfo tDraw;
            tDraw.container = QString::number(quintptr(pContainer), 16);
            tDraw.name = pContainer->qualifiedName();
            tDraw.layer = pContext->camera()->currentPaintLayer();
            tDraw.distance = vPosition.magnitude();
            tDraw.centerX = vPosition.X;
            tDraw.centerY = vPosition.Y;
            tDraw.centerZ = vPosition.Z;
            tDraw.radius = dRadius;
            tDraw.maxDistance = m_dMaxDistance;
            tDraw.frustum = bInFrustum;
            tDraw.inRange = bInRange;
            tDraw.drawn = bFrustumCheck;
            tDraw.boxMinX = vPosition.X - dRadius;
            tDraw.boxMinY = vPosition.Y - dRadius;
            tDraw.boxMinZ = vPosition.Z - dRadius;
            tDraw.boxMaxX = vPosition.X + dRadius;
            tDraw.boxMaxY = vPosition.Y + dRadius;
            tDraw.boxMaxZ = vPosition.Z + dRadius;

            pContext->camera()->recordMeshDraw(tDraw);
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
                QOpenGLShaderProgram* pProgram = pMaterial->activate(pContext);

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
    dumpIndented(stream, iIdent, QString("[CMeshGeometry]"));
    dumpIndented(stream, iIdent, QString("Num vertices : %1").arg(m_vVertices.count()));
    dumpIndented(stream, iIdent, QString("Num faces : %1").arg(m_vFaces.count()));
    dumpIndented(stream, iIdent, QString("Max distance : %1").arg(m_dMaxDistance));
    dumpIndented(stream, iIdent, QString("GL type : %1").arg(m_iGLType));
    dumpIndented(stream, iIdent, QString("Use partitioning : %1").arg(m_bUseSpacePartitionning));
    dumpIndented(stream, iIdent, QString("Bounds min : %1").arg(m_bBounds.minimum().toString()));
    dumpIndented(stream, iIdent, QString("Bounds max : %1").arg(m_bBounds.maximum().toString()));
}
