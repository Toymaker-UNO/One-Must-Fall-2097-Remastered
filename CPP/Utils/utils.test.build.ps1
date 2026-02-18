# Utils 모듈 빌드 (ExternalLibrary와 동일 패턴, Log/Config에서 SDL2·libconfuse 링크)
$ErrorActionPreference = "Stop"
$Root = $PSScriptRoot
$ProjRoot = (Resolve-Path (Join-Path $Root "..\..") -ErrorAction SilentlyContinue).Path
if (-not $ProjRoot) { $ProjRoot = (Get-Location).Path }

$ExtLib = Join-Path $Root "..\ExternalLibrary"
$ExtInclude = Join-Path $ExtLib "bin\include"
$LibDir = Join-Path $ProjRoot "C\lib\bin"
if (-not (Test-Path $LibDir)) { $LibDir = Join-Path $ExtLib "bin" }

$IncludeUtils = $Root
$Out = Join-Path $Root "utils.test.exe"

$Cxx = "g++"
$IncludeArgs = @(
    "-std=c++17",
    "-mconsole",
    "-Wl,-subsystem,console",
    "-I", $IncludeUtils,
    "-I", $ExtLib,
    "-I", $ExtInclude
)
# SDL2가 필요로 하는 Windows 시스템 라이브러리 (C/00_build.ps1과 동일)
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

# 기존 .o 파일 제거 (이전 빌드 잔여물 방지)
Get-ChildItem -Path $Root -Filter "*.o" -File -ErrorAction SilentlyContinue | Remove-Item -Force

Write-Host "Building Utils module (sources + test main)..."
$TestMain = Join-Path $Root "utils.test.main.cpp"
& $Cxx $IncludeArgs -c $TestMain -o (Join-Path $Root "utils.test.main.o")
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
$ObjFiles = @((Join-Path $Root "utils.test.main.o"))

& $Cxx $ObjFiles $LinkArgs -o $Out
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

# main 오브젝트 파일은 빌드 후 정리
$MainObj = Join-Path $Root "utils.test.main.o"
if (Test-Path $MainObj) {
    Remove-Item $MainObj -Force
}

Write-Host "Done. Run: $Out"
