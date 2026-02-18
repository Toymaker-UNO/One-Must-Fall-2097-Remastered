# Formats module build: Utils dependent (Reader/Writer use Io, Str, Allocator -> Log -> SDL2)
$ErrorActionPreference = "Stop"
$Root = $PSScriptRoot
$ProjRoot = (Resolve-Path (Join-Path $Root "..\..") -ErrorAction SilentlyContinue).Path
if (-not $ProjRoot) { $ProjRoot = (Get-Location).Path }

$ExtLib = Join-Path $Root "..\ExternalLibrary"
$ExtInclude = Join-Path $ExtLib "bin\include"
$LibDir = Join-Path $ProjRoot "C\lib\bin"
if (-not (Test-Path $LibDir)) { $LibDir = Join-Path $ExtLib "bin" }
$Utils = Join-Path $Root "..\Utils"
$Out = Join-Path $Root "formats.test.exe"

$Cxx = "g++"
$IncludeArgs = @(
    "-std=c++17",
    "-mconsole",
    "-Wl,-subsystem,console",
    "-I", $Root,
    "-I", $Utils,
    "-I", $ExtLib,
    "-I", $ExtInclude
)
$LinkArgs = @(
    "-L$LibDir",
    "-lSDL2",
    "-lconfuse",
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
Write-Host "Building Formats module (test main)..."
$TestMain = Join-Path $Root "formats.test.main.cpp"
& $Cxx $IncludeArgs -c $TestMain -o (Join-Path $Root "formats.test.main.o")
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& $Cxx (Join-Path $Root "formats.test.main.o") $LinkArgs -o $Out
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
if (Test-Path (Join-Path $Root "formats.test.main.o")) { Remove-Item (Join-Path $Root "formats.test.main.o") -Force }
Write-Host "Done. Run: $Out"
