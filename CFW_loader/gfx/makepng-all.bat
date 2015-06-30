@echo off
for /r %%i in (menu?.bin) do (
call :conv %%i 320
)
for /r %%i in (nand*.bin) do (
call :conv %%i 320
)
for /r %%i in (arm*.bin) do (
call :conv %%i 320
)

call :conv options.bin 320

for /r %%i in (*TOP.bin) do (
call :conv %%i 400
)
GOTO:EOF

:conv
del UI\%~n1.png command > nul 2>&1
convert -size 240x%2 -depth 8 bgr:%1 -size %2x240 -rotate 270 %~n1.png
GOTO:EOF