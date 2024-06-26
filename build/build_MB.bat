call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars64.bat" %2
set VisualStudioVersion=16.0
devenv miniblink.sln /build "Debug|Win32" /Out ../out/Debug/build.log /Project miniblink
devenv miniblink.sln /build "Release|Win32" /Out ../out/Release/build.log /Project miniblink
devenv miniblink.sln /build "Release_NoNode|Win32" /Out ../out/Release_vc6/build.log /Project miniblink
devenv miniblink.sln /build "Release_vc6|Win32" /Out ../out/Release_vc6/build.log /Project miniblink
devenv miniblink.sln /build "Debug|x64" /Out ../out/x64/Debug/build.log /Project miniblink
devenv miniblink.sln /build "Release|x64" /Out ../out/x64/Release/build.log /Project miniblink
devenv miniblink.sln /build "Release_NoNode|x64" /Out ../out/Release_NoNode/build.log /Project miniblink
pause
