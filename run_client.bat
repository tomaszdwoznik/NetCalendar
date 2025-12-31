@echo off

setlocal enabledelayedexpansion

set "VENV_DIR=client\.venv"
set "REQ_FILE=client\requirements.txt"
set "APP_PATH=client\app/main.py"

title Uruchamianie Aplikacji

python --version >nul 2>&1
if errorlevel 1 (
  echo [ERROR] Nie znaleziono interpretera Python.
  echo Pobierz go z https://www.python.org/downloads/
  goto :error
)

if not exist "%VENV_DIR%" (
  echo [INFO] Tworzenie srodowiska wirtualnego w %VENV_DIR%...
  python -m venv %VENV_DIR%
  if errorlevel 1 (
    echo [ERROR] Nie udalo sie utworzyc srodowiska venv.
    goto :error
  )
  timeout /t 1 >nul
)

set "ACTIVATE_SCRIPT=%VENV_DIR%\Scripts\activate.bat"
if not exist "!ACTIVATE_SCRIPT!" (
  set "ACTIVATE_SCRIPT=%VENV_DIR%\bin\activate.bat"
)

if not exist "!ACTIVATE_SCRIPT!" (
  echo [ERROR] Nie znaleziono skryptu aktywacji w %VENV_DIR%.
  goto :error
)

call "!ACTIVATE_SCRIPT!"
echo [INFO] Sprawdzanie i instalacja zaleznosci...
pip install -q --disable-pip-version-check -r %REQ_FILE%

echo [INFO] Uruchamianie aplikacji: %APP_PATH%
python %APP_PATH%

if errorlevel 1 (
  echo.
  echo [ERROR] Aplikacja zostala zamknieta z kodem bledu.
  goto :error
)

exit /b 0

:error
echo.
echo [INFO] Nacisnij dowolny klawisz, aby zamknac to okno.
pause >nul
exit /b 1