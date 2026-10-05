add_executable(Quick3DTest
    main.cpp
    Quick3DTest.cpp
    Quick3DTest.h
    CUnitTests.cpp
    CUnitTests.h
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
)

set_target_properties(Quick3DTest PROPERTIES DEBUG_POSTFIX "d")
