#-------------------------------------------------
#
# Project created by QtCreator 2016-01-22T20:41:20
#
#-------------------------------------------------

QT += core gui network opengl xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Quick3D
TEMPLATE = lib

DEFINES += QUICK3D_LIB
DEFINES += SFML_SYSTEM_EXPORTS
DEFINES += SFML_WINDOW_EXPORTS

QMAKE_CXXFLAGS += -Wno-invalid-offsetof
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-reorder

INCLUDEPATH += ../COTS/SFML-1.6/include
INCLUDEPATH += ../COTS/unzip11
INCLUDEPATH += ../COTS/SFML-1.6/src

LIBS += -lopengl32 -luser32 -lgdi32 -lwinmm

SOURCES += \
    C3DScene.cpp \
    CAircraft.cpp \
    CAircraftController.cpp \
    CAnimator.cpp \
    CAnimatorFrame.cpp \
    CArmature.cpp \
    CAtmosphere.cpp \
    CAutoTerrain.cpp \
    CBasicAnimationStep.cpp \
    CBasicAnimator.cpp \
    CBasicAnimatorFrame.cpp \
    CBILData.cpp \
    CBILField.cpp \
    CBone.cpp \
    CBoundedMeshInstances.cpp \
    CBoundingBox.cpp \
    CBox.cpp \
    CBuildingGenerator.cpp \
    CCamera.cpp \
    CCartoController.cpp \
    CComponent.cpp \
    CComponentFactory.cpp \
    CComponentLoader.cpp \
    CCone.cpp \
    CConnectedStream.cpp \
    CConsoleBoard.cpp \
    CController.cpp \
    CDumpable.cpp \
    CElectricalBus.cpp \
    CElectricalComponent.cpp \
    CElectricalConsumer.cpp \
    CElectricalContactor.cpp \
    CElevator.cpp \
    CEngine.cpp \
    CFace.cpp \
    CFog.cpp \
    CForceController.cpp \
    CGeneratedField.cpp \
    CGenerateFunction.cpp \
    CGeoloc.cpp \
    CGLExtension.cpp \
    CGLWidgetScene.cpp \
    CHeightField.cpp \
    CHGTData.cpp \
    CHGTField.cpp \
    CHTTPMapClient.cpp \
    CHTTPServer.cpp \
    CHydraulicComponent.cpp \
    CImageFilter.cpp \
    CImageFilter_Matrix.cpp \
    CImageUtilities.cpp \
    CJetEngine.cpp \
    CJoystick.cpp \
    CLight.cpp \
    CLogManager.cpp \
    CMan.cpp \
    CMaterial.cpp \
    CMaterialInstance.cpp \
    CMesh.cpp \
    CMeshInstance.cpp \
    CMJPEGServer.cpp \
    CNamed.cpp \
    COBJLoader.cpp \
    CParented.cpp \
    CParticle.cpp \
    CParticleSystem.cpp \
    CPerlin.cpp \
    CPhysicalComponent.cpp \
    CPIDController.cpp \
    CPluginInterface.cpp \
    CPluginLoader.cpp \
    CPreferencesManager.cpp \
    CQuick3DUtilities.cpp \
    CRain.cpp \
    CRenderContext.cpp \
    CRessourcesManager.cpp \
    CRudder.cpp \
    CSeaVehicle.cpp \
    CServoAnimatorFrame.cpp \
    CServoPosition.cpp \
    CShaderCollection.cpp \
    CSkyBox.cpp \
    CSocketStream.cpp \
    CSphere.cpp \
    CSRTMData.cpp \
    CSRTMField.cpp \
    CStandardController.cpp \
    CTankController.cpp \
    CTerrain.cpp \
    CTerrestrialVehicle.cpp \
    CTexture.cpp \
    CTiledMaterial.cpp \
    CTimeSampler.cpp \
    CTracableMutex.cpp \
    CTrajectorable.cpp \
    CTrajectory.cpp \
    CTreeGenerator.cpp \
    CUtility.cpp \
    CVehicle.cpp \
    CVertex.cpp \
    CVertexGroup.cpp \
    CView.cpp \
    CViewport.cpp \
    CWater.cpp \
    CWaterMaterial.cpp \
    CWaypoint.cpp \
    CWing.cpp \
    CWorker.cpp \
    CWorkerManager.cpp \
    CWorldChunk.cpp \
    CXMLNode.cpp \
    CZip.cpp \
    GeoTools/coordcnv.cpp \
    GeoTools/geocent.cpp \
    GeoTools/geotrans.cpp \
    GeoTools/mgrs.cpp \
    GeoTools/polarst.cpp \
    GeoTools/tranmerc.cpp \
    GeoTools/ups.cpp \
    GeoTools/utm.cpp \
    GeoTools/UtmMgrs.cpp \
    ../COTS/zlib-1.2.6/adler32.c \
    ../COTS/zlib-1.2.6/compress.c \
    ../COTS/zlib-1.2.6/crc32.c \
    ../COTS/zlib-1.2.6/deflate.c \
    ../COTS/zlib-1.2.6/gzclose.c \
    ../COTS/zlib-1.2.6/gzlib.c \
    ../COTS/zlib-1.2.6/gzread.c \
    ../COTS/zlib-1.2.6/gzwrite.c \
    ../COTS/zlib-1.2.6/infback.c \
    ../COTS/zlib-1.2.6/inffast.c \
    ../COTS/zlib-1.2.6/inflate.c \
    ../COTS/zlib-1.2.6/inftrees.c \
    ../COTS/zlib-1.2.6/trees.c \
    ../COTS/zlib-1.2.6/uncompr.c \
    ../COTS/zlib-1.2.6/zutil.c \
    ../COTS/unzip11/ioapi.c \
    ../COTS/unzip11/iowin32.c \
    ../COTS/unzip11/mztools.c \
    ../COTS/unzip11/unzip.c \
    ../COTS/unzip11/zip.c \
    ../COTS/SFML-1.6/src/SFML/System/Clock.cpp \
    ../COTS/SFML-1.6/src/SFML/System/Err.cpp \
    ../COTS/SFML-1.6/src/SFML/System/Lock.cpp \
    ../COTS/SFML-1.6/src/SFML/System/Mutex.cpp \
    ../COTS/SFML-1.6/src/SFML/System/Sleep.cpp \
    ../COTS/SFML-1.6/src/SFML/System/String.cpp \
    ../COTS/SFML-1.6/src/SFML/System/Thread.cpp \
    ../COTS/SFML-1.6/src/SFML/System/ThreadLocal.cpp \
    ../COTS/SFML-1.6/src/SFML/System/Time.cpp \
    ../COTS/SFML-1.6/src/SFML/Window/Context.cpp \
    ../COTS/SFML-1.6/src/SFML/Window/GlContext.cpp \
    ../COTS/SFML-1.6/src/SFML/Window/GlResource.cpp \
    ../COTS/SFML-1.6/src/SFML/Window/Joystick.cpp \
    ../COTS/SFML-1.6/src/SFML/Window/JoystickManager.cpp \
    ../COTS/SFML-1.6/src/SFML/Window/Keyboard.cpp \
    ../COTS/SFML-1.6/src/SFML/Window/Mouse.cpp \
    ../COTS/SFML-1.6/src/SFML/Window/VideoMode.cpp \
    ../COTS/SFML-1.6/src/SFML/Window/Window.cpp \
    ../COTS/SFML-1.6/src/SFML/Window/WindowImpl.cpp \
    ../COTS/SFML-1.6/src/SFML/Window/Win32/InputImpl.cpp \
    ../COTS/SFML-1.6/src/SFML/Window/Win32/JoystickImpl.cpp \
    ../COTS/SFML-1.6/src/SFML/Window/Win32/VideoModeImpl.cpp \
    ../COTS/SFML-1.6/src/SFML/Window/Win32/WglContext.cpp \
    ../COTS/SFML-1.6/src/SFML/Window/Win32/WindowImplWin32.cpp \
    ../COTS/SFML-1.6/src/SFML/System/Win32/ClockImpl.cpp \
    ../COTS/SFML-1.6/src/SFML/System/Win32/MutexImpl.cpp \
    ../COTS/SFML-1.6/src/SFML/System/Win32/SleepImpl.cpp \
    ../COTS/SFML-1.6/src/SFML/System/Win32/ThreadImpl.cpp \
    ../COTS/SFML-1.6/src/SFML/System/Win32/ThreadLocalImpl.cpp \
    CQ3DLoader.cpp

HEADERS  += \
    Angles.h \
    C3DScene.h \
    CAircraft.h \
    CAircraftController.h \
    CAnimator.h \
    CAnimatorFrame.h \
    CArmature.h \
    CAtmosphere.h \
    CAutoTerrain.h \
    CAverager.h \
    CBasicAnimationStep.h \
    CBasicAnimator.h \
    CBasicAnimatorFrame.h \
    CBILData.h \
    CBILField.h \
    CBone.h \
    CBoundedMeshInstances.h \
    CBoundingBox.h \
    CBox.h \
    CBuildingGenerator.h \
    CCamera.h \
    CCartoController.h \
    CComponent.h \
    CComponentFactory.h \
    CComponentLoader.h \
    CComponentReference.h \
    CCone.h \
    CConnectedStream.h \
    CConsoleBoard.h \
    CController.h \
    CDumpable.h \
    CElectricalBus.h \
    CElectricalComponent.h \
    CElectricalConsumer.h \
    CElectricalContactor.h \
    CElevator.h \
    CEngine.h \
    CFace.h \
    CFog.h \
    CForceController.h \
    CGeneratedField.h \
    CGenerateFunction.h \
    CGeoloc.h \
    CGeoZone.h \
    CGLExtension.h \
    CGLWidgetScene.h \
    CHeightField.h \
    CHGTData.h \
    CHGTField.h \
    CHTTPMapClient.h \
    CHTTPServer.h \
    CHydraulicComponent.h \
    CImageFilter.h \
    CImageFilter_Matrix.h \
    CImageUtilities.h \
    CJetEngine.h \
    CJoystick.h \
    CLight.h \
    CLogManager.h \
    CMan.h \
    CMaterial.h \
    CMaterialInstance.h \
    CMesh.h \
    CMeshInstance.h \
    CMJPEGServer.h \
    CNamed.h \
    COBJLoader.h \
    ComponentInstanciator.h \
    CParented.h \
    CParticle.h \
    CParticleSystem.h \
    CPerlin.h \
    CPhysicalComponent.h \
    CPIDController.h \
    CPluginInterface.h \
    CPluginLoader.h \
    CPreferencesManager.h \
    CQ3DConstants.h \
    CQuick3DUtilities.h \
    CRain.h \
    CRenderContext.h \
    CRessourcesManager.h \
    CRudder.h \
    CSeaVehicle.h \
    CServoAnimatorFrame.h \
    CServoPosition.h \
    CShaderCollection.h \
    CSingleton.h \
    CSkyBox.h \
    CSocketStream.h \
    CSphere.h \
    CSRTMData.h \
    CSRTMField.h \
    CStandardController.h \
    CTankController.h \
    CTerrain.h \
    CTerrestrialVehicle.h \
    CTexture.h \
    CTiledMaterial.h \
    CTimeSampler.h \
    CTracableMutex.h \
    CTrajectorable.h \
    CTrajectory.h \
    CTreeGenerator.h \
    CUtility.h \
    CVehicle.h \
    CVertex.h \
    CVertexGroup.h \
    CView.h \
    CViewport.h \
    CWater.h \
    CWaterMaterial.h \
    CWaypoint.h \
    CWing.h \
    CWorker.h \
    CWorkerManager.h \
    CWorldChunk.h \
    CXMLNode.h \
    CZip.h \
    glext.h \
    IController.h \
    ILogger.h \
    Interpolator.h \
    IProgressListener.h \
    quick3d_global.h \
    CVector3.h \
    CVector2.h \
    CVector4.h \
    CRay3.h \
    CPlane3.h \
    CQuaternion.h \
    CMatrix4.h \
    CAxis.h \
    GeoTools/coordcnv.h \
    GeoTools/geocent.h \
    GeoTools/geotrans.h \
    GeoTools/mgrs.h \
    GeoTools/polarst.h \
    GeoTools/tranmerc.h \
    GeoTools/ups.h \
    GeoTools/utm.h \
    GeoTools/UtmMgrs.h \
    ../COTS/zlib-1.2.6/crc32.h \
    ../COTS/zlib-1.2.6/deflate.h \
    ../COTS/zlib-1.2.6/gzguts.h \
    ../COTS/zlib-1.2.6/inffast.h \
    ../COTS/zlib-1.2.6/inffixed.h \
    ../COTS/zlib-1.2.6/inflate.h \
    ../COTS/zlib-1.2.6/inftrees.h \
    ../COTS/zlib-1.2.6/trees.h \
    ../COTS/zlib-1.2.6/zconf.h \
    ../COTS/zlib-1.2.6/zlib.h \
    ../COTS/zlib-1.2.6/zutil.h \
    ../COTS/unzip11/crypt.h \
    ../COTS/unzip11/ioapi.h \
    ../COTS/unzip11/iowin32.h \
    ../COTS/unzip11/mztools.h \
    ../COTS/unzip11/unzip.h \
    ../COTS/unzip11/zip.h \
    ../COTS/SFML-1.6/src/SFML/Window/GlContext.hpp \
    ../COTS/SFML-1.6/src/SFML/Window/InputImpl.hpp \
    ../COTS/SFML-1.6/src/SFML/Window/JoystickImpl.hpp \
    ../COTS/SFML-1.6/src/SFML/Window/JoystickManager.hpp \
    ../COTS/SFML-1.6/src/SFML/Window/VideoModeImpl.hpp \
    ../COTS/SFML-1.6/src/SFML/Window/WindowImpl.hpp \
    ../COTS/SFML-1.6/src/SFML/Window/Win32/InputImpl.hpp \
    ../COTS/SFML-1.6/src/SFML/Window/Win32/JoystickImpl.hpp \
    ../COTS/SFML-1.6/src/SFML/Window/Win32/WglContext.hpp \
    ../COTS/SFML-1.6/src/SFML/Window/Win32/WindowImplWin32.hpp \
    ../COTS/SFML-1.6/src/SFML/System/Win32/ClockImpl.hpp \
    ../COTS/SFML-1.6/src/SFML/System/Win32/MutexImpl.hpp \
    ../COTS/SFML-1.6/src/SFML/System/Win32/SleepImpl.hpp \
    ../COTS/SFML-1.6/src/SFML/System/Win32/ThreadImpl.hpp \
    ../COTS/SFML-1.6/src/SFML/System/Win32/ThreadLocalImpl.hpp \
    CQ3DLoader.h

DISTFILES +=

RESOURCES += \
    Quick3DBase.qrc

# Generate help
helpfile = Quick3D.qdocconf

QMAKE_POST_LINK += qdoc $$quote($$shell_path($$helpfile)) $$escape_expand(\\n\\t)
