call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars64.bat" %2
set VisualStudioVersion=16.0
devenv miniblink.sln /build "Release|Win32" /Out ../out/Release/build.log /Project miniblink

pause
