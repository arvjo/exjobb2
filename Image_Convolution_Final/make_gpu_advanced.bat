@ECHO OFF
 
g++ -std=c++11 gpu_advanced.cpp Filters.cpp -lgdi32 -I. C:\Windows\System32\OpenCL.DLL -o gpu_advanced.exe 
@ECHO "script done"