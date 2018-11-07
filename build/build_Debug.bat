@For /F "Skip=1" %%i in ('Wmic Logicaldisk Where "DriveType=3" Get Name') Do call "%%i\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\amd64_x86\vcvarsamd64_x86.bat" %2
set VisualStudioVersion=14.0
devenv miniblink.sln /build "Debug|Win32" /Out ../out/Release/build.log /Project miniblink

pause
