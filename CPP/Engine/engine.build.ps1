# Engine + main: 단일 실행 파일(omf.exe) 빌드
# 의존: Utils, Formats, Resources, Video, Audio, Controller, Console, Game (헤더만 사용)
$ErrorActionPreference = "Stop"
$Root = $PSScriptRoot
$ProjRoot = (Resolve-Path (Join-Path $Root "..\..") -ErrorAction SilentlyContinue).Path
if (-not $ProjRoot) { $ProjRoot = (Get-Location).Path }

$CPP = Join-Path $Root ".."
$Out = Join-Path $Root "omf.exe"

$Cxx = "g++"
$IncludeDirs = @(
    $Root,
    $CPP,
    (Join-Path $CPP "Utils"),
    (Join-Path $CPP "Formats"),
    (Join-Path $CPP "Game"),
    (Join-Path $CPP "Video"),
    (Join-Path $CPP "Audio"),
    (Join-Path $CPP "Controller"),
    (Join-Path $CPP "Console"),
    (Join-Path $CPP "Resources"),
    (Join-Path $CPP "Resources\hpp"),
    (Join-Path $CPP "ExternalLibrary"),
    (Join-Path $CPP "ExternalLibrary\bin\include")
)
$IncludeArgs = @()
foreach ($d in $IncludeDirs) {
    $IncludeArgs += "-I"
    $IncludeArgs += $d
}
$CompileArgs = @(
    "-std=c++17",
    "-mconsole",
    "-Wl,-subsystem,console",
    "-DSDL_MAIN_HANDLED"
) + $IncludeArgs

$LinkArgs = @(
    "-lmingw32",
    "-mconsole"
)
$LibDir = Join-Path $ProjRoot "C\lib\bin"
if (Test-Path $LibDir) {
    $LinkArgs = @("-L$LibDir") + $LinkArgs
    $LinkArgs += "-lSDL2"
    # SDL2 정적 링크 시 MinGW에서 필요한 Windows 시스템 라이브러리
    $LinkArgs += "-lsetupapi", "-limm32", "-lole32", "-loleaut32", "-lversion", "-lgdi32", "-luser32", "-lwinmm", "-lhid", "-ldxguid", "-lshell32", "-lcomdlg32", "-lopengl32", "-lpropsys", "-lbcrypt"
}

Write-Host "Building Engine (omf.exe)..."
& $Cxx $CompileArgs -c (Join-Path $Root "main.cpp") -o (Join-Path $Root "main.o")
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& $Cxx (Join-Path $Root "main.o") $LinkArgs -o $Out
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
if (Test-Path (Join-Path $Root "main.o")) { Remove-Item (Join-Path $Root "main.o") -Force }
Write-Host "Done. Output: $Out"
& $Out
$exitCode = $LASTEXITCODE
if ($exitCode -ne 0) { Write-Host "Exit code: $exitCode" -ForegroundColor Yellow }
Write-Host "Engine build and run completed." -ForegroundColor Green
