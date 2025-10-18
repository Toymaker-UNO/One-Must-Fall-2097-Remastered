# SDL Test Build Script for One Must Fall 2097
Write-Host "=== SDL Test Build Started ===" -ForegroundColor Magenta

# 설정
$SRC_DIR = "src"
$BUILD_DIR = "build"
$VCPKG_LIB_DIR = "lib/bin"
$VCPKG_INCLUDE_DIR = "lib/include"
$OUTPUT_NAME = "sdl_test.exe"
$MAIN_FILE = "sdl_test_main.cpp"

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

# 메인 파일 확인
if (!(Test-Path $MAIN_FILE)) {
    Write-Host "Main file not found: $MAIN_FILE" -ForegroundColor Red
    exit 1
}

# 소스 파일 수집
Write-Host "Collecting source files..." -ForegroundColor Yellow

# SDL Test 메인 파일
$MAIN_CPP_FILE = $MAIN_FILE

# SDL Test는 단순한 SDL2 프로그램이므로 src 파일들이 필요하지 않음
$SRC_C_FILES = @()

Write-Host "Found SDL Test main: $MAIN_CPP_FILE" -ForegroundColor Green
Write-Host "Found $($SRC_C_FILES.Count) src C files" -ForegroundColor Green

# 컴파일
Write-Host "Compiling..." -ForegroundColor Yellow
$OBJECT_FILES = @()

# SDL Test 메인 파일 컴파일
$object_file = $MAIN_CPP_FILE -replace "\.cpp$", ".o"
$object_file = $object_file -replace "^", "$BUILD_DIR\\"
$object_dir = Split-Path $object_file -Parent
if (!(Test-Path $object_dir)) {
    New-Item -ItemType Directory -Path $object_dir -Force | Out-Null
}

$compile_args = @($CXXFLAGS; $MAIN_CPP_FILE; "-c"; "-o"; $object_file)
Write-Host "Compiling SDL Test main: $MAIN_CPP_FILE..." -ForegroundColor Cyan
& $CXX @compile_args
if ($LASTEXITCODE -ne 0) {
    Write-Host "Failed to compile SDL Test main" -ForegroundColor Red
    exit 1
}
$OBJECT_FILES += $object_file

# Src 파일들 컴파일 (SDL Test에서는 필요 없지만 구조 유지)
foreach ($source_file in $SRC_C_FILES) {
    $object_file = $source_file -replace "\.c$", ".o"
    $object_file = $object_file -replace "src\\", "$BUILD_DIR\\"
    $object_dir = Split-Path $object_file -Parent
    if (!(Test-Path $object_dir)) {
        New-Item -ItemType Directory -Path $object_dir -Force | Out-Null
    }
    
    $compile_args = @($CFLAGS; $source_file; "-c"; "-o"; $object_file)
    Write-Host "Compiling src: $source_file..." -ForegroundColor Cyan
    & $CC @compile_args
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
        Write-Host "SDL Test compilation successful!" -ForegroundColor Green
        Write-Host "Executable: $BUILD_DIR/$OUTPUT_NAME" -ForegroundColor Cyan
    } else {
        Write-Host "SDL Test compilation failed with exit code $LASTEXITCODE" -ForegroundColor Red
        exit 1
    }
} catch {
    Write-Host "SDL Test compilation error: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}

# 리소스 복사 (선택사항 - PNG 파일들)
Write-Host "Copying test resources..." -ForegroundColor Yellow
if (Test-Path "resources") {
    $RESOURCE_BUILD_DIR = "$BUILD_DIR/resources"
    New-Item -ItemType Directory -Path $RESOURCE_BUILD_DIR -Force | Out-Null
    
    # PNG 파일들만 복사
    $PNG_FILES = Get-ChildItem -Path "resources" -Filter "*.png" -Recurse
    $COPIED_COUNT = 0
    
    foreach ($file in $PNG_FILES) {
        try {
            Copy-Item -Path $file.FullName -Destination "$RESOURCE_BUILD_DIR/$($file.Name)" -Force
            $COPIED_COUNT++
        } catch {
            Write-Host "Failed to copy: $($file.Name)" -ForegroundColor Red
        }
    }
    
    Write-Host "Copied $COPIED_COUNT PNG files" -ForegroundColor Green
} else {
    Write-Host "Resources directory not found!" -ForegroundColor Red
}

Write-Host "=== SDL Test Build Completed ===" -ForegroundColor Magenta
Write-Host "Usage: $BUILD_DIR/$OUTPUT_NAME <first.bmp> <second.bmp>" -ForegroundColor Yellow
Write-Host "Example: $BUILD_DIR/$OUTPUT_NAME test1.bmp test2.bmp" -ForegroundColor Yellow
Write-Host "Controls: Arrow keys to switch images, ESC to quit" -ForegroundColor Yellow
