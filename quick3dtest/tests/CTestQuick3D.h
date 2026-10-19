
#pragma once

// Qt
#include <QtTest/QtTest>

//-------------------------------------------------------------------------------------------------

//! Test orchestrator: runs every test suite sequentially, main stays simple.
class CTestQuick3D : public QObject
{
    Q_OBJECT

public:

    explicit CTestQuick3D();

    //! Runs every test suite sequentially, returns the combined status.
    int runAllTests();
};

//-------------------------------------------------------------------------------------------------
