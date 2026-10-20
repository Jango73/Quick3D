add_executable(Quick3DTest
    main.cpp
    Quick3DTest.cpp
    Quick3DTest.h
    CUiRemoteControlHandler.cpp
    CUiRemoteControlHandler.h
    tests/CTestFixture.cpp
    tests/CTestFixture.h
    tests/CTestMath.cpp
    tests/CTestMath.h
    tests/CTestCamera.cpp
    tests/CTestCamera.h
    tests/CTestGeo.cpp
    tests/CTestGeo.h
    tests/CTestPhysics.cpp
    tests/CTestPhysics.h
    tests/CTestVehicle.cpp
    tests/CTestVehicle.h
    tests/CTestScene.cpp
    tests/CTestScene.h
    tests/CTestQuick3D.cpp
    tests/CTestQuick3D.h
    Quick3DTest.ui
    ExportDialog.ui
    Quick3DTest.qrc
)

target_link_libraries(Quick3DTest PRIVATE
    Quick3D
    qt-plus
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::Xml
    Qt6::OpenGL
    Qt6::Test
)

set_target_properties(Quick3DTest PROPERTIES DEBUG_POSTFIX "d")

# CTestScene includes Jolt types directly for the rotation anchor test
target_include_directories(Quick3DTest PRIVATE
    "${CMAKE_SOURCE_DIR}/third/JoltPhysics"
)

# Runtime data (scenes, models, textures) resolves next to the binary
# (see CRessourcesManager); mirror quick3dtest/bin/ beside the executable
add_custom_command(TARGET Quick3DTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/bin
        $<TARGET_FILE_DIR:Quick3DTest>
)
