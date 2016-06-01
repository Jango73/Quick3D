@ECHO OFF
SET SolutionDir=%1
XCOPY "Debug DLL\*.dll" "%SolutionDir%\Quick3DTest\Debug" /I /Y /D
