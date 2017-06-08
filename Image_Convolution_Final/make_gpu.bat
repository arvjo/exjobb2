@ECHO OFF
 
g++ -std=c++11 gpu_basic.cpp Filters.cpp -lgdi32 -I. C:\Windows\System32\OpenCL.DLL -o gpu_basic.exe 
@ECHO "script done"