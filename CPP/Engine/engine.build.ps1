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

$LinkArgs = @("-mconsole")
$LibDir = Join-Path $ProjRoot "C\lib\bin"
if (Test-Path $LibDir) {
    $LinkArgs = @("-L$LibDir") + $LinkArgs
    # C/00_build.ps1 과 동일: SDL2 + Windows 시스템 라이브러리 (방법2 정적 링크)
    $LinkArgs += "-lSDL2"
    # OpenGL3 렌더러 (Gl3Renderer): epoxy + opengl32
    $LinkArgs += "-lepoxy", "-lopengl32"
    $LinkArgs += "-lwinmm", "-lws2_32", "-lole32", "-loleaut32", "-lsetupapi", "-limm32", "-lgdi32", "-luser32", "-ladvapi32", "-lshell32", "-lversion", "-lshlwapi", "-lmingw32"
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
