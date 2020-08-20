set SOURCEDIR=..\
set DATADIR=..\..\Cortex-Command-Community-Project-Data
set TARGETDIR=..\..\Cortex-Command-Community-Project-Release
set ORIGDIR=%cd%

if NOT EXIST %TARGETDIR% GOTO EMPTY
rmdir /S /Q %TARGETDIR%
:EMPTY

mkdir %TARGETDIR%

rem for /f "delims=" %%i in ('dir /ad/b *.rte') do xcopy /Y %%i %TARGETDIR%\%%i\ /S /I

rem Manually copy modules, to avoid copying of mods and debug modules
xcopy /Y %DATADIR%\Base.rte %TARGETDIR%\Base.rte\ /S /I /Q
xcopy /Y %DATADIR%\Missions.rte %TARGETDIR%\Missions.rte\ /S /I /Q
xcopy /Y %DATADIR%\Tutorial.rte %TARGETDIR%\Tutorial.rte\ /S /I /Q

xcopy /Y .\ReferenceFolders\Scenes.rte %TARGETDIR%\Scenes.rte\ /S /I /Q
xcopy /Y .\ReferenceFolders\Metagames.rte %TARGETDIR%\Metagames.rte\ /S /I /Q

rem Delete settings.ini as it's re-created anyway, and allows to preserve player's settings after update
del %TARGETDIR%\Base.rte\Settings.ini /Q

xcopy /Y %DATADIR%\Coalition.rte %TARGETDIR%\Coalition.rte\ /S /I /Q
xcopy /Y %DATADIR%\Dummy.rte %TARGETDIR%\Dummy.rte\ /S /I /Q
xcopy /Y %DATADIR%\Ronin.rte %TARGETDIR%\Ronin.rte\ /S /I /Q
xcopy /Y %DATADIR%\Techion.rte %TARGETDIR%\Techion.rte\ /S /I /Q
xcopy /Y %DATADIR%\Imperatus.rte %TARGETDIR%\Imperatus.rte\ /S /I /Q
xcopy /Y %DATADIR%\Browncoats.rte %TARGETDIR%\Browncoats.rte\ /S /I /Q
xcopy /Y %DATADIR%\MuIlaak.rte %TARGETDIR%\MuIlaak.rte\ /S /I /Q
xcopy /Y %DATADIR%\Uzira.rte %TARGETDIR%\Uzira.rte\ /S /I /Q

xcopy /Y %DATADIR%\Benchmark.rte %TARGETDIR%\Benchmark.rte\ /S /I /Q

xcopy /Y "%DATADIR%\Cortex Command.exe" %TARGETDIR%
xcopy /Y "%DATADIR%\Credits.txt" %TARGETDIR%

xcopy /Y %SOURCEDIR%\external\lib\fmod.dll %TARGETDIR%
xcopy /Y %SOURCEDIR%\external\lib\lua51.dll %TARGETDIR%
xcopy /Y %SOURCEDIR%\external\lib\liblz4.dll %TARGETDIR%
xcopy /Y %SOURCEDIR%\external\lib\zlibwapi.dll %TARGETDIR%
