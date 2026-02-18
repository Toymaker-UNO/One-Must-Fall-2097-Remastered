# Video module build: Utils, Formats, ExternalLibrary (SDL2) dependent
$ErrorActionPreference = "Stop"
$Root = $PSScriptRoot
$ProjRoot = (Resolve-Path (Join-Path $Root "..\..") -ErrorAction SilentlyContinue).Path
if (-not $ProjRoot) { $ProjRoot = (Get-Location).Path }

$ExtLib = Join-Path $Root "..\ExternalLibrary"
$ExtInclude = Join-Path $ExtLib "bin\include"
$LibDir = Join-Path $ProjRoot "C\lib\bin"
if (-not (Test-Path $LibDir)) { $LibDir = Join-Path $ExtLib "bin" }
$Utils = Join-Path $Root "..\Utils"
$Formats = Join-Path $Root "..\Formats"
$Out = Join-Path $Root "video.test.exe"

$Cxx = "g++"
$IncludeArgs = @(
    "-std=c++17",
    "-mconsole",
    "-Wl,-subsystem,console",
    "-I", $Root,
    "-I", (Join-Path $Root ".."),
    "-I", $Utils,
    "-I", $Formats,
    "-I", $ExtLib,
    "-I", $ExtInclude
)
$LinkArgs = @(
    "-L$LibDir",
    "-lSDL2",
    "-lshlwapi",
    "-lwinmm",
    "-lws2_32",
    "-lole32",
    "-loleaut32",
    "-lsetupapi",
    "-limm32",
    "-lgdi32",
    "-luser32",
    "-ladvapi32",
    "-lshell32",
    "-lversion",
    "-lmingw32",
    "-mconsole"
)

Get-ChildItem -Path $Root -Filter "*.o" -File -ErrorAction SilentlyContinue | Remove-Item -Force
Write-Host "Building Video module (test main)..."
$TestMain = Join-Path $Root "video.test.main.cpp"
& $Cxx $IncludeArgs -c $TestMain -o (Join-Path $Root "video.test.main.o")
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& $Cxx (Join-Path $Root "video.test.main.o") $LinkArgs -o $Out
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
if (Test-Path (Join-Path $Root "video.test.main.o")) { Remove-Item (Join-Path $Root "video.test.main.o") -Force }
Write-Host "Done. Running test: $Out"
& $Out
if ($LASTEXITCODE -ne 0) {
    Write-Host "Test failed with exit code $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}
Write-Host "Video build and test verification passed." -ForegroundColor Green
