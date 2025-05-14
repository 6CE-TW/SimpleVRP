@echo off
set CUDA_PATH=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.8

set COMPILER="%CUDA_PATH%\bin\nvcc.exe"
set FLAGS= -I. -std=c++17

set OUTPUT_PATH=output
set SOURCE_FILE=main.cc data.cc solver.cc initial_solution/initial_solution.cc

if not exist %OUTPUT_PATH% mkdir %OUTPUT_PATH%
echo Compiling...
%COMPILER% %FLAGS% %SOURCE_FILE% -o %OUTPUT_PATH%\main.exe
