call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsamd64_x86.bat" %2
set VisualStudioVersion=16.0
devenv miniblink.sln /Project "miniblink" /build "Debug|Win32" /Out ../out/Debug/build.log
devenv miniblink.sln /Project "miniblink" /build "Release|Win32" /Out ../out/Release/build.log
devenv miniblink.sln /Project "miniblink" /build "Release_vc6|Win32" /Out ../out/Release_vc6/build.log
devenv miniblink.sln /Project "miniblink" /build "Debug|x64" /Out ../out/x64/Debug/build.log
devenv miniblink.sln /Project "miniblink" /build "Release|x64" /Out ../out/x64/Release/build.log
pause
