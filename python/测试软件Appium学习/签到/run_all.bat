@echo off

rem Set path to the Python executable
set PYTHON=C:\Users\xcs\AppData\Local\Microsoft\WindowsApps\PythonSoftwareFoundation.Python.3.10_qbz5n2kfra8p0\python.exe

rem Run each Python script in the current directory
for %%f in (*.py) do (
    echo Running %%f
    %PYTHON% "%%f"
)

pause