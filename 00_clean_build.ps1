# Simple Direct Build Script for One Must Fall 2097
Write-Host "=== C++ Build Started ===" -ForegroundColor Magenta

# Settings (layers and lib under CPP2 root; run script from CPP2)
$BUILD_DIR = "build"
$LAYER_ROOTS = @("LAYER_1_UI", "LAYER_2_Application", "LAYER_3_Domain", "LAYER_4_Infrastructure", "LAYER_5_Common")
$GAME_RESOURCES_DIR = "LAYER_4_Infrastructure/assets/game_resources"
$SHADERS_DIR = "LAYER_4_Infrastructure/assets/shaders"
$VCPKG_LIB_DIR = "LAYER_5_Common/lib/bin"
$VCPKG_INCLUDE_DIR = "LAYER_5_Common/lib/include"
$OUTPUT_NAME = "openomf.exe"
$PROJECT_ROOT = (Get-Location).Path

# Compiler: use C++ compiler for entire build (requirement)
$CXX = "g++"

# C flags
$CFLAGS = @()
$CFLAGS += "-std=c11"
$CFLAGS += "-O2"
$CFLAGS += "-w"
# Layer include order: UI -> Application -> Domain -> Infrastructure -> Common
$CFLAGS += "-ILAYER_1_UI"
$CFLAGS += "-ILAYER_2_Application"
$CFLAGS += "-ILAYER_3_Domain"
$CFLAGS += "-ILAYER_4_Infrastructure"
$CFLAGS += "-ILAYER_5_Common"
$CFLAGS += "-ILAYER_5_Common/vendored"
$CFLAGS += "-I$VCPKG_INCLUDE_DIR"
$CFLAGS += "-I$VCPKG_INCLUDE_DIR/SDL2"
$CFLAGS += "-DENABLE_SDL_AUDIO_BACKEND"
$CFLAGS += "-DENABLE_NULL_AUDIO_BACKEND"
$CFLAGS += "-DENABLE_OPENGL3_RENDERER"
$CFLAGS += "-DENABLE_NULL_RENDERER"
$CFLAGS += "-DSDL_MAIN_HANDLED"
$CFLAGS += "-DUSE_LIBPNG=1"
$CFLAGS += "-DPNG_FOUND=1"
$CFLAGS += "-DUSE_OPUSFILE=1"
$CFLAGS += "-DLIBXMP_STATIC"
$CFLAGS += "-DXMP_STATIC"
$CFLAGS += "-DV_MAJOR=0"
$CFLAGS += "-DV_MINOR=0"
$CFLAGS += "-DV_PATCH=0"
$CFLAGS += "-DPERROR=printf"
$CFLAGS += "-fpermissive"
# -Wno-jump-misses-init is C-only; omitted when using g++ for .c files
$CFLAGS += "-include"
$CFLAGS += "stdint.h"
$CFLAGS += "-include"
$CFLAGS += "stddef.h"
$CFLAGS += "-include"
$CFLAGS += "string.h"
$CFLAGS += "-include"
$CFLAGS += "stdbool.h"

# C++ flags
$CXXFLAGS = @()
$CXXFLAGS += "-std=c++17"
$CXXFLAGS += "-O2"
$CXXFLAGS += "-w"
$CXXFLAGS += "-ILAYER_1_UI"
$CXXFLAGS += "-ILAYER_2_Application"
$CXXFLAGS += "-ILAYER_3_Domain"
$CXXFLAGS += "-ILAYER_4_Infrastructure"
$CXXFLAGS += "-ILAYER_5_Common"
$CXXFLAGS += "-ILAYER_5_Common/vendored"
$CXXFLAGS += "-I$VCPKG_INCLUDE_DIR"
$CXXFLAGS += "-I$VCPKG_INCLUDE_DIR/SDL2"
$CXXFLAGS += "-DENABLE_SDL_AUDIO_BACKEND"
$CXXFLAGS += "-DENABLE_NULL_AUDIO_BACKEND"
$CXXFLAGS += "-DENABLE_OPENGL3_RENDERER"
$CXXFLAGS += "-DENABLE_NULL_RENDERER"
$CXXFLAGS += "-DSDL_MAIN_HANDLED"
$CXXFLAGS += "-DUSE_LIBPNG=1"
$CXXFLAGS += "-DPNG_FOUND=1"
$CXXFLAGS += "-DUSE_OPUSFILE=1"
$CXXFLAGS += "-DLIBXMP_STATIC"
$CXXFLAGS += "-DXMP_STATIC"
$CXXFLAGS += "-DV_MAJOR=0"
$CXXFLAGS += "-DV_MINOR=0"
$CXXFLAGS += "-DV_PATCH=0"
$CXXFLAGS += "-DPERROR=printf"

# Linker flags
$LDFLAGS = @()
$LDFLAGS += "-L$VCPKG_LIB_DIR"
$LDFLAGS += "-lSDL2"
$LDFLAGS += "-lSDL2_mixer"
$LDFLAGS += "-lxmp"
$LDFLAGS += "-lepoxy"
$LDFLAGS += "-lenet"
$LDFLAGS += "-lconfuse"
$LDFLAGS += "-lpng"
$LDFLAGS += "-lzlib"
$LDFLAGS += "-lopusfile"
$LDFLAGS += "-lopus"
$LDFLAGS += "-lvorbisfile"
$LDFLAGS += "-lvorbisenc"
$LDFLAGS += "-lvorbis"
$LDFLAGS += "-logg"
$LDFLAGS += "-lm"
$LDFLAGS += "-lwinmm"
$LDFLAGS += "-lws2_32"
$LDFLAGS += "-lole32"
$LDFLAGS += "-loleaut32"
$LDFLAGS += "-lsetupapi"
$LDFLAGS += "-limm32"
$LDFLAGS += "-lgdi32"
$LDFLAGS += "-luser32"
$LDFLAGS += "-ladvapi32"
$LDFLAGS += "-lshell32"
$LDFLAGS += "-lversion"
$LDFLAGS += "-lshlwapi"
$LDFLAGS += "-lmingw32"
$LDFLAGS += "-mconsole"

# Build dir
Write-Host "Cleaning build directory..." -ForegroundColor Yellow
if (Test-Path $BUILD_DIR) {
    Remove-Item -Recurse -Force $BUILD_DIR
}
New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null

# Collect sources from all layer roots (exclude lib and assets)
$layerDirs = $LAYER_ROOTS | ForEach-Object {
    $p = Join-Path $PROJECT_ROOT $_
    if (Test-Path $p) { $p }
}
Write-Host "Collecting source files..." -ForegroundColor Yellow
$C_FILES = $layerDirs | Get-ChildItem -Recurse -Filter "*.c" -ErrorAction SilentlyContinue | Where-Object { $_.FullName -notmatch "\\lib\\|\\assets\\" } | ForEach-Object { $_.FullName }
$CPP_FILES = $layerDirs | Get-ChildItem -Recurse -Filter "*.cpp" -ErrorAction SilentlyContinue | Where-Object { $_.FullName -notmatch "\\lib\\|\\assets\\" } | ForEach-Object { $_.FullName }
Write-Host "Found $($C_FILES.Count) C source files" -ForegroundColor Green
Write-Host "Found $($CPP_FILES.Count) C++ source files" -ForegroundColor Green

# Compile
Write-Host "Compiling..." -ForegroundColor Yellow
$OBJECT_FILES = @()

# Compile C files with C++ compiler (g++ treats .c as C source)
foreach ($source_file in $C_FILES) {
    $object_file = $source_file.Replace($PROJECT_ROOT, $BUILD_DIR) -replace "\.c$", ".o"
    $object_dir = Split-Path $object_file -Parent
    if (!(Test-Path $object_dir)) {
        New-Item -ItemType Directory -Path $object_dir -Force | Out-Null
    }

    $compile_args = $CFLAGS + @($source_file, "-c", "-o", $object_file)
    Write-Host "Compiling (C): $source_file..." -ForegroundColor Cyan
    & $CXX @compile_args
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Failed to compile $source_file" -ForegroundColor Red
        exit 1
    }
    $OBJECT_FILES += $object_file
}

# Compile C++ files
foreach ($source_file in $CPP_FILES) {
    $object_file = $source_file.Replace($PROJECT_ROOT, $BUILD_DIR) -replace "\.cpp$", ".o"
    $object_dir = Split-Path $object_file -Parent
    if (!(Test-Path $object_dir)) {
        New-Item -ItemType Directory -Path $object_dir -Force | Out-Null
    }

    $compile_args = $CXXFLAGS + @($source_file, "-c", "-o", $object_file)
    Write-Host "Compiling C++: $source_file..." -ForegroundColor Cyan
    & $CXX @compile_args
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Failed to compile $source_file" -ForegroundColor Red
        exit 1
    }
    $OBJECT_FILES += $object_file
}

# Link
Write-Host "Linking..." -ForegroundColor Yellow
$LINK_ARGS = $OBJECT_FILES + $LDFLAGS + @("-o", "$BUILD_DIR/$OUTPUT_NAME")

try {
    & $CXX @LINK_ARGS
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Compilation successful!" -ForegroundColor Green
    } else {
        Write-Host "Compilation failed with exit code $LASTEXITCODE" -ForegroundColor Red
        exit 1
    }
} catch {
    Write-Host "Compilation error: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}

# Copy resources
Write-Host "Copying game resources..." -ForegroundColor Yellow
if (Test-Path $GAME_RESOURCES_DIR) {
    $RESOURCE_BUILD_DIR = "$BUILD_DIR/resources"
    New-Item -ItemType Directory -Path $RESOURCE_BUILD_DIR -Force | Out-Null

    $RESOURCE_FILES = Get-ChildItem -Path $GAME_RESOURCES_DIR -File
    $COPIED_COUNT = 0

    foreach ($file in $RESOURCE_FILES) {
        try {
            Copy-Item -Path $file.FullName -Destination "$RESOURCE_BUILD_DIR/$($file.Name)" -Force
            $COPIED_COUNT++
        } catch {
            Write-Host "Failed to copy: $($file.Name)" -ForegroundColor Red
        }
    }

    Write-Host "Copied $COPIED_COUNT resource files" -ForegroundColor Green
} else {
    Write-Host "Game resources directory not found!" -ForegroundColor Red
}

# Copy shaders
Write-Host "Copying shader files..." -ForegroundColor Yellow
if (Test-Path $SHADERS_DIR) {
    $SHADER_BUILD_DIR = "$BUILD_DIR/shaders"
    New-Item -ItemType Directory -Path $SHADER_BUILD_DIR -Force | Out-Null

    $SHADER_FILES = Get-ChildItem -Path $SHADERS_DIR -File
    $COPIED_COUNT = 0

    foreach ($file in $SHADER_FILES) {
        try {
            Copy-Item -Path $file.FullName -Destination "$SHADER_BUILD_DIR/$($file.Name)" -Force
            $COPIED_COUNT++
        } catch {
            Write-Host "Failed to copy shader: $($file.Name)" -ForegroundColor Red
        }
    }

    Write-Host "Copied $COPIED_COUNT shader files" -ForegroundColor Green
} else {
    Write-Host "Shaders directory not found!" -ForegroundColor Red
}

Write-Host "=== Build Completed ===" -ForegroundColor Magenta
Write-Host "Executable: $BUILD_DIR/$OUTPUT_NAME" -ForegroundColor Cyan
