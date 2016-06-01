@ECHO OFF
SET SolutionDir=%1
XCOPY "x86\ZlibDllRelease\*.lib" "%SolutionDir%\Lib" /I /Y /D
XCOPY "x86\ZlibDllRelease\*.dll" "%SolutionDir%\Lib" /I /Y /D
XCOPY "x86\ZlibDllRelease\*.dll" "%SolutionDir%\Deploy\LBDDVeille" /I /Y /D
XCOPY "x86\ZlibDllRelease\*.dll" "%SolutionDir%\Deploy\LBDDSenseur" /I /Y /D
XCOPY "x86\ZlibDllRelease\*.dll" "%SolutionDir%\Deploy\LBDDDetection" /I /Y /D
XCOPY "x86\ZlibDllRelease\*.dll" "%SolutionDir%\Deploy\LPCCVideo" /I /Y /D
XCOPY "x86\ZlibDllRelease\*.dll" "%SolutionDir%\Deploy\LPCCService" /I /Y /D
XCOPY "x86\ZlibDllRelease\*.dll" "%SolutionDir%\Deploy\Tests" /I /Y /D
XCOPY "x86\ZlibDllRelease\*.dll" "%SolutionDir%\Deploy\Tests\IHMCapta" /I /Y /D
XCOPY "x86\ZlibDllRelease\*.dll" "%SolutionDir%\Deploy\Tests\ConsoleMaint" /I /Y /D
XCOPY "x86\ZlibDllRelease\*.dll" "%SolutionDir%\Deploy\Tests\BouchonCOTSDetection" /I /Y /D
XCOPY "x86\ZlibDllRelease\*.dll" "%SolutionDir%\Deploy\Tests\CCOTSDetectionIHM" /I /Y /D
XCOPY "x86\ZlibDllRelease\*.dll" "%SolutionDir%\Deploy\Tests\Noctua" /I /Y /D
XCOPY "x86\ZlibDllRelease\*.dll" "%SolutionDir%\Simulator" /I /Y /D
