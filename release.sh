#!/bin/sh
echo "NOTE: You need to build the companion apps before using this script."
echo "The folder Pasta-Release will be erased, and PastaCFW.zip too."

cd Brahma_Fork
make clean
make
if [ ! -f PastaCFW.smdh -a f PastaCFW.3dsx ]; then
	echo "ERROR: Cannot find PastaCFW.smdh or PastaCFW.3dsx."
	exit
fi

cd ../CFW_loader
make clean
make
if [ ! -f loader.bin ]; then
	echo "ERROR: Cannot find loader.bin."
	exit
fi

cd ..
rm -r Pasta-Release
mkdir -p Pasta-Release/3ds/PastaCFW/UI/0
cp -r Brahma_Fork/PastaCFW.smdh Brahma_Fork/PastaCFW.3dsx CFW_loader/loader.bin Pasta-Release/3ds/PastaCFW/
cp CFW_loader/gfx/*.* Pasta-Release/3ds/PastaCFW/UI/0/
(cd Pasta-Release/3ds/PastaCFW/UI/0/ && sh makebgr-all.sh)
rm Pasta-Release/3ds/PastaCFW/UI/0/*.png
cp Companion_App/Mac-Linux/pastaConfig/builds/GNU-Linux\ 32\ bit/pastaConfig Pasta-Release/pastaConfig-32bit
cp Companion_App/Mac-Linux/pastaConfig/builds/GNU-Linux\ 64\ bit/pastaConfig Pasta-Release/pastaConfig-64bit
chmod +x Pasta-Release/pastaConfig-32bit Pasta-Release/pastaConfig-64bit Pasta-Release/3ds/PastaCFW/UI/0/*.sh
cp Companion_App/Windows/PastaCFW\ Configurator/PastaCFW\ Configurator/bin/Debug/PastaCFW\ Configurator.exe Pasta-Release/
rm PastaCFW.zip
(cd Pasta-Release && zip -r -9 ../PastaCFW.zip .)
rm -r Pasta-Release
