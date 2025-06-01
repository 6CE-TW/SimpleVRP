## How to run server in windows

1. build folder server
    ```bash
    mkdir server
    cd server
    ```

2. establish virtual environment and 
    ```powershell
    python -m venv venv
    Set-ExecutionPolicy RemoteSigned -Scope CurrentUser
    ```

    ```RemoteSigned```: PowerShell script created locally can be executed, script downloaded from internet must be digitally signed before execution. \
    ```-Scope CurrentUser```: change the setting only for the current user, and does not affect the whole system.

3. run virtual environment
    ```bash
    venv\Scripts\activate
    ```
    or
    ```powershell
    .\venv\Scripts\Activate.ps1
    ```

4. install fastapi uvicorn and requests
    ```bash
    pip install fastapi uvicorn[standard] requests
    ```

5. activate the server
    ```bash
    uvicorn server:app --host 0.0.0.0 --port 8000
    ```
    can use other port if port 8000 is being used

    check the following:
    1. Make sure Windows Firewall is not blocking ```Python``` or ```uvicorn``` from accepting connections.
    2. If a prompt appears saying “Allow this app to communicate on the network,” be sure to click “Allow”.

    check if ```Python``` or ```uvicorn``` is in the white list

    1. Open the Windows search bar and type “Firewall”.
    2. Select “Allow an app through Windows Firewall”.
    3. Click the “Change settings” button in the top right (requires administrator privileges).
    4. Scroll down and check if the following items are listed and checked:
        - python.exe
        - python3.exe
        - uvicorn.exe (if available)
    5. Make sure these are checked for Private networks (at minimum), like this:
        ```vbnet
        ☑ python.exe     ☑ Private   ☐ Public
        ```
    6. If they are not listed, click “Allow another app”, then browse and add your Python interpreter path, such as:
        ```vbnet
        venv\Scripts\python.exe
        ```
        or your system-wide Python installation path.

6. Verify that the server is working

    After starting uvicorn, open a browser and go to:
    http://127.0.0.1:8000

    You should see:
    ```json
    {"message": "Hello from FastAPI on Windows!"}
    ```

## Check if other program is using port 8000

1. Check the pid of the program
    ```powershell
    netstat -ano | findstr :8000
    ```
    The terminal will print result like this:
    ```
    TCP    127.0.0.1:8000         0.0.0.0:0              LISTENING       35964
    TCP    127.0.0.1:50085        127.0.0.1:8000         TIME_WAIT       0
    ```
    In this example, program with pid=35964 is using port 8000

2. Check the process name of the program

    ```powershell
    Get-Process -Id 35964
    ```

    ```
    Handles  NPM(K)    PM(K)      WS(K)     CPU(s)     Id  SI ProcessName
    -------  ------    -----      -----     ------     --  -- -----------
        385     106   115712      33848     137.17  35964   1 Code
    ```

3. Kill the process if needed
    ```powershell
    taskkill /PID 35964 /F
    ```

