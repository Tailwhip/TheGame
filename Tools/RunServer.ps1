$ErrorActionPreference = "Stop"

$VenvPath = Join-Path (Get-Location) ".venv"
function Test-VenvExists {
    param(
        [string]$VenvPath = ".venv"
    )

    if (Test-Path $VenvPath) {
        return $true
    }

    return $false
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
        Write-Host "Running server..."
        if (-not (Test-VenvExists)) {
            throw "Virtual environment missing! Run .\Tools\ProjSetup.ps1 first!"
        }

        Activate-Venv

        agent_trainer.exe
    }
    catch {
        Write-Host "Running failed: $_" -ForegroundColor Red
        exit 1
    }
}

Main
