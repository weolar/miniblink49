call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\amd64_x86\vcvarsamd64_x86.bat" %2
set VisualStudioVersion=14.0
devenv miniblink.sln /build "Debug|Win32" /Out ../out/Debug/build.log
devenv miniblink.sln /build "Release|Win32" /Out ../out/Release/build.log
devenv miniblink.sln /build "Release_vc6|Win32" /Out ../out/Release_vc6/build.log
devenv miniblink.sln /build "Debug|x64" /Out ../out/x64/Debug/build.log
devenv miniblink.sln /build "Release|x64" /Out ../out/x64/Release/build.log
pause
