@echo off
set CUDA_PATH=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.8

set COMPILER="%CUDA_PATH%\bin\nvcc.exe"
set FLAGS= -I. -std=c++17

set OUTPUT_PATH=output
set SOURCE_FILE=main.cc data/data.cc algorithm/solver.cc algorithm/initial_solution/initial_solution.cc algorithm/local_search/local_search.cc

if not exist %OUTPUT_PATH% mkdir %OUTPUT_PATH%
echo Compiling...
%COMPILER% %FLAGS% %SOURCE_FILE% -o %OUTPUT_PATH%\main.exe
