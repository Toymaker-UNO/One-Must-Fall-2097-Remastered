# Game module build: GameState skeleton
$ErrorActionPreference = "Stop"
$Root = $PSScriptRoot
$ProjRoot = (Resolve-Path (Join-Path $Root "..\..") -ErrorAction SilentlyContinue).Path
if (-not $ProjRoot) { $ProjRoot = (Get-Location).Path }

$Out = Join-Path $Root "game.test.exe"

$Cxx = "g++"
$IncludeArgs = @(
    "-std=c++17",
    "-mconsole",
    "-Wl,-subsystem,console",
    "-I", $Root,
    "-I", (Join-Path $Root "..")
)
$LinkArgs = @(
    "-lmingw32",
    "-mconsole"
)

Get-ChildItem -Path $Root -Filter "*.o" -File -ErrorAction SilentlyContinue | Remove-Item -Force
Write-Host "Building Game module (test main)..."
$TestMain = Join-Path $Root "game.test.main.cpp"
& $Cxx $IncludeArgs -c $TestMain -o (Join-Path $Root "game.test.main.o")
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& $Cxx (Join-Path $Root "game.test.main.o") $LinkArgs -o $Out
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
if (Test-Path (Join-Path $Root "game.test.main.o")) { Remove-Item (Join-Path $Root "game.test.main.o") -Force }
Write-Host "Done. Running test: $Out"
& $Out
if ($LASTEXITCODE -ne 0) {
    Write-Host "Test failed with exit code $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}
Write-Host "Game build and test verification passed." -ForegroundColor Green

