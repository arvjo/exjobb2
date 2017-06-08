@ECHO OFF
 
g++ -std=c++11 cpu.cpp -lgdi32 -I. C:\Windows\System32\OpenCL.DLL -o cpu.exe 
@ECHO "script done"