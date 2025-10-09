# Simple Direct Build Script for BKTool
Write-Host "=== BKTool Build Started ===" -ForegroundColor Magenta

# Configuration
$SRC_DIR = "src"
$TOOLS_DIR = "Tools"
$BUILD_DIR = "build"
$VCPKG_LIB_DIR = "lib/bin"
$VCPKG_INCLUDE_DIR = "lib/include"
$OUTPUT_NAME = "bktool.exe"

# Compiler settings
$CC = "gcc"

# C Compiler Flags
$CFLAGS = @()
$CFLAGS += "-std=c11"
$CFLAGS += "-O2"
$CFLAGS += "-w"
$CFLAGS += "-I$SRC_DIR"
$CFLAGS += "-I$TOOLS_DIR/shared"
$CFLAGS += "-I$VCPKG_INCLUDE_DIR"
$CFLAGS += "-I$VCPKG_INCLUDE_DIR/SDL2"
$CFLAGS += "-I$SRC_DIR/vendored"
$CFLAGS += "-DSDL_MAIN_HANDLED" # Required for SDL main
$CFLAGS += "-DUSE_LIBPNG=1" # If PNG is used for sprite export/import
$CFLAGS += "-DPNG_FOUND=1" # Enable PNG functions

# Linker Flags
$LDFLAGS = @()
$LDFLAGS += "-L$VCPKG_LIB_DIR"
$LDFLAGS += "-lSDL2"
$LDFLAGS += "-lSDL2main" # Often needed for SDL applications
$LDFLAGS += "-lpng"
$LDFLAGS += "-lzlib"
$LDFLAGS += "-lm"
$LDFLAGS += "-lmingw32"
$LDFLAGS += "-mconsole"
$LDFLAGS += "-lconfuse" # For config functions
$LDFLAGS += "-lole32" # For Windows COM functions
$LDFLAGS += "-loleaut32" # For Windows OLE functions
$LDFLAGS += "-lsetupapi" # For Windows Setup API functions
$LDFLAGS += "-lwinmm" # For Windows multimedia functions
$LDFLAGS += "-luser32" # For Windows user functions
$LDFLAGS += "-lgdi32" # For Windows GDI functions
$LDFLAGS += "-ladvapi32" # For Windows advanced API functions
$LDFLAGS += "-lversion" # For Windows version functions
$LDFLAGS += "-lhid" # For Windows HID functions
$LDFLAGS += "-lcfgmgr32" # For Windows configuration manager functions
$LDFLAGS += "-limm32" # For Windows input method functions
$LDFLAGS += "-lshlwapi" # For PathFileExistsA function
# argtable3 is included as source file, no need to link

# Clean and create build directory
Write-Host "Cleaning build directory..." -ForegroundColor Yellow
if (Test-Path $BUILD_DIR) {
    Remove-Item -Recurse -Force $BUILD_DIR
}
New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null

# Collect source files
Write-Host "Collecting source files..." -ForegroundColor Yellow
$BKT_MAIN_FILE = "$TOOLS_DIR/bktool/main.c"
$SHARED_C_FILES = Get-ChildItem -Path "$TOOLS_DIR/shared" -Recurse -Filter "*.c" | ForEach-Object { $_.FullName }

# Explicitly include necessary src files
$SRC_C_FILES = @()
$SRC_C_FILES += Get-ChildItem -Path "$SRC_DIR/formats" -Recurse -Filter "*.c" | ForEach-Object { $_.FullName }
$SRC_C_FILES += Get-ChildItem -Path "$SRC_DIR/utils" -Recurse -Filter "*.c" | ForEach-Object { $_.FullName }
$SRC_C_FILES += "$SRC_DIR/video/vga_state.c"
$SRC_C_FILES += "$SRC_DIR/video/vga_palette.c"
$SRC_C_FILES += "$SRC_DIR/video/damage_tracker.c"
$SRC_C_FILES += "$SRC_DIR/video/vga_remap.c"
$SRC_C_FILES += "$SRC_DIR/resources/pathmanager.c"
$SRC_C_FILES += "$SRC_DIR/resources/ids.c"
$SRC_C_FILES += "$SRC_DIR/resources/trnmanager.c"
$SRC_C_FILES += "$SRC_DIR/vendored/argtable3.c"

$ALL_C_FILES = @($BKT_MAIN_FILE; $SHARED_C_FILES; $SRC_C_FILES)

Write-Host "Found BKTool main: $BKT_MAIN_FILE" -ForegroundColor Green
Write-Host "Found $($SHARED_C_FILES.Count) shared C files" -ForegroundColor Green
Write-Host "Found $($SRC_C_FILES.Count) src C files" -ForegroundColor Green

# Compile
Write-Host "Compiling..." -ForegroundColor Yellow
$OBJECT_FILES = @()

foreach ($source_file in $ALL_C_FILES) {
    $relative_path = $source_file.Replace("$PSScriptRoot\", "")
    $object_file = "$BUILD_DIR\" + $relative_path -replace "\.c$", ".o"
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

# Link
Write-Host "Linking..." -ForegroundColor Yellow
$LINK_ARGS = @($OBJECT_FILES; $LDFLAGS; "-o"; "$BUILD_DIR/$OUTPUT_NAME")

try {
    & $CC @LINK_ARGS
    if ($LASTEXITCODE -eq 0) {
        Write-Host "BKTool compilation successful!" -ForegroundColor Green
    } else {
        Write-Host "BKTool compilation failed with exit code $LASTEXITCODE" -ForegroundColor Red
        exit 1
    }
} catch {
    Write-Host "BKTool compilation error: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}

Write-Host "=== BKTool Build Completed ===" -ForegroundColor Magenta
Write-Host "Executable: $BUILD_DIR/$OUTPUT_NAME" -ForegroundColor Cyan
