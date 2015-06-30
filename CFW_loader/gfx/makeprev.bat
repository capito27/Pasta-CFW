@echo off

set delay=%1
if "%delay%" == "" set delay=100

if not exist template.png (
	bitsadmin /transfer "3DS-Template" /download /priority high http://www.nintendo.com/images/page/3ds/what-is-3ds/hero-new-3ds.png %cd%\template.png
)

mkdir Preview\Images\AIO Preview\Images\Menu Preview\Images\NAND-dump Preview\Images\ARM9-dump Preview\Animations\AIO Preview\Animations\Menu Preview\Animations\NAND-dump Preview\Animations\ARM9-dump


if exist menuTOP.png (
	for /l %%x in (0, 1, 5) do (
		call :makep1 menuTOP.png menu%%x.png
	)

	for /l %%x in (0, 1, 1) do (
		call :makep1 menuTOP.png nand%%x.png
	)

	call :makep1 menuTOP.png nand2E.png
	call :makep1 menuTOP.png nand2OK.png

	for /l %%x in (0, 1, 1) do (
		call :makep1 menuTOP.png arm9%%x.png
	)

	call :makep1 menuTOP.png arm92E.png
	call :makep1 menuTOP.png arm92OK.png

	call :makep1 menuTOP.png options.png
) else (
	echo "ERROR: Cannot find menuTOP.png."
)

if exist creditsTOP.png (
	call :makep1 creditsTOP.png menu6.png
) else (
	echo "ERROR: Cannot find creditsTOP.png."
)

convert Preview\menu0.png Preview\menu6.png Preview\nand0.png +append Preview\menuprev-aio.png
convert -delay %delay% -loop 0 menu0.png menu1.png menu2.png menu3.png menu4.png menu5.png menu6.png Preview\menuprev-1.gif
convert -delay %delay% -loop 0 Preview\menu0.png Preview\menu1.png Preview\menu2.png Preview\menu3.png Preview\menu4.png Preview\menu5.png Preview\menu6.png Preview\menuprev-2.gif
convert -delay %delay% -loop 0 nand0.png nand1.png nand2OK.png nand2E.png Preview\nand-0.gif
convert -delay %delay% -loop 0 Preview\nand0.png Preview\nand1.png Preview\nand2OK.png Preview\nand2E.png Preview\nand-1.gif
convert -delay %delay% -loop 0 arm90.png arm91.png arm92OK.png arm92E.png Preview\arm9-0.gif
convert -delay %delay% -loop 0 Preview\arm90.png Preview\arm91.png Preview\arm92OK.png Preview\arm92E.png Preview\arm9-1.gif
convert -delay %delay% -loop 0 menu0.png menu1.png menu2.png menu3.png menu4.png menu5.png menu6.png options.png nand0.png nand1.png nand2OK.png nand2E.png arm90.png arm91.png arm92OK.png arm92E.png Preview\menuprev-aio-0.gif
convert -delay %delay% -loop 0 Preview\menu0.png Preview\menu1.png Preview\menu2.png Preview\menu3.png Preview\menu4.png Preview\menu5.png Preview\menu6.png Preview\options.png Preview\nand0.png Preview\nand1.png Preview\nand2OK.png Preview\nand2E.png Preview\arm90.png Preview\arm91.png Preview\arm92OK.png Preview\arm92E.png Preview\menuprev-aio-1.gif

cd Preview

move menuprev-aio-0.gif Animations\AIO
move menuprev-aio-1.gif Animations\AIO
move menuprev-1.gif Animations\Menu
move menuprev-2.gif Animations\Menu
move nand-0.gif Animations\NAND-dump
move nand-1.gif Animations\NAND-dump
move arm9-0.gif Animations\ARM9-dump
move arm9-1.gif Animations\ARM9-dump

move menuprev-aio.png Images\AIO
move options.png Images
move menu0.png Images\Menu
move menu1.png Images\Menu
move menu2.png Images\Menu
move menu3.png Images\Menu
move menu4.png Images\Menu
move menu5.png Images\Menu
move menu6.png Images\Menu
move nand0.png Images\NAND-dump
move nand1.png Images\NAND-dump
move nand2E.png Images\NAND-dump
move nand2OK.png Images\NAND-dump
move arm90.png Images\ARM9-dump
move arm91.png Images\ARM9-dump
move arm92E.png Images\ARM9-dump
move arm92OK.png Images\ARM9-dump

cd..
GOTO:EOF

:makep1
	if exist %2 (
		convert %1 -filter Lanczos -resize 264x158 Preview\temp1.png
		convert template.png Preview\temp1.png -geometry +71+34 -composite Preview\temp2.png
		del Preview\temp1.png
		convert %2 -filter Lanczos -resize 213x160 Preview\temp1.png
		convert Preview\temp2.png Preview\temp1.png -geometry +96+240 -composite Preview\%2
		del Preview\temp1.png
		del Preview\temp2.png
	) else (
		echo "ERROR: Cannot find %2."
	)
GOTO:EOF
