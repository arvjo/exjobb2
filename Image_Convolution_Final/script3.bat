@echo off

set /a nrOfRuns=3
set /a displayTime=1
set /a runCounter = 0
set imageName=4k/snail.jpg
set filter=sharpen11x11
set resolution=4K

REM @echo off
echo       %resolution% images %filter% (OPTIMIZED WITH NEW KERNEL)>>TEST_RESULTS.txt
echo -------------------------------------------------->>TEST_RESULTS.txt
echo.
echo       %resolution% images %filter% (OPTIMIZED WITH NEW KERNEL) 
echo --------------------------------------------------

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

echo       %resolution% images %filter% (OPTIMIZED WITHOUT NEW KERNEL)>>TEST_RESULTS.txt
echo -------------------------------------------------->>TEST_RESULTS.txt
echo.
echo       %resolution% images %filter% (OPTIMIZED WITHOUT NEW KERNEL) 
echo --------------------------------------------------

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
