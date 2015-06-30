@echo off

echo NOTE: You need to build the companion apps before using this script.
echo The folder Pasta-Release will be erased, and PastaCFW.zip too.
echo .
echo IMPORTANT: The executable bit of pastaConfig cannot be set in Windows. Please use the sh script if possible.

cd Brahma_Fork
::make clean
::make
if not exist PastaCFW.smdh (
	cd..
	echo "ERROR: Cannot find PastaCFW.smdh."
	GOTO:EOF
)
if not exist PastaCFW.3dsx (
	cd..
	echo "ERROR: Cannot find PastaCFW.3dsx."
	GOTO:EOF
)

cd ..\CFW_loader
::make clean
::make
if not exist loader.bin (
	echo "ERROR: Cannot find loader.bin."
	GOTO:EOF
)

cd ..
rd /S /Q Pasta-Release
mkdir Pasta-Release\3ds\PastaCFW\UI\0
copy Brahma_Fork\PastaCFW.smdh Pasta-Release\3ds\PastaCFW\
copy Brahma_Fork\PastaCFW.3dsx Pasta-Release\3ds\PastaCFW\
copy CFW_loader\loader.bin Pasta-Release\3ds\PastaCFW\
copy CFW_loader\gfx\*.* Pasta-Release\3ds\PastaCFW\UI\0\
cd Pasta-Release\3ds\PastaCFW\UI\0\ 

for /r %%i in (*.png) do (
echo Preparing %%~ni
del %%~pi\%%~ni.bin command > nul 2>&1
del %%~pi\%%~ni.bgr command > nul 2>&1
convert -rotate 90 %%i %%~ni.bgr
rename %%~ni.bgr %%~ni.bin
)

cd ..\..\..\..\..\
del Pasta-Release\3ds\PastaCFW\UI\0\*.png
copy "Companion_App\Mac-Linux\pastaConfig\builds\GNU-Linux 32 bit\pastaConfig" Pasta-Release\pastaConfig-32bit
copy "Companion_App\Mac-Linux\pastaConfig\builds\GNU-Linux 64 bit\pastaConfig" Pasta-Release\pastaConfig-64bit
copy "Companion_App\Windows\PastaCFW Configurator\PastaCFW Configurator\bin\Debug\PastaCFW Configurator.exe" Pasta-Release\
del PastaCFW.zip
if exist "%programfiles%\7-Zip\7z.exe" (
	echo Using 7-Zip.
	cd Pasta-Release
	"%programfiles%\7-Zip\7z.exe" -mx10 a "..\PastaCFW.zip" "*"
	cd ..
	rd /S /Q Pasta-Release
) else if exist "%programfiles%\WinRAR\WinRAR.exe" (
		echo Using WinRAR.
		cd Pasta-Release
		"%programfiles%\WinRAR\WinRAR.exe" a -afzip -r -m5 "..\PastaCFW.zip" "*"
		cd ..
		rd /S /Q Pasta-Release
	) else (
		echo WARNING: 7-Zip or WinRAR not found in "%programfiles%". Skipping zip packaging.
	)
)