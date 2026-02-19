# CPP 모듈별 빌드/컴파일 시험 — 의존 순서대로 실행
# 프로젝트 루트 또는 CPP 디렉터리에서 실행: .\CPP\00_build_all_modules.ps1

$ErrorActionPreference = "Stop"
$ScriptDir = $PSScriptRoot
$ProjRoot = (Resolve-Path (Join-Path $ScriptDir "..") -ErrorAction SilentlyContinue).Path
if (-not $ProjRoot) { $ProjRoot = (Get-Location).Path }

# 의존 순서: ExternalLibrary → Utils → Formats → Resources → Video → Audio → Controller → Console → Game → ...
$Modules = @(
    @{ Name = "ExternalLibrary"; Script = "external_library.test.build.ps1"; Dir = "ExternalLibrary" },
    @{ Name = "Utils";          Script = "utils.test.build.ps1";     Dir = "Utils" },
    @{ Name = "Formats";        Script = "formats.test.build.ps1";  Dir = "Formats" },
    @{ Name = "Resources";      Script = "resources.test.build.ps1"; Dir = "Resources" },
    @{ Name = "Video";         Script = "video.test.build.ps1";     Dir = "Video" },
    @{ Name = "Audio";         Script = "audio.test.build.ps1";     Dir = "Audio" },
    @{ Name = "Controller";    Script = "controller.test.build.ps1"; Dir = "Controller" },
    @{ Name = "Console";       Script = "console.test.build.ps1";    Dir = "Console" },
    @{ Name = "Game";          Script = "game.test.build.ps1";       Dir = "Game" },
    @{ Name = "Engine";        Script = "engine.build.ps1";          Dir = "Engine" }
)

$Failed = @()
foreach ($m in $Modules) {
    $Dir = Join-Path $ScriptDir $m.Dir
    $BuildScript = Join-Path $Dir $m.Script
    if (-not (Test-Path $BuildScript)) {
        Write-Host "Skip (no script): $($m.Name)" -ForegroundColor Yellow
        continue
    }
    Write-Host "=== Building module: $($m.Name) ===" -ForegroundColor Cyan
    Push-Location $Dir
    try {
        & (Join-Path $Dir $m.Script)
        if ($LASTEXITCODE -ne 0) {
            $Failed += $m.Name
        }
    } finally {
        Pop-Location
    }
}

if ($Failed.Count -gt 0) {
    Write-Host "Failed modules: $($Failed -join ', ')" -ForegroundColor Red
    exit 1
}
Write-Host "All modules built successfully." -ForegroundColor Green
