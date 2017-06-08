::@ECHO off

@echo off
echo "480p images">>TEST_RESULTS.txt
echo "3x3 box blur">>TEST_RESULTS.txt 
echo "----------------------------------------------------"
echo "			------CPU-single--------"
@ECHO off
for /l %%x in (1, 1, 100) do (

	gpu_advanced.exe 480p/snail.jpg CPU-single box3x3 test3.jpg 1 0 >> single.txt
)


REM for /l %%x in (1, 1, 100) do (

	REM gpu_basic.exe 480p/snail.jpg GPU box3x3 test1.jpg 1 0 >> basic1.txt
	REM REM ::set /p var=<batchRunResults.txt
		REM REM ::if %errorlevel% == 0 (
		   REM REM echo run nr %%x took %var% seconds.
		   REM REM echo %var% >> basic_tests.txt
		REM REM )else(
		REM REM )
	
REM )

for /l %%x in (1, 1, 1) do (

	gpu_advanced.exe 480p/snail.jpg GPU box3x3 test2.jpg 1 0 >> advanced1.txt

)

REM for /l %%x in (1, 1, 100) do (

	REM gpu_advanced.exe 480p/snail.jpg CPU box3x3 test3.jpg 1 0 >> advanced2.txt

REM )

