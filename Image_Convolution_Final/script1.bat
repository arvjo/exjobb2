@echo off


set /a nrOfRuns=100
set /a displayTime=0
set /a runCounter = 0
set imageName=480p/snail.jpg
set filter=sharpen3x3
set resolution=480p

:Start

@echo off
echo       %resolution% images %filter% (BASIC)>>TEST_RESULTS.txt
echo -------------------------------------------------->>TEST_RESULTS.txt
echo            ---------CPU-single--------->>TEST_RESULTS.txt
echo.
echo       %resolution% images %filter% (BASIC) 
echo --------------------------------------------------
echo            ---------CPU-single---------

set /a c=50

setlocal ENABLEDELAYEDEXPANSION
for /l %%x in (1, 1, !nrOfRuns!) do (
	
	gpu_advanced.exe !imageName! CPU-single Basic !filter! 0 !displayTime! 0 >>TEST_RESULTS.txt
	<nul set /p = ^>
	IF %%x GEQ !c! (
		echo.
		set /a c=c+50
	)
)
endlocal

echo -------------------------------------------------->>TEST_RESULTS.txt
echo       %resolution% images %filter% (BASIC)>>TEST_RESULTS.txt
echo            ---------CPU-Multi---------->>TEST_RESULTS.txt
echo.
echo            ---------CPU-Multi----------
set /a c=50
setlocal ENABLEDELAYEDEXPANSION
for /l %%x in (1, 1, !nrOfRuns!) do (
	
	gpu_advanced.exe !imageName! CPU Basic !filter! 0 !displayTime! 0 >>TEST_RESULTS.txt
	<nul set /p = ^>
	IF %%x GEQ !c! (
		echo.
		set /a c=c+50
	)
)
endlocal

echo -------------------------------------------------->>TEST_RESULTS.txt
echo       %resolution% images %filter% (BASIC)>>TEST_RESULTS.txt
echo            ---------GPU-Multi---------->>TEST_RESULTS.txt
echo.
echo            ---------GPU-Multi----------
set /a c=50
setlocal ENABLEDELAYEDEXPANSION
for /l %%x in (1, 1, !nrOfRuns!) do (
	
	gpu_advanced.exe !imageName! GPU Basic !filter! 0 !displayTime! 0 >>TEST_RESULTS.txt
	<nul set /p = ^>
	IF %%x GEQ !c! (
		echo.
		set /a c=c+50
	)
)
endlocal

echo       %resolution% images %filter% (OPTIMIZED)>>TEST_RESULTS.txt
echo -------------------------------------------------->>TEST_RESULTS.txt
echo            ---------CPU-Multi---------->>TEST_RESULTS.txt
echo.
echo       %resolution% images %filter% (OPTIMIZED) 
echo --------------------------------------------------
echo            ---------CPU-Multi----------

set /a c=50
setlocal ENABLEDELAYEDEXPANSION
for /l %%x in (1, 1, !nrOfRuns!) do (
	
	gpu_advanced.exe !imageName! CPU Optimized !filter! 0 !displayTime! 0 >>TEST_RESULTS.txt
	<nul set /p = ^>
	IF %%x GEQ !c! (
		echo.
		set /a c=c+50
	)
)
endlocal


echo       %resolution% images %filter% (OPTIMIZED)>>TEST_RESULTS.txt
echo -------------------------------------------------->>TEST_RESULTS.txt
echo            ---------GPU-Multi---------->>TEST_RESULTS.txt
echo.
echo            ---------GPU-Multi----------
set /a c=50
setlocal ENABLEDELAYEDEXPANSION
for /l %%x in (1, 1, !nrOfRuns!) do (
	
	gpu_advanced.exe !imageName! GPU Optimized !filter! 0 !displayTime! 0 >>TEST_RESULTS.txt
	<nul set /p = ^>
	IF %%x GEQ !c! (
		echo.
		set /a c=c+50
	)
)
endlocal

set /a runCounter=runCounter+1
set /a tmp=0
IF %runCounter% GTR 36 (
	GOTO End
)
IF %runCounter% EQU 1 (
	set filter=sharpen5x5
)
IF %runCounter% EQU 2 (
	set filter=sharpen7x7
)
IF %runCounter% EQU 3 (
	set filter=sharpen9x9
)
IF %runCounter% EQU 4 (
	set filter=sharpen11x11
)
IF %runCounter% EQU 5 (
	set /a runCounter=7
)
IF %runCounter% EQU 6 (
	set /a runCounter=7
)
REM 720p-----
IF %runCounter% EQU 7 (
	set filter=sharpen3x3
	set imageName=720p/green.jpg
	set resolution=720p
)
IF %runCounter% EQU 8 (
	set filter=sharpen5x5
)
IF %runCounter% EQU 9 (
	set filter=sharpen7x7
)
IF %runCounter% EQU 10 (
	set filter=sharpen9x9
)
IF %runCounter% EQU 11 (
	set filter=sharpen11x11
)
IF %runCounter% EQU 12 (
	REM set filter=sharpen13x13'
	set /a runCounter=13
)
REM 1080p-----
IF %runCounter% EQU 13 (
	set filter=sharpen3x3
	set imageName=1080p/blue.jpg
	set resolution=1080p
)
IF %runCounter% EQU 14 (
	set filter=sharpen5x5
)
IF %runCounter% EQU 15 (
	set filter=sharpen7x7
)
IF %runCounter% EQU 16 (
	set filter=sharpen9x9
)
IF %runCounter% EQU 17 (
	set filter=sharpen11x11
)
IF %runCounter% EQU 18 (
	set /a runCounter=19
)
REM 1440p-----
IF %runCounter% EQU 19 (
	set filter=sharpen3x3
	set imageName=1440p/ruins.jpg
	set resolution=1440p
)
IF %runCounter% EQU 20 (
	set filter=sharpen5x5
)
IF %runCounter% EQU 21 (
	set filter=sharpen7x7
)
IF %runCounter% EQU 22 (
	set filter=sharpen9x9
)
IF %runCounter% EQU 23 (
	set filter=sharpen11x11
)
IF %runCounter% EQU 24 (
	set /a runCounter=25
)
REM 4k-----
IF %runCounter% EQU 25 (
	set filter=sharpen3x3
	set imageName=4k/snail.jpg
	set resolution=4k
)
IF %runCounter% EQU 26 (
	set filter=sharpen5x5
)
IF %runCounter% EQU 27 (
	set filter=sharpen7x7
)
IF %runCounter% EQU 28 (
	set filter=sharpen9x9
)
IF %runCounter% EQU 29 (
	set filter=sharpen11x11
)
IF %runCounter% EQU 30 (
	set /a runCounter=31
)
REM 8k-----
IF %runCounter% EQU 31 (
	set filter=sharpen3x3
	set imageName=8k/ruins.jpg
	set resolution=8k
)
IF %runCounter% EQU 32 (
	set filter=sharpen5x5
)
IF %runCounter% EQU 33 (
	set filter=sharpen7x7
)
IF %runCounter% EQU 34 (
	set filter=sharpen9x9
)
IF %runCounter% EQU 35 (
	set filter=sharpen11x11
)
IF %runCounter% EQU 36 (
	set /a runCounter=37
)

echo.
echo.
GOTO Start


:End
shutdown.exe /s /t 00
echo.
pause