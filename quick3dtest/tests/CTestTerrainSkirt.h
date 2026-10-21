
#pragma once

// Application
#include "CTestFixture.h"

//-------------------------------------------------------------------------------------------------

class CTestTerrainSkirt : public CTestFixture
{
    Q_OBJECT

public:

    explicit CTestTerrainSkirt();

private slots:

    //! Border loop of a 5 by 5 grid holds 16 unique ordered indices
    void testBorderLoopIsClosed();

    //! Skirt depth follows cell size and relief, clamped to minimum and maximum
    void testSkirtDepthClamps();

    //! Disabled parameters append no vertex and no face
    void testDisabledAddsNothing();

    //! Enabled build appends one tucked ring and records the core count
    void testBuildSkirtAppendsTuckedRing();

    //! Skirt faces reference extension vertices so ray queries skip them
    void testSkirtFacesReferenceExtensionOnly();

    //! Core border normals are unchanged by the skirt, skirt walls face outward
    void testSkirtLeavesCoreNormalsUntouched();
};

//-------------------------------------------------------------------------------------------------
