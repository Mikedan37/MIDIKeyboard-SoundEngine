# QWERTY MIDI Keyboard - Setup Script for Windows
# Installs dependencies and sets up auto-launch

Write-Host "QWERTY MIDI Keyboard - Setup" -ForegroundColor Cyan
Write-Host "================================" -ForegroundColor Cyan
Write-Host ""

# Check Python
try {
    $pythonVersion = python --version 2>&1
    Write-Host "$pythonVersion found" -ForegroundColor Green
} catch {
    Write-Host "ERROR: Python is required but not installed." -ForegroundColor Red
    Write-Host "Please install Python 3 from https://www.python.org/" -ForegroundColor Yellow
    exit 1
}

Write-Host ""

# Setup Python environment
Write-Host "Setting up Python environment..." -ForegroundColor Cyan
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$venvPath = Join-Path $scriptDir "midi_sound_engine\venv"

if (-Not (Test-Path $venvPath)) {
    python -m venv $venvPath
    Write-Host "Created virtual environment" -ForegroundColor Green
} else {
    Write-Host "Virtual environment already exists" -ForegroundColor Green
}

# Activate virtual environment
& "$venvPath\Scripts\Activate.ps1"
python -m pip install --upgrade pip --quiet
pip install -r (Join-Path $scriptDir "midi_sound_engine\requirements.txt")
Write-Host "Python dependencies installed" -ForegroundColor Green
Write-Host ""

# Setup firmware build (if CMake available)
$firmwareDir = Join-Path $scriptDir "qwerty_midi_pico"
if (Get-Command cmake -ErrorAction SilentlyContinue) {
    Write-Host "Setting up firmware build environment..." -ForegroundColor Cyan
    $buildDir = Join-Path $firmwareDir "build"
    if (-Not (Test-Path $buildDir)) {
        New-Item -ItemType Directory -Path $buildDir | Out-Null
    }
    Write-Host "Firmware build environment ready" -ForegroundColor Green
    Write-Host "   Run 'cd qwerty_midi_pico\build && cmake .. && cmake --build .' to build" -ForegroundColor Yellow
} else {
    Write-Host "WARNING: CMake not found - firmware build skipped" -ForegroundColor Yellow
    Write-Host "   Install CMake to build firmware" -ForegroundColor Yellow
}
Write-Host ""

# Setup Windows Task Scheduler
Write-Host "Setting up auto-launch..." -ForegroundColor Cyan

$pythonExe = Join-Path $venvPath "Scripts\python.exe"
$monitorScript = Join-Path $scriptDir "midi_sound_engine\monitor_and_launch.py"
$taskName = "QWERTY_MIDI_Keyboard"

# Remove existing task if it exists
Unregister-ScheduledTask -TaskName $taskName -ErrorAction SilentlyContinue

# Create scheduled task
$action = New-ScheduledTaskAction -Execute $pythonExe -Argument "`"$monitorScript`""
$trigger = New-ScheduledTaskTrigger -AtLogOn
$settings = New-ScheduledTaskSettingsSet -AllowStartIfOnBatteries -DontStopIfGoingOnBatteries -StartWhenAvailable -RestartCount 3 -RestartInterval (New-TimeSpan -Minutes 1)
$principal = New-ScheduledTaskPrincipal -UserId $env:USERNAME -LogonType Interactive -RunLevel Highest

Register-ScheduledTask -TaskName $taskName -Action $action -Trigger $trigger -Settings $settings -Principal $principal -Description "QWERTY MIDI Keyboard Auto-Launch" | Out-Null

Write-Host "Windows Task Scheduler configured" -ForegroundColor Green
Write-Host "   Task: $taskName" -ForegroundColor Yellow
Write-Host "   Manage with: Get-ScheduledTask -TaskName $taskName" -ForegroundColor Yellow
Write-Host ""

Write-Host "Setup complete!" -ForegroundColor Green
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Cyan
Write-Host "  1. Flash firmware to Pico (see qwerty_midi_pico\README.md)" -ForegroundColor White
Write-Host "  2. Plug in your Pico" -ForegroundColor White
Write-Host "  3. The synthesizer will auto-launch on login!" -ForegroundColor White
Write-Host ""
Write-Host "Manual start:" -ForegroundColor Cyan
Write-Host "  cd midi_sound_engine && .\venv\Scripts\Activate.ps1 && python monitor_and_launch.py" -ForegroundColor White

