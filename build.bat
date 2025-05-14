@echo off
set CUDA_PATH=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.8
set OUTPUT_PATH=output

if not exist %OUTPUT_PATH% mkdir %OUTPUT_PATH%
echo Compiling...
"%CUDA_PATH%\bin\nvcc.exe" main.cc data.cc solver.cc -std=c++17 -o %OUTPUT_PATH%\main.exe