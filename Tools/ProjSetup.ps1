$ErrorActionPreference = "Stop"

$PythonVersion = "3.14"
$VenvPath = Join-Path (Get-Location) ".venv"

function Get-PythonCommand {
    $Python = Get-Command "python$PythonVersion" -ErrorAction SilentlyContinue

    if ($Python) {
        return @{
            Command = $Python.Source
            Arguments = @()
        }
    }

    # Fall back to Windows Python Launcher
    $Launcher = Get-Command "py" -ErrorAction SilentlyContinue

    if ($Launcher) {
        return @{
            Command = $Launcher.Source
            Arguments = @("-$PythonVersion")
        }
    }

    throw "Python $PythonVersion was not found!"
}

function Test-VenvExists {
    param(
        [string]$VenvPath = ".venv"
    )

    if (Test-Path $VenvPath) {
        return $true
    }

    return $false
}

function New-Venv {
    param(
        [Parameter(Mandatory)]
        [hashtable]$Python
    )

    Write-Host "Creating virtual environment using Python $PythonVersion..."

    & $Python.Command @($Python.Arguments) -m venv $VenvPath

    if ($LASTEXITCODE -ne 0) {
        throw "Failed to create virtual environment."
    }

    Write-Host "Virtual environment created successfully."
}

function Update-Pip {
    $PythonExe = Join-Path $VenvPath "Scripts\python.exe"

    Write-Host "Upgrading pip..."

    & $PythonExe -m pip install --upgrade pip

    if ($LASTEXITCODE -ne 0) {
        throw "Failed to upgrade pip."
    }

    Write-Host "pip upgraded successfully."
}

function Activate-Venv {
    param(
        [string]$VenvPath = ".venv"
    )

    $ActivateScript = Join-Path $VenvPath "Scripts\Activate.ps1"

    if (-not (Test-Path $ActivateScript)) {
        throw "Virtual environment not found: $ActivateScript"
    }

    . $ActivateScript

    Write-Host "Virtual environment activated." -ForegroundColor Green
}

function Main {
    try {
        Write-Host "Starting project setup..."
        if (Test-VenvExists) {
            Write-Host "Virtual environment already exists..."
        }
        else {
            Write-Host "Creating new virtual environment..."
            $Python = Get-PythonCommand
            New-Venv -Python $Python
        }
        Update-Pip
        Activate-Venv
        pip install .\Source\Python
    }
    catch {
        Write-Host "Setup failed: $_" -ForegroundColor Red
        exit 1
    }
}

Main
