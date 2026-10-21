
#pragma once

// Qt
#include <QVector>

// Application
#include "quick3d_global.h"
#include "CVector3.h"
#include "CMeshConstants.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations
//-------------------------------------------------------------------------------------------------

class CMeshGeometry;

//-------------------------------------------------------------------------------------------------

//! Parameters controlling patch skirt generation
struct QUICK3D_EXPORT SPatchSkirtParameters
{
    //! Builds default parameters
    SPatchSkirtParameters();

    bool Enabled;               //!< When false no skirt vertex is added
    double DepthFactor;         //!< Multiplier applied to the mean cell size
    double ReliefFactor;        //!< Multiplier applied to the border altitude relief
    double InsetFactor;         //!< Horizontal tuck under the neighbor, as a fraction of depth
    double MinimumDepthMeters;  //!< Lower clamp of the extrusion depth
    double MaximumDepthMeters;  //!< Upper clamp of the extrusion depth
};

//-------------------------------------------------------------------------------------------------

//! Describes which mesh vertices belong to the added skirt
struct QUICK3D_EXPORT SPatchSkirtRange
{
    //! Builds an empty range
    SPatchSkirtRange()
        : CoreVertexCount(0)
        , SkirtFirstVertex(-1)
        , SkirtVertexCount(0)
        , SkirtFirstFace(-1)
        , SkirtFaceCount(0)
    {
    }

    //! Returns true when a skirt was appended
    bool hasSkirt() const { return SkirtVertexCount > 0 && SkirtFaceCount > 0; }

    //! Returns true when the vertex index belongs to the skirt
    bool isSkirtVertex(int iVertexIndex) const
    {
        return hasSkirt() && iVertexIndex >= SkirtFirstVertex;
    }

    int CoreVertexCount;    //!< Vertex count before the skirt was appended
    int SkirtFirstVertex;   //!< Index of the first skirt vertex, -1 when none
    int SkirtVertexCount;   //!< Number of appended skirt vertices
    int SkirtFirstFace;     //!< Index of the first skirt face, -1 when none
    int SkirtFaceCount;     //!< Number of appended skirt faces
};

//-------------------------------------------------------------------------------------------------

/*!
    \class CPatchSkirtBuilder
    \brief Appends a tucked skirt ring to a grid patch mesh.
    \inmodule Quick3D
    \sa CMeshGeometry
*/
class QUICK3D_EXPORT CPatchSkirtBuilder
{
public:

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    /*!
        Appends a skirt to \a pGeometry built as an \a iGridPoints by \a iGridPoints grid. \br\br
        The skirt duplicates the border vertices, drops them along their gravity vector by an
        adaptive depth and tucks them toward the patch center so the wall slides under the
        neighboring patch instead of standing exactly vertical. Faces are emitted as quads
        with outward winding. The geometry records the core vertex count so ray queries
        skip skirt faces. The caller must guarantee exclusive access to the mesh.
    */
    static SPatchSkirtRange buildSkirt(CMeshGeometry* pGeometry, int iGridPoints, const SPatchSkirtParameters& rParameters);

    /*!
        Computes the extrusion depth in meters from \a dCellSizeMeters and \a dReliefMeters. \br\br
        Depth is \c DepthFactor * cell size plus \c ReliefFactor * border relief, clamped to
        the configured minimum and maximum.
    */
    static double skirtDepth(double dCellSizeMeters, double dReliefMeters, const SPatchSkirtParameters& rParameters);

    /*!
        Collects the border vertex indices of an \a iGridPoints by \a iGridPoints grid in
        closed loop order, suitable for skirt wall emission.
    */
    static QVector<int> borderLoop(int iGridPoints);
};
