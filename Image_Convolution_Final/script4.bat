@echo off


set /a nrOfRuns=5
set /a displayTime=1
set /a runCounter = 0
set imageName=4k/snail.jpg
set filter=sharpen3x3
set resolution=4k

:Start
@echo off
REM echo       %resolution% images %filter% (BASIC)>>TEST_RESULTS4.txt
REM echo -------------------------------------------------->>TEST_RESULTS4.txt
REM echo            ---------CPU-single--------->>TEST_RESULTS4.txt
REM echo.
REM echo       %resolution% images %filter% (BASIC) 
REM echo --------------------------------------------------
REM echo            ---------CPU-single---------

REM set /a c=50

REM setlocal ENABLEDELAYEDEXPANSION
REM for /l %%x in (1, 1, !nrOfRuns!) do (
	
	REM gpu_advanced.exe !imageName! CPU-single Basic !filter! 0 !displayTime! 0 >>TEST_RESULTS4.txt
	REM <nul set /p = ^>
	REM IF %%x GEQ !c! (
		REM echo.
		REM set /a c=c+50
	REM )
REM )
REM endlocal

echo -------------------------------------------------->>TEST_RESULTS4.txt
echo       %resolution% images %filter% (BASIC)>>TEST_RESULTS4.txt
echo            ---------CPU-Multi---------->>TEST_RESULTS4.txt
echo.
echo            ---------CPU-Multi----------
set /a c=50
setlocal ENABLEDELAYEDEXPANSION
for /l %%x in (1, 1, !nrOfRuns!) do (
	
	gpu_advanced.exe !imageName! CPU Basic !filter! 0 !displayTime! 0 >>TEST_RESULTS4.txt
	<nul set /p = ^>
	IF %%x GEQ !c! (
		echo.
		set /a c=c+50
	)
)
endlocal

echo -------------------------------------------------->>TEST_RESULTS4.txt
echo       %resolution% images %filter% (BASIC)>>TEST_RESULTS4.txt
echo            ---------GPU-Multi-----%resolution%-----%filter%------->>TEST_RESULTS4.txt
echo.
echo            ---------GPU-Multi-----%resolution%-----%filter%-------
set /a c=50
setlocal ENABLEDELAYEDEXPANSION
for /l %%x in (1, 1, !nrOfRuns!) do (
	
	gpu_advanced.exe !imageName! GPU Basic !filter! 0 !displayTime! 0 >>TEST_RESULTS4.txt
	<nul set /p = ^>
	IF %%x GEQ !c! (
		echo.
		set /a c=c+50
	)
)
endlocal

REM echo       %resolution% images %filter% (OPTIMIZED)>>TEST_RESULTS4.txt
REM echo -------------------------------------------------->>TEST_RESULTS4.txt
REM echo            ---------CPU-Multi---------->>TEST_RESULTS4.txt
REM echo.
REM echo       %resolution% images %filter% (OPTIMIZED) 
REM echo --------------------------------------------------
REM echo            ---------CPU-Multi----------

REM set /a c=50
REM setlocal ENABLEDELAYEDEXPANSION
REM for /l %%x in (1, 1, !nrOfRuns!) do (
	
	REM gpu_advanced.exe !imageName! CPU Optimized !filter! 0 !displayTime! 0 >>TEST_RESULTS4.txt
	REM <nul set /p = ^>
	REM IF %%x GEQ !c! (
		REM echo.
		REM set /a c=c+50
	REM )
REM )
REM endlocal


echo       %resolution% images %filter% (OPTIMIZED)>>TEST_RESULTS4.txt
echo -------------------------------------------------->>TEST_RESULTS4.txt
echo            ---------GPU-Multi---------->>TEST_RESULTS4.txt
echo.
echo            ---------GPU-Multi----------
set /a c=50
setlocal ENABLEDELAYEDEXPANSION
for /l %%x in (1, 1, !nrOfRuns!) do (
	
	gpu_advanced.exe !imageName! GPU Optimized !filter! 0 !displayTime! 0 >>TEST_RESULTS4.txt
	<nul set /p = ^>
	IF %%x GEQ !c! (
		echo.
		set /a c=c+50
	)
)
endlocal

set /a runCounter=runCounter+1
set /a tmp=0
IF %runCounter% GTR 4 (
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

echo.
echo.
GOTO Start


:End
REM shutdown.exe /s /t 00
echo.
pause