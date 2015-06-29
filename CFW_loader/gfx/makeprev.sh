#!/bin/sh
if [ ! -f template.png ]; then
	curl -O "http://www.nintendo.com/images/page/3ds/what-is-3ds/hero-new-3ds.png"
	mv hero-new-3ds.png template.png
fi

mkdir -p Preview/Images/AIO Preview/Images/Menu Preview/Images/NAND-dump Preview/Images/ARM9-dump Preview/Animations/AIO Preview/Animations/Menu Preview/Animations/NAND-dump Preview/Animations/ARM9-dump

makep1()
{
	if [ -f $2 ]; then
		convert $1 -filter Lanczos -resize 264x158 Preview/temp1.png
		convert template.png Preview/temp1.png -geometry +71+34 -composite Preview/temp2.png
		rm Preview/temp1.png
		convert $2 -filter Lanczos -resize 213x160 Preview/temp1.png
		convert Preview/temp2.png Preview/temp1.png -geometry +96+240 -composite Preview/$2
		rm Preview/temp1.png Preview/temp2.png
	else
		echo "ERROR: Cannot find $2."
	fi
}

if [ -f menuTOP.png ]; then
	for i in `seq 0 5`; do
		makep1 menuTOP.png menu$i.png
	done

	for i in `seq 0 1`; do
		makep1 menuTOP.png nand$i.png
	done

	makep1 menuTOP.png nand2E.png
	makep1 menuTOP.png nand2OK.png

	for i in `seq 0 1`; do
		makep1 menuTOP.png arm9$i.png
	done

	makep1 menuTOP.png arm92E.png
	makep1 menuTOP.png arm92OK.png

	makep1 menuTOP.png options.png
else
	echo "ERROR: Cannot find menuTOP.png."
fi

if [ -f creditsTOP.png ]; then
	makep1 creditsTOP.png menu6.png
else
	echo "ERROR: Cannot find creditsTOP.png."
fi

convert Preview/menu0.png Preview/menu6.png Preview/nand0.png +append Preview/menuprev-aio.png
convert -delay 100 -loop 0 menu?.png Preview/menuprev-1.gif
convert -delay 100 -loop 0 Preview/menu?.png Preview/menuprev-2.gif
convert -delay 100 -loop 0 nand?.png nand2OK.png nand2E.png Preview/nand-0.gif
convert -delay 100 -loop 0 Preview/nand?.png Preview/nand2OK.png Preview/nand2E.png Preview/nand-1.gif
convert -delay 100 -loop 0 arm9?.png arm92OK.png arm92E.png Preview/arm9-0.gif
convert -delay 100 -loop 0 Preview/arm9?.png Preview/arm92OK.png Preview/arm92E.png Preview/arm9-1.gif
convert -delay 100 -loop 0 menu?.png options.png nand?.png nand2OK.png nand2E.png arm9?.png arm92OK.png arm92E.png Preview/menuprev-aio-0.gif
convert -delay 100 -loop 0 Preview/menu?.png Preview/options.png Preview/nand?.png Preview/nand2OK.png Preview/nand2E.png Preview/arm9?.png Preview/arm92OK.png Preview/arm92E.png Preview/menuprev-aio-1.gif

cd Preview

mv menuprev-aio*.gif Animations/AIO
mv menuprev-1.gif menuprev-2.gif Animations/Menu
mv nand*.gif Animations/NAND-dump
mv arm9*.gif Animations/ARM9-dump

mv menuprev-aio.png Images/AIO
mv options.png Images
mv menu*.png Images/Menu
mv nand*.png Images/NAND-dump
mv arm9*.png Images/ARM9-dump
