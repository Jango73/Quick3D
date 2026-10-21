
// qt-plus
#include "CLogger.h"

// Application
#include "CPatchSkirtBuilder.h"
#include "CMeshGeometry.h"
#include "CVertex.h"
#include "CFace.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

/*!
    Builds default parameters.
*/
SPatchSkirtParameters::SPatchSkirtParameters()
    : Enabled(true)
    , DepthFactor(SKIRT_DEPTH_FACTOR)
    , ReliefFactor(SKIRT_RELIEF_FACTOR)
    , InsetFactor(SKIRT_INSET_FACTOR)
    , MinimumDepthMeters(SKIRT_MINIMUM_DEPTH_M)
    , MaximumDepthMeters(SKIRT_MAXIMUM_DEPTH_M)
{
}

/************************************************************************/

/*!
    Appends a skirt to \a pGeometry built as an \a iGridPoints by \a iGridPoints grid. \br\br
    The skirt duplicates the border vertices, drops them along their gravity vector by an
    adaptive depth and tucks them toward the patch center so the wall slides under the
    neighboring patch instead of standing exactly vertical. Faces are emitted as quads
    with outward winding. The caller must guarantee exclusive access to the mesh.
*/
SPatchSkirtRange CPatchSkirtBuilder::buildSkirt(CMeshGeometry* pGeometry, int iGridPoints, const SPatchSkirtParameters& rParameters)
{
    SPatchSkirtRange oRange;

    if (pGeometry == nullptr)
    {
        LOG_WARNING("CPatchSkirtBuilder::buildSkirt() : Null geometry");
        return oRange;
    }

    if (rParameters.Enabled == false)
    {
        return oRange;
    }

    if (iGridPoints < 2)
    {
        LOG_WARNING("CPatchSkirtBuilder::buildSkirt() : Grid too small");
        return oRange;
    }

    QVector<CVertex>& vVertices = pGeometry->vertices();
    QVector<CFace>& vFaces = pGeometry->faces();

    int iCoreVertexCount = iGridPoints * iGridPoints;

    if (vVertices.count() < iCoreVertexCount)
    {
        LOG_WARNING("CPatchSkirtBuilder::buildSkirt() : Grid vertices missing");
        return oRange;
    }

    oRange.CoreVertexCount = iCoreVertexCount;

    QVector<int> vLoop = borderLoop(iGridPoints);

    if (vLoop.count() < 4)
    {
        LOG_WARNING("CPatchSkirtBuilder::buildSkirt() : Empty border loop");
        return oRange;
    }

    // Measure the patch extent and the border relief for the adaptive depth
    CVector3 vMinimum = vVertices[vLoop[0]].position();
    CVector3 vMaximum = vVertices[vLoop[0]].position();
    double dMinimumAltitude = vVertices[vLoop[0]].altitude();
    double dMaximumAltitude = vVertices[vLoop[0]].altitude();
    CVector3 vCenter(0.0, 0.0, 0.0);

    foreach (int iIndex, vLoop)
    {
        CVector3 vPosition = vVertices[iIndex].position();

        vCenter += vPosition;

        if (vPosition.X < vMinimum.X) vMinimum.X = vPosition.X;
        if (vPosition.Y < vMinimum.Y) vMinimum.Y = vPosition.Y;
        if (vPosition.Z < vMinimum.Z) vMinimum.Z = vPosition.Z;
        if (vPosition.X > vMaximum.X) vMaximum.X = vPosition.X;
        if (vPosition.Y > vMaximum.Y) vMaximum.Y = vPosition.Y;
        if (vPosition.Z > vMaximum.Z) vMaximum.Z = vPosition.Z;

        double dAltitude = vVertices[iIndex].altitude();

        if (dAltitude < dMinimumAltitude) dMinimumAltitude = dAltitude;
        if (dAltitude > dMaximumAltitude) dMaximumAltitude = dAltitude;
    }

    vCenter = vCenter / (double) vLoop.count();

    CVector3 vExtent = vMaximum - vMinimum;
    double dPatchSizeMeters = vExtent.X;

    if (vExtent.Y > dPatchSizeMeters) dPatchSizeMeters = vExtent.Y;
    if (vExtent.Z > dPatchSizeMeters) dPatchSizeMeters = vExtent.Z;

    double dCellSizeMeters = dPatchSizeMeters / (double) (iGridPoints - 1);
    double dReliefMeters = dMaximumAltitude - dMinimumAltitude;

    if (dReliefMeters < 0.0) dReliefMeters = 0.0;

    double dDepthMeters = skirtDepth(dCellSizeMeters, dReliefMeters, rParameters);
    double dInsetMeters = rParameters.InsetFactor * dDepthMeters;

    if (dInsetMeters < 0.0) dInsetMeters = 0.0;

    LOG_DEBUG(QString("CPatchSkirtBuilder::buildSkirt() : points %1, depth %2, inset %3")
        .arg(iGridPoints)
        .arg(dDepthMeters)
        .arg(dInsetMeters)
        );

    // Duplicate the border ring, dropped down and tucked toward the center
    oRange.SkirtFirstVertex = vVertices.count();

    QVector<int> vSkirtIndices;

    foreach (int iIndex, vLoop)
    {
        CVertex oSkirt = vVertices[iIndex];

        CVector3 vUp = oSkirt.gravity() * -1.0;

        if (vUp.magnitude() < MESH_NULL_VECTOR_MAGNITUDE)
        {
            vUp = oSkirt.normal();
        }

        vUp = vUp.normalized();

        CVector3 vToCenter = vCenter - oSkirt.position();
        CVector3 vInward = vToCenter - vUp * vToCenter.dot(vUp);

        if (vInward.magnitude() > MESH_NULL_VECTOR_MAGNITUDE)
        {
            vInward = vInward.normalized();
        }
        else
        {
            vInward = CVector3(0.0, 0.0, 0.0);
        }

        oSkirt.position() = oSkirt.position() - vUp * dDepthMeters + vInward * dInsetMeters;
        oSkirt.altitude() = oSkirt.altitude() - dDepthMeters;

        vSkirtIndices.append(vVertices.count());
        vVertices.append(oSkirt);
    }

    oRange.SkirtVertexCount = vSkirtIndices.count();

    // Emit one outward-facing quad per border segment
    oRange.SkirtFirstFace = vFaces.count();

    for (int iSegment = 0; iSegment < vLoop.count(); iSegment++)
    {
        int iNext = (iSegment + 1) % vLoop.count();

        int iTopA = vLoop[iSegment];
        int iTopB = vLoop[iNext];
        int iBottomB = vSkirtIndices[iNext];
        int iBottomA = vSkirtIndices[iSegment];

        vFaces.append(CFace(pGeometry, iTopA, iTopB, iBottomB, iBottomA));
    }

    oRange.SkirtFaceCount = vFaces.count() - oRange.SkirtFirstFace;

    pGeometry->setCoreVertexCount(oRange.CoreVertexCount);

    return oRange;
}

/************************************************************************/

/*!
    Computes the extrusion depth in meters from \a dCellSizeMeters and \a dReliefMeters. \br\br
    Depth is \c DepthFactor * cell size plus \c ReliefFactor * border relief, clamped to
    the configured minimum and maximum.
*/
double CPatchSkirtBuilder::skirtDepth(double dCellSizeMeters, double dReliefMeters, const SPatchSkirtParameters& rParameters)
{
    if (dCellSizeMeters < 0.0) dCellSizeMeters = 0.0;
    if (dReliefMeters < 0.0) dReliefMeters = 0.0;

    double dDepth = rParameters.DepthFactor * dCellSizeMeters + rParameters.ReliefFactor * dReliefMeters;

    if (dDepth < rParameters.MinimumDepthMeters) dDepth = rParameters.MinimumDepthMeters;
    if (dDepth > rParameters.MaximumDepthMeters) dDepth = rParameters.MaximumDepthMeters;

    return dDepth;
}

/************************************************************************/

/*!
    Collects the border vertex indices of an \a iGridPoints by \a iGridPoints grid in
    closed loop order, suitable for skirt wall emission.
*/
QVector<int> CPatchSkirtBuilder::borderLoop(int iGridPoints)
{
    QVector<int> vLoop;

    if (iGridPoints < 2)
    {
        return vLoop;
    }

    int iLast = iGridPoints - 1;

    // North edge, eastward
    for (int iX = 0; iX < iLast; iX++)
    {
        vLoop.append(iLast * iGridPoints + iX);
    }

    // East edge, southward
    for (int iZ = iLast; iZ > 0; iZ--)
    {
        vLoop.append(iZ * iGridPoints + iLast);
    }

    // South edge, westward
    for (int iX = iLast; iX > 0; iX--)
    {
        vLoop.append(iX);
    }

    // West edge, northward
    for (int iZ = 0; iZ < iLast; iZ++)
    {
        vLoop.append(iZ * iGridPoints);
    }

    return vLoop;
}
