# TL;DR

A simple DEMO for calculating simple VRP, which includes sending request for getting distance matrix, calculating in local, and passing result to show the routes.

# Process Flow

1. Build server (currently in google could platform): 
   - package: FastAPI
   - code: python
   - connect: asliu876216@35.194.198.57

2. From c++ program, send request to remote server to get `distance_matrix`
    - package: cpr
    - code: c++ 
    - cpr build-up workflow:
    
    1. Add a ```buiild``` folder inside ```dependencies/cpr```
       ```bash
       cd cpr && mkdir build && cd build
       ```
    2. run CMake
       ```bash
       cmake .. -DCPR_USE_SYSTEM_CURL=OFF -DCPR_BUILD_TESTS=OFF
       ```
    3. Compile
       ```bash
       cmake --build . --config Release
       ```
    4. Add include path into ```.vscode/c_cpp_properties.json```
       ```bash
       press  Ctrl+Shift+P
       select C/C++: Edit Configurations (UI)
       find   "Include path"
       add path for cpr and curl
       ```
       check ```.vscode/c_cpp_properties.json```
       ```json
       "includePath": [
           "${workspaceFolder}/dependencies/cpr/include",
           "${workspaceFolder}/dependencies/cpr/build/_deps/curl-src/include"
           "${workspaceFolder}/dependencies/cpr/build/cpr_generated_includes"
       ],
       ```

3. c++ program get the result from remote server
4. calculate
5. From c++ program pass `save-routes` to local server
6. dynamically load routes in html
    - package: Leaflet

# Requirement

1. CUDA
2. glog
3. nlohmann_json
4. FastAPI
5. cpr
6. Leaflet

# CMake Build Flow

1. make and enter ```build``` folder
    ```bash
    mkdir build
    cd build
    ```
2. run CMake
    ```bash
    cmake .. -DCMAKE_BUILD_TYPE=Release
    ```
3. Compile
    ```bash
    cmake --build . --config Release
    ```
4. Run the program
    ```bash
    ./Release/main.exe
    ```


# For Developer

## How to add dependencies as submodule

1. Add repo into dependencies folder

    ```bash
      git submodule add <repo url> <folder>
    ```

    ex:
    ```bash
      git submodule add https://github.com/nlohmann/json.git dependencies/json
    ```

2. Commit

    ```bash
      git commit -m "Add <some module> as dependency"
    ```

## First time clone repo into local

  ```bash
    git clone --recursive <repo url> <folder>
  ```

  ex:
  ```bash
    git clone --recursive git@github.com:6CE-TW/SimpleVRP.git
  ```

## When new submodule added at some feature, How to active it?

  Run `git submodule init` at project root

  ```bash
    cd <project root>
    git submodule init
  ```

## Update all submodule

  ```bash
    git submodule update
    <or>
    git submodule foreach --recursive git pull origin master
  ```

## Update specific submodule

  ```bash
    cd dependencies/<module name>
    git pull
  ```

  ex:
  ```bash
    cd dependencies/json
    git pull
  ```

## Checkout submodule version
  ```bash
    cd dependencies/<module name>
    git fetch
    git checkout -b <new branch name> <tag name>
  ```

  ex:
  ```bash
    cd dependencies/json
    git fetch
    git checkout tags/v3.10.4 -b v3.10.4
  ```

## How to disable auto format for some block
  Using `// clang-format off` to mark start disable and `// clang-format on` to mark end.

  Vscode formatter engine have to setting to `clangFormat`. check `.vscode/settings.json`.

  ```json
    {
      "C_Cpp.formatting": "clangFormat"
    }
  ```

  ex:
  ```c++
    inline const std::string LocalSearchDescription(const LocalSearchType &local_search_type)
    {
      // clang-format off
      switch (local_search_type)
      {
      case LocalSearchType::TWO_OPT:                return "TWO_OPT";
      case LocalSearchType::OR_OPT:                 return "OR_OPT";
      case LocalSearchType::THREE_OPT:              return "THREE_OPT";
      case LocalSearchType::RELOCATE_SAME_VEHICLE:  return "RELOCATE_SAME_VEHICLE";
      case LocalSearchType::RELOCATE_DIFF_VEHICLE:  return "RELOCATE_DIFF_VEHICLE";
      case LocalSearchType::EXCHANGE:               return "EXCHANGE";
      case LocalSearchType::CROSS:                  return "CROSS";
      case LocalSearchType::UNKNOWN:                
      default:                                      return "UNKNOWN";
      }
      // clang-format on
    };
  ```