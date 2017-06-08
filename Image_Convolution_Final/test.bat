@ECHO off



::480p images, 3x3 filters
echo "480p images"
@ECHO off
gpu_basic.exe 480p/snail.jpg GPU test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Box blur 3x3			: Passed in %VAR% seconds.
   echo Box blur 3x3			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Box blur 3x3: Failed
	echo Box blur 3x3: Failed >> log.txt
)

gpu_basic.exe 480p/green.jpg CPU test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Diagonal motion blur 3x3	: Passed in %VAR% seconds.
   echo Diagonal motion blur 3x3	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Diagonal motion blur 3x3: Failed
	echo Diagonal motion blur 3x3: Failed >> log.txt
)
gpu_basic.exe 480p/blue.jpg sharpen3x3 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Sharpen 3x3			: Passed in %VAR% seconds.
   echo Sharpen 3x3			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Sharpen 3x3: Failed
	echo Sharpen 3x3: Failed >> log.txt
)
gpu_basic.exe 480p/ruins.jpg hor_edge_detect3x3 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Horizontal edge detection 3x3	: Passed in %VAR% seconds.
   echo Horizontal edge detection 3x3	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Horizontal edge detection 3x3: Failed
	echo Horizontal edge detection 3x3: Failed >> log.txt
)

::720p images, 3x3 filters
echo "720p images"
@ECHO off
gpu_basic.exe 720p/snail.jpg box3x3 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Box blur 3x3			: Passed in %VAR% seconds.
    echo Box blur 3x3			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Box blur 3x3: Failed
	echo Box blur 3x3: Failed >> log.txt
)
gpu_basic.exe 720p/green.jpg diag_motion3x3 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Diagonal motion blur 3x3	: Passed in %VAR% seconds.
   echo Diagonal motion blur 3x3	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Diagonal motion blur 3x3: Failed
	echo Diagonal motion blur 3x3: Failed >> log.txt
)
gpu_basic.exe 720p/blue.jpg sharpen3x3 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Sharpen 3x3			: Passed in %VAR% seconds.
   echo Sharpen 3x3			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Sharpen 3x3: Failed
	echo Sharpen 3x3: Failed >> log.txt
)
gpu_basic.exe 720p/ruins.jpg hor_edge_detect3x3 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Horizontal edge detection 3x3	: Passed in %VAR% seconds.
   echo Horizontal edge detection 3x3	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Horizontal edge detection 3x3: Failed
	echo Horizontal edge detection 3x3: Failed >> log.txt
)


::1080p images, 3x3 filters
echo "1080p images"
@ECHO off
gpu_basic.exe 1080p/snail.jpg box3x3 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Box blur 3x3			: Passed in %VAR% seconds.
   echo Box blur 3x3			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Box blur 3x3: Failed
	echo Box blur 3x3: Failed >> log.txt
)
gpu_basic.exe 1080p/green.jpg diag_motion3x3 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Diagonal motion blur 3x3	: Passed in %VAR% seconds.
   echo Diagonal motion blur 3x3	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Diagonal motion blur 3x3: Failed
	echo Diagonal motion blur 3x3: Failed >> log.txt
)
gpu_basic.exe 1080p/blue.jpg sharpen3x3 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Sharpen 3x3			: Passed in %VAR% seconds.
   echo Sharpen 3x3			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Sharpen 3x3: Failed
	echo Sharpen 3x3: Failed >> log.txt
)
gpu_basic.exe 1080p/ruins.jpg hor_edge_detect3x3 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Horizontal edge detection 3x3	: Passed in %VAR% seconds.
   echo Horizontal edge detection 3x3	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Horizontal edge detection 3x3: Failed
	echo Horizontal edge detection 3x3: Failed >> log.txt
)
	
::1440p  images, 3x3 filters
echo "1440p images"
@ECHO off
gpu_basic.exe 1440p/snail.jpg box3x3 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Box blur 3x3			: Passed in %VAR% seconds.
   echo Box blur 3x3			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Box blur 3x3: Failed
	echo Box blur 3x3: Failed >> log.txt
)
gpu_basic.exe 1440p/green.jpg diag_motion3x3 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Diagonal motion blur 3x3	: Passed in %VAR% seconds.
   echo Diagonal motion blur 3x3	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Diagonal motion blur 3x3: Failed
	echo Diagonal motion blur 3x3: Failed >> log.txt
)
gpu_basic.exe 1440p/blue.jpg sharpen3x3 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Sharpen 3x3			: Passed in %VAR% seconds.
   echo Sharpen 3x3			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Sharpen 3x3: Failed
	echo Sharpen 3x3: Failed >> log.txt
)
gpu_basic.exe 1440p/ruins.jpg hor_edge_detect3x3 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Horizontal edge detection 3x3	: Passed in %VAR% seconds.
   echo Horizontal edge detection 3x3	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Horizontal edge detection 3x3: Failed
	echo Horizontal edge detection 3x3: Failed >> log.txt
)

::4K  images, 3x3 filters
echo "4k images"
@ECHO off
gpu_basic.exe 4k/snail.jpg box3x3 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Box blur 3x3			: Passed in %VAR% seconds.
   echo Box blur 3x3			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Box blur 3x3: Failed
	echo Box blur 3x3: Failed >> log.txt
)
gpu_basic.exe 4k/green.jpg diag_motion3x3 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Diagonal motion blur 3x3	: Passed in %VAR% seconds.
   echo Diagonal motion blur 3x3	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Diagonal motion blur 3x3: Failed
	echo Diagonal motion blur 3x3: Failed >> log.txt
)
gpu_basic.exe 4k/blue.jpg sharpen3x3 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Sharpen 3x3			: Passed in %VAR% seconds.
   echo Sharpen 3x3			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Sharpen 3x3: Failed
	echo Sharpen 3x3: Failed >> log.txt
)
gpu_basic.exe 4k/ruins.jpg hor_edge_detect3x3 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Horizontal edge detection 3x3	: Passed in %VAR% seconds.
   echo Horizontal edge detection 3x3	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Horizontal edge detection 3x3: Failed
	echo Horizontal edge detection 3x3: Failed >> log.txt
)

::8K  images, 3x3 filters
echo "8k images"
@ECHO off
gpu_basic.exe 8k/snail.jpg box3x3 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Box blur 3x3			: Passed in %VAR% seconds.
   echo Box blur 3x3			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Box blur 3x3: Failed
	echo Box blur 3x3: Failed >> log.txt
)
gpu_basic.exe 8k/green.jpg diag_motion3x3 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Diagonal motion blur 3x3	: Passed in %VAR% seconds.
   echo Diagonal motion blur 3x3	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Diagonal motion blur 3x3: Failed
	echo Diagonal motion blur 3x3: Failed >> log.txt
)
gpu_basic.exe 8k/blue.jpg sharpen3x3 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Sharpen 3x3			: Passed in %VAR% seconds.
   echo Sharpen 3x3			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Sharpen 3x3: Failed
	echo Sharpen 3x3: Failed >> log.txt
)
gpu_basic.exe 8k/ruins.jpg hor_edge_detect3x3 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Horizontal edge detection 3x3	: Passed in %VAR% seconds.
   echo Horizontal edge detection 3x3	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Horizontal edge detection 3x3: Failed	
	echo Horizontal edge detection 3x3: Failed >> log.txt
)
	
	
::480p images, 5x5 filters
echo "480p images"
@ECHO off
gpu_basic.exe 480p/snail.jpg box5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Box blur 5x5			: Passed in %VAR% seconds.
   echo Box blur 5x5			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Box blur 5x5: Failed
	echo Box blur 5x5: Failed >> log.txt
)

gpu_basic.exe 480p/green.jpg diag_motion5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Diagonal motion blur 5x5	: Passed in %VAR% seconds.
   echo Diagonal motion blur 5x5	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Diagonal motion blur 5x5: Failed
	echo Diagonal motion blur 5x5: Failed >> log.txt
)
gpu_basic.exe 480p/blue.jpg sharpen5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Sharpen 5x5			: Passed in %VAR% seconds.
   echo Sharpen 5x5			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Sharpen 5x5: Failed
	echo Sharpen 5x5: Failed >> log.txt
)
gpu_basic.exe 480p/ruins.jpg hor_edge_detect5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Horizontal edge detection 5x5	: Passed in %VAR% seconds.
   echo Horizontal edge detection 5x5	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Horizontal edge detection 5x5: Failed
	echo Horizontal edge detection 5x5: Failed >> log.txt
)

::720p images, 5x5 filters
echo "720p images"
@ECHO off
gpu_basic.exe 720p/snail.jpg box5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Box blur 5x5			: Passed in %VAR% seconds.
    echo Box blur 5x5			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Box blur 5x5: Failed
	echo Box blur 5x5: Failed >> log.txt
)
gpu_basic.exe 720p/green.jpg diag_motion5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Diagonal motion blur 5x5	: Passed in %VAR% seconds.
   echo Diagonal motion blur 5x5	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Diagonal motion blur 5x5: Failed
	echo Diagonal motion blur 5x5: Failed >> log.txt
)
gpu_basic.exe 720p/blue.jpg sharpen5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Sharpen 5x5			: Passed in %VAR% seconds.
   echo Sharpen 5x5			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Sharpen 5x5: Failed
	echo Sharpen 5x5: Failed >> log.txt
)
gpu_basic.exe 720p/ruins.jpg hor_edge_detect5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Horizontal edge detection 5x5	: Passed in %VAR% seconds.
   echo Horizontal edge detection 5x5	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Horizontal edge detection 5x5: Failed
	echo Horizontal edge detection 5x5: Failed >> log.txt
)


::1080p images, 5x5 filters
echo "1080p images"
@ECHO off
gpu_basic.exe 1080p/snail.jpg box5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Box blur 5x5			: Passed in %VAR% seconds.
   echo Box blur 5x5			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Box blur 5x5: Failed
	echo Box blur 5x5: Failed >> log.txt
)
gpu_basic.exe 1080p/green.jpg diag_motion5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Diagonal motion blur 5x5	: Passed in %VAR% seconds.
   echo Diagonal motion blur 5x5	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Diagonal motion blur 5x5: Failed
	echo Diagonal motion blur 5x5: Failed >> log.txt
)
gpu_basic.exe 1080p/blue.jpg sharpen5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Sharpen 5x5			: Passed in %VAR% seconds.
   echo Sharpen 5x5			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Sharpen 5x5: Failed
	echo Sharpen 5x5: Failed >> log.txt
)
gpu_basic.exe 1080p/ruins.jpg hor_edge_detect5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Horizontal edge detection 5x5	: Passed in %VAR% seconds.
   echo Horizontal edge detection 5x5	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Horizontal edge detection 5x5: Failed
	echo Horizontal edge detection 5x5: Failed >> log.txt
)
	
::1440p  images, 5x5 filters
echo "1440p images"
@ECHO off
gpu_basic.exe 1440p/snail.jpg box5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Box blur 5x5			: Passed in %VAR% seconds.
   echo Box blur 5x5			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Box blur 5x5: Failed
	echo Box blur 5x5: Failed >> log.txt
)
gpu_basic.exe 1440p/green.jpg diag_motion5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Diagonal motion blur 5x5	: Passed in %VAR% seconds.
   echo Diagonal motion blur 5x5	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Diagonal motion blur 5x5: Failed
	echo Diagonal motion blur 5x5: Failed >> log.txt
)
gpu_basic.exe 1440p/blue.jpg sharpen5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Sharpen 5x5			: Passed in %VAR% seconds.
   echo Sharpen 5x5			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Sharpen 5x5: Failed
	echo Sharpen 5x5: Failed >> log.txt
)
gpu_basic.exe 1440p/ruins.jpg hor_edge_detect5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Horizontal edge detection 5x5	: Passed in %VAR% seconds.
   echo Horizontal edge detection 5x5	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Horizontal edge detection 5x5: Failed
	echo Horizontal edge detection 5x5: Failed >> log.txt
)

::4K  images, 5x5 filters
echo "4k images"
@ECHO off
gpu_basic.exe 4k/snail.jpg box5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Box blur 5x5			: Passed in %VAR% seconds.
   echo Box blur 5x5			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Box blur 5x5: Failed
	echo Box blur 5x5: Failed >> log.txt
)
gpu_basic.exe 4k/green.jpg diag_motion5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Diagonal motion blur 5x5	: Passed in %VAR% seconds.
   echo Diagonal motion blur 5x5	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Diagonal motion blur 5x5: Failed
	echo Diagonal motion blur 5x5: Failed >> log.txt
)
gpu_basic.exe 4k/blue.jpg sharpen5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Sharpen 5x5			: Passed in %VAR% seconds.
   echo Sharpen 5x5			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Sharpen 5x5: Failed
	echo Sharpen 5x5: Failed >> log.txt
)
gpu_basic.exe 4k/ruins.jpg hor_edge_detect5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Horizontal edge detection 5x5	: Passed in %VAR% seconds.
   echo Horizontal edge detection 5x5	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Horizontal edge detection 5x5: Failed
	echo Horizontal edge detection 5x5: Failed >> log.txt
)

::8K  images, 5x5 filters
echo "8k images"
@ECHO off
gpu_basic.exe 8k/snail.jpg box5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Box blur 5x5			: Passed in %VAR% seconds.
   echo Box blur 5x5			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Box blur 5x5: Failed
	echo Box blur 5x5: Failed >> log.txt
)
gpu_basic.exe 8k/green.jpg diag_motion5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Diagonal motion blur 5x5	: Passed in %VAR% seconds.
   echo Diagonal motion blur 5x5	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Diagonal motion blur 5x5: Failed
	echo Diagonal motion blur 5x5: Failed >> log.txt
)
gpu_basic.exe 8k/blue.jpg sharpen5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Sharpen 5x5			: Passed in %VAR% seconds.
   echo Sharpen 5x5			: Passed in %VAR% seconds. >> log.txt
) else (
	echo Sharpen 5x5: Failed
	echo Sharpen 5x5: Failed >> log.txt
)
gpu_basic.exe 8k/ruins.jpg hor_edge_detect5x5 test2.jpg 3 > batchRunResults.txt
set /p VAR=<batchRunResults.txt
::echo "errorlevel is " + %errorlevel%
if %errorlevel% == 0 (
   echo Horizontal edge detection 5x5	: Passed in %VAR% seconds.
   echo Horizontal edge detection 5x5	: Passed in %VAR% seconds. >> log.txt
) else (
	echo Horizontal edge detection 5x5: Failed	
	echo Horizontal edge detection 5x5: Failed >> log.txt
)