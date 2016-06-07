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
INCLUDEPATH += ../COTS/SFML-1.6/include
INCLUDEPATH += ../COTS/unzip11
INCLUDEPATH += ../COTS/SFML-1.6/src
LIBS += -lopengl32 -luser32 -lgdi32 -lwinmm

# C++ Flags
QMAKE_CXXFLAGS += -Wno-invalid-offsetof
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-reorder

# Help
helpfile = Quick3D.qdocconf

QMAKE_POST_LINK += qdoc $$quote($$shell_path($$helpfile)) $$escape_expand(\\n\\t)

# Code
SOURCES += \
    Source/C3DScene.cpp \
    Source/CAircraft.cpp \
    Source/CAircraftController.cpp \
    Source/CAnimator.cpp \
    Source/CAnimatorFrame.cpp \
    Source/CArmature.cpp \
    Source/CAtmosphere.cpp \
    Source/CAutoTerrain.cpp \
    Source/CBasicAnimationStep.cpp \
    Source/CBasicAnimator.cpp \
    Source/CBasicAnimatorFrame.cpp \
    Source/CBILData.cpp \
    Source/CBILField.cpp \
    Source/CBone.cpp \
    Source/CBoundedMeshInstances.cpp \
    Source/CBoundingBox.cpp \
    Source/CBox.cpp \
    Source/CBuildingGenerator.cpp \
    Source/CCamera.cpp \
    Source/CCartoController.cpp \
    Source/CComponent.cpp \
    Source/CComponentFactory.cpp \
    Source/CComponentLoader.cpp \
    Source/CCone.cpp \
    Source/CConnectedStream.cpp \
    Source/CConsoleBoard.cpp \
    Source/CController.cpp \
    Source/CDumpable.cpp \
    Source/CElectricalBus.cpp \
    Source/CElectricalComponent.cpp \
    Source/CElectricalConsumer.cpp \
    Source/CElectricalContactor.cpp \
    Source/CElevator.cpp \
    Source/CEngine.cpp \
    Source/CFace.cpp \
    Source/CFog.cpp \
    Source/CForceController.cpp \
    Source/CGeneratedField.cpp \
    Source/CGenerateFunction.cpp \
    Source/CGeoloc.cpp \
    Source/CGLExtension.cpp \
    Source/CGLWidgetScene.cpp \
    Source/CHeightField.cpp \
    Source/CHGTData.cpp \
    Source/CHGTField.cpp \
    Source/CHTTPMapClient.cpp \
    Source/CHTTPServer.cpp \
    Source/CHydraulicComponent.cpp \
    Source/CImageFilter.cpp \
    Source/CImageFilter_Matrix.cpp \
    Source/CImageUtilities.cpp \
    Source/CJetEngine.cpp \
    Source/CJoystick.cpp \
    Source/CLight.cpp \
    Source/CLogManager.cpp \
    Source/CMan.cpp \
    Source/CMaterial.cpp \
    Source/CMaterialInstance.cpp \
    Source/CMesh.cpp \
    Source/CMeshInstance.cpp \
    Source/CMJPEGServer.cpp \
    Source/CNamed.cpp \
    Source/COBJLoader.cpp \
    Source/CParented.cpp \
    Source/CParticle.cpp \
    Source/CParticleSystem.cpp \
    Source/CPerlin.cpp \
    Source/CPhysicalComponent.cpp \
    Source/CPIDController.cpp \
    Source/CPluginInterface.cpp \
    Source/CPluginLoader.cpp \
    Source/CPreferencesManager.cpp \
    Source/CQ3DLoader.cpp \
    Source/CQuick3DUtilities.cpp \
    Source/CRain.cpp \
    Source/CRenderContext.cpp \
    Source/CRessourcesManager.cpp \
    Source/CRudder.cpp \
    Source/CSeaVehicle.cpp \
    Source/CServoAnimatorFrame.cpp \
    Source/CServoPosition.cpp \
    Source/CShaderCollection.cpp \
    Source/CSkyBox.cpp \
    Source/CSocketStream.cpp \
    Source/CSphere.cpp \
    Source/CSRTMData.cpp \
    Source/CSRTMField.cpp \
    Source/CStandardController.cpp \
    Source/CTankController.cpp \
    Source/CTerrain.cpp \
    Source/CTerrestrialVehicle.cpp \
    Source/CTexture.cpp \
    Source/CTiledMaterial.cpp \
    Source/CTimeSampler.cpp \
    Source/CTracableMutex.cpp \
    Source/CTrajectorable.cpp \
    Source/CTrajectory.cpp \
    Source/CTreeGenerator.cpp \
    Source/CUtility.cpp \
    Source/CVehicle.cpp \
    Source/CVertex.cpp \
    Source/CVertexGroup.cpp \
    Source/CView.cpp \
    Source/CViewport.cpp \
    Source/CWater.cpp \
    Source/CWaterMaterial.cpp \
    Source/CWaypoint.cpp \
    Source/CWheel.cpp \
    Source/CWing.cpp \
    Source/CWorker.cpp \
    Source/CWorkerManager.cpp \
    Source/CWorldChunk.cpp \
    Source/CXMLNode.cpp \
    Source/CZip.cpp \
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
    ../COTS/SFML-1.6/src/SFML/System/Win32/ThreadLocalImpl.cpp

HEADERS  += \
    Source/Angles.h \
    Source/C3DScene.h \
    Source/CAircraft.h \
    Source/CAircraftController.h \
    Source/CAnimator.h \
    Source/CAnimatorFrame.h \
    Source/CArmature.h \
    Source/CAtmosphere.h \
    Source/CAutoTerrain.h \
    Source/CAverager.h \
    Source/CBasicAnimationStep.h \
    Source/CBasicAnimator.h \
    Source/CBasicAnimatorFrame.h \
    Source/CBILData.h \
    Source/CBILField.h \
    Source/CBone.h \
    Source/CBoundedMeshInstances.h \
    Source/CBoundingBox.h \
    Source/CBox.h \
    Source/CBuildingGenerator.h \
    Source/CCamera.h \
    Source/CCartoController.h \
    Source/CComponent.h \
    Source/CComponentFactory.h \
    Source/CComponentLoader.h \
    Source/CComponentReference.h \
    Source/CCone.h \
    Source/CConnectedStream.h \
    Source/CConsoleBoard.h \
    Source/CController.h \
    Source/CDumpable.h \
    Source/CElectricalBus.h \
    Source/CElectricalComponent.h \
    Source/CElectricalConsumer.h \
    Source/CElectricalContactor.h \
    Source/CElevator.h \
    Source/CEngine.h \
    Source/CFace.h \
    Source/CFog.h \
    Source/CForceController.h \
    Source/CGeneratedField.h \
    Source/CGenerateFunction.h \
    Source/CGeoloc.h \
    Source/CGeoZone.h \
    Source/CGLExtension.h \
    Source/CGLWidgetScene.h \
    Source/CHeightField.h \
    Source/CHGTData.h \
    Source/CHGTField.h \
    Source/CHTTPMapClient.h \
    Source/CHTTPServer.h \
    Source/CHydraulicComponent.h \
    Source/CImageFilter.h \
    Source/CImageFilter_Matrix.h \
    Source/CImageUtilities.h \
    Source/CJetEngine.h \
    Source/CJoystick.h \
    Source/CLight.h \
    Source/CLogManager.h \
    Source/CMan.h \
    Source/CMaterial.h \
    Source/CMaterialInstance.h \
    Source/CMesh.h \
    Source/CMeshInstance.h \
    Source/CMJPEGServer.h \
    Source/CNamed.h \
    Source/COBJLoader.h \
    Source/ComponentInstanciator.h \
    Source/CParented.h \
    Source/CParticle.h \
    Source/CParticleSystem.h \
    Source/CPerlin.h \
    Source/CPhysicalComponent.h \
    Source/CPIDController.h \
    Source/CPluginInterface.h \
    Source/CPluginLoader.h \
    Source/CPreferencesManager.h \
    Source/CQ3DConstants.h \
    Source/CQ3DLoader.h \
    Source/CQuick3DUtilities.h \
    Source/CRain.h \
    Source/CRenderContext.h \
    Source/CRessourcesManager.h \
    Source/CRudder.h \
    Source/CSeaVehicle.h \
    Source/CServoAnimatorFrame.h \
    Source/CServoPosition.h \
    Source/CShaderCollection.h \
    Source/CSingleton.h \
    Source/CSkyBox.h \
    Source/CSocketStream.h \
    Source/CSphere.h \
    Source/CSRTMData.h \
    Source/CSRTMField.h \
    Source/CStandardController.h \
    Source/CTankController.h \
    Source/CTerrain.h \
    Source/CTerrestrialVehicle.h \
    Source/CTexture.h \
    Source/CTiledMaterial.h \
    Source/CTimeSampler.h \
    Source/CTracableMutex.h \
    Source/CTrajectorable.h \
    Source/CTrajectory.h \
    Source/CTreeGenerator.h \
    Source/CUtility.h \
    Source/CVehicle.h \
    Source/CVertex.h \
    Source/CVertexGroup.h \
    Source/CView.h \
    Source/CViewport.h \
    Source/CWater.h \
    Source/CWaterMaterial.h \
    Source/CWaypoint.h \
    Source/CWheel.h \
    Source/CWing.h \
    Source/CWorker.h \
    Source/CWorkerManager.h \
    Source/CWorldChunk.h \
    Source/CXMLNode.h \
    Source/CZip.h \
    Source/glext.h \
    Source/ILogger.h \
    Source/Interpolator.h \
    Source/IProgressListener.h \
    Source/quick3d_global.h \
    Source/CVector3.h \
    Source/CVector2.h \
    Source/CVector4.h \
    Source/CRay3.h \
    Source/CPlane3.h \
    Source/CQuaternion.h \
    Source/CMatrix4.h \
    Source/CAxis.h \
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
    ../COTS/SFML-1.6/src/SFML/System/Win32/ThreadLocalImpl.hpp

DISTFILES +=

RESOURCES += \
    Quick3DBase.qrc \
    Quick3DShaders.qrc
