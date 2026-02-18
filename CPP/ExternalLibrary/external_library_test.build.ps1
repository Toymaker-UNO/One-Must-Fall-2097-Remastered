# ExternalLibrary 테스트 빌드 (최소 구성)
# 아무 위치에서 실행 가능: .\CPP\ExternalLibrary\build_test.ps1

$ErrorActionPreference = "Stop"
$Root = $PSScriptRoot
# ExternalLibrary 폴더 기준으로 프로젝트 루트 = 상위 두 단계
$ProjRoot = (Resolve-Path (Join-Path $Root "..\..") -ErrorAction SilentlyContinue).Path
if (-not $ProjRoot) { $ProjRoot = (Get-Location).Path }

$Src = Join-Path $Root "external_library_test.main.cpp"
$IncludeHpp = $Root
$IncludeLibs = Join-Path $Root "bin\include"
$IncludeVendored = Join-Path $ProjRoot "C\src\vendored"
$Out = Join-Path $Root "external_library_test.exe"

$Cxx = "g++"
$Args = @(
    "-std=c++17",
    "-mconsole",
    "-Wl,-subsystem,console",
    "-I", $IncludeHpp,
    "-I", $IncludeLibs,
    "-I", $IncludeVendored,
    $Src,
    "-o", $Out
)

Write-Host "Building: $Src"
Write-Host "Include (hpp): $IncludeHpp"
Write-Host "Include (libs): $IncludeLibs"
Write-Host "Include (vendored): $IncludeVendored"
Write-Host "Output: $Out"
& $Cxx $Args
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Write-Host "Done. Run: $Out"
