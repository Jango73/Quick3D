
HEADERS += \
    source/animation/*.h \
    source/base/*.h \
    source/components/*.h \
    source/input/*.h \
    source/math/*.h \
    source/mesh/*.h \
    source/render/*.h \
    source/terrain/*.h \
    source/utils/*.h

SOURCES += \
    source/animation/*.cpp \
    source/base/*.cpp \
    source/components/*.cpp \
    source/input/*.cpp \
    source/math/*.cpp \
    source/mesh/*.cpp \
    source/render/*.cpp \
    source/terrain/*.cpp \
    source/utils/*.cpp

RESOURCES += \
    Quick3DShaders.qrc

win32 {
    HEADERS += \
        source/zip/*.h \
        ../third/zlib/crc32.h \
        ../third/zlib/deflate.h \
        ../third/zlib/gzguts.h \
        ../third/zlib/inffast.h \
        ../third/zlib/inffixed.h \
        ../third/zlib/inflate.h \
        ../third/zlib/inftrees.h \
        ../third/zlib/trees.h \
        ../third/zlib/zconf.h \
        ../third/zlib/zlib.h \
        ../third/zlib/zutil.h \
        ../third/unzip11/crypt.h \
        ../third/unzip11/ioapi.h \
        ../third/unzip11/iowin32.h \
        ../third/unzip11/mztools.h \
        ../third/unzip11/unzip.h \
        ../third/unzip11/zip.h \
        ../third/SFML-1.6/src/SFML/Window/GlContext.hpp \
        ../third/SFML-1.6/src/SFML/Window/InputImpl.hpp \
        ../third/SFML-1.6/src/SFML/Window/JoystickImpl.hpp \
        ../third/SFML-1.6/src/SFML/Window/JoystickManager.hpp \
        ../third/SFML-1.6/src/SFML/Window/VideoModeImpl.hpp \
        ../third/SFML-1.6/src/SFML/Window/WindowImpl.hpp \
        ../third/SFML-1.6/src/SFML/Window/Win32/InputImpl.hpp \
        ../third/SFML-1.6/src/SFML/Window/Win32/JoystickImpl.hpp \
        ../third/SFML-1.6/src/SFML/Window/Win32/WglContext.hpp \
        ../third/SFML-1.6/src/SFML/Window/Win32/WindowImplWin32.hpp \
        ../third/SFML-1.6/src/SFML/System/Win32/ClockImpl.hpp \
        ../third/SFML-1.6/src/SFML/System/Win32/MutexImpl.hpp \
        ../third/SFML-1.6/src/SFML/System/Win32/SleepImpl.hpp \
        ../third/SFML-1.6/src/SFML/System/Win32/ThreadImpl.hpp \
        ../third/SFML-1.6/src/SFML/System/Win32/ThreadLocalImpl.hpp

    SOURCES += \
        source/zip/*.cpp \
        ../third/zlib/adler32.c \
        ../third/zlib/compress.c \
        ../third/zlib/crc32.c \
        ../third/zlib/deflate.c \
        ../third/zlib/gzclose.c \
        ../third/zlib/gzlib.c \
        ../third/zlib/gzread.c \
        ../third/zlib/gzwrite.c \
        ../third/zlib/infback.c \
        ../third/zlib/inffast.c \
        ../third/zlib/inflate.c \
        ../third/zlib/inftrees.c \
        ../third/zlib/trees.c \
        ../third/zlib/uncompr.c \
        ../third/zlib/zutil.c \
        ../third/unzip11/ioapi.c \
        ../third/unzip11/iowin32.c \
        ../third/unzip11/mztools.c \
        ../third/unzip11/unzip.c \
        ../third/unzip11/zip.c \
        ../third/SFML-1.6/src/SFML/System/Clock.cpp \
        ../third/SFML-1.6/src/SFML/System/Err.cpp \
        ../third/SFML-1.6/src/SFML/System/Lock.cpp \
        ../third/SFML-1.6/src/SFML/System/Mutex.cpp \
        ../third/SFML-1.6/src/SFML/System/Sleep.cpp \
        ../third/SFML-1.6/src/SFML/System/String.cpp \
        ../third/SFML-1.6/src/SFML/System/Thread.cpp \
        ../third/SFML-1.6/src/SFML/System/ThreadLocal.cpp \
        ../third/SFML-1.6/src/SFML/System/Time.cpp \
        ../third/SFML-1.6/src/SFML/Window/Context.cpp \
        ../third/SFML-1.6/src/SFML/Window/GlContext.cpp \
        ../third/SFML-1.6/src/SFML/Window/GlResource.cpp \
        ../third/SFML-1.6/src/SFML/Window/Joystick.cpp \
        ../third/SFML-1.6/src/SFML/Window/JoystickManager.cpp \
        ../third/SFML-1.6/src/SFML/Window/Keyboard.cpp \
        ../third/SFML-1.6/src/SFML/Window/Mouse.cpp \
        ../third/SFML-1.6/src/SFML/Window/VideoMode.cpp \
        ../third/SFML-1.6/src/SFML/Window/Window.cpp \
        ../third/SFML-1.6/src/SFML/Window/WindowImpl.cpp \
        ../third/SFML-1.6/src/SFML/Window/Win32/InputImpl.cpp \
        ../third/SFML-1.6/src/SFML/Window/Win32/JoystickImpl.cpp \
        ../third/SFML-1.6/src/SFML/Window/Win32/VideoModeImpl.cpp \
        ../third/SFML-1.6/src/SFML/Window/Win32/WglContext.cpp \
        ../third/SFML-1.6/src/SFML/Window/Win32/WindowImplWin32.cpp \
        ../third/SFML-1.6/src/SFML/System/Win32/ClockImpl.cpp \
        ../third/SFML-1.6/src/SFML/System/Win32/MutexImpl.cpp \
        ../third/SFML-1.6/src/SFML/System/Win32/SleepImpl.cpp \
        ../third/SFML-1.6/src/SFML/System/Win32/ThreadImpl.cpp \
        ../third/SFML-1.6/src/SFML/System/Win32/ThreadLocalImpl.cpp
}
