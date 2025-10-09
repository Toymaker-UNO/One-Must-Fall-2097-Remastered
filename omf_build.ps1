# Simple Direct Build Script for One Must Fall 2097
Write-Host "=== C++ Build Started ===" -ForegroundColor Magenta

# 설정
$SRC_DIR = "src"
$BUILD_DIR = "build"
$GAME_RESOURCES_DIR = "resources/game_resources"
$VCPKG_LIB_DIR = "lib/bin"
$VCPKG_INCLUDE_DIR = "lib/include"
$OUTPUT_NAME = "openomf.exe"

# 컴파일러 설정
$CC = "gcc"
$CXX = "g++"

# C 컴파일러 플래그
$CFLAGS = @()
$CFLAGS += "-std=c11"
$CFLAGS += "-O2"
$CFLAGS += "-w"
$CFLAGS += "-I$SRC_DIR"
$CFLAGS += "-I$SRC_DIR/vendored"
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
$CFLAGS += "-Wno-jump-misses-init"
$CFLAGS += "-include"
$CFLAGS += "stdint.h"
$CFLAGS += "-include"
$CFLAGS += "stddef.h"
$CFLAGS += "-include"
$CFLAGS += "string.h"
$CFLAGS += "-include"
$CFLAGS += "stdbool.h"

# C++ 컴파일러 플래그
$CXXFLAGS = @()
$CXXFLAGS += "-std=c++17"
$CXXFLAGS += "-O2"
$CXXFLAGS += "-w"
$CXXFLAGS += "-I$SRC_DIR"
$CXXFLAGS += "-I$SRC_DIR/vendored"
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

# 링커 플래그
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

# 빌드 디렉토리 정리 및 생성
Write-Host "Cleaning build directory..." -ForegroundColor Yellow
if (Test-Path $BUILD_DIR) {
    Remove-Item -Recurse -Force $BUILD_DIR
}
New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null

# 소스 파일 수집
Write-Host "Collecting source files..." -ForegroundColor Yellow
$C_FILES = Get-ChildItem -Path $SRC_DIR -Recurse -Filter "*.c" | ForEach-Object { $_.FullName }
$CPP_FILES = Get-ChildItem -Path $SRC_DIR -Recurse -Filter "*.cpp" | ForEach-Object { $_.FullName }
Write-Host "Found $($C_FILES.Count) C source files" -ForegroundColor Green
Write-Host "Found $($CPP_FILES.Count) C++ source files" -ForegroundColor Green

# 컴파일
Write-Host "Compiling..." -ForegroundColor Yellow
$OBJECT_FILES = @()

# C 파일들 컴파일
foreach ($source_file in $C_FILES) {
    $object_file = $source_file -replace "\.c$", ".o"
    $object_file = $object_file -replace "src\\", "$BUILD_DIR\\"
    $object_dir = Split-Path $object_file -Parent
    if (!(Test-Path $object_dir)) {
        New-Item -ItemType Directory -Path $object_dir -Force | Out-Null
    }
    
    $compile_args = @($CFLAGS; $source_file; "-c"; "-o"; $object_file)
    Write-Host "Compiling C: $source_file..." -ForegroundColor Cyan
    & $CC @compile_args
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Failed to compile $source_file" -ForegroundColor Red
        exit 1
    }
    $OBJECT_FILES += $object_file
}

# C++ 파일들 컴파일
foreach ($source_file in $CPP_FILES) {
    $object_file = $source_file -replace "\.cpp$", ".o"
    $object_file = $object_file -replace "src\\", "$BUILD_DIR\\"
    $object_dir = Split-Path $object_file -Parent
    if (!(Test-Path $object_dir)) {
        New-Item -ItemType Directory -Path $object_dir -Force | Out-Null
    }
    
    $compile_args = @($CXXFLAGS; $source_file; "-c"; "-o"; $object_file)
    Write-Host "Compiling C++: $source_file..." -ForegroundColor Cyan
    & $CXX @compile_args
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Failed to compile $source_file" -ForegroundColor Red
        exit 1
    }
    $OBJECT_FILES += $object_file
}

# 오브젝트 파일들을 C++ 링커로 링크
Write-Host "Linking..." -ForegroundColor Yellow
$LINK_ARGS = @($OBJECT_FILES; $LDFLAGS; "-o"; "$BUILD_DIR/$OUTPUT_NAME")

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

# 리소스 복사
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

# 셰이더 파일 복사
Write-Host "Copying shader files..." -ForegroundColor Yellow
if (Test-Path "resources/shaders") {
    $SHADER_BUILD_DIR = "$BUILD_DIR/shaders"
    New-Item -ItemType Directory -Path $SHADER_BUILD_DIR -Force | Out-Null
    
    $SHADER_FILES = Get-ChildItem -Path "resources/shaders" -File
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

