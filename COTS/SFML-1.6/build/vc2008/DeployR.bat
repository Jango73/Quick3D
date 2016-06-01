@ECHO OFF
SET SolutionDir=%1
XCOPY "Release DLL\*.dll" "%SolutionDir%\Quick3DTest\Release" /I /Y /D
