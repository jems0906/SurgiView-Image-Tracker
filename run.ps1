param(
    [switch]$Configure,
    [switch]$Build,
    [switch]$Test,
    [switch]$Run,
    [switch]$All
)

$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$buildDir = Join-Path $repoRoot "build"

if (-not ($Configure -or $Build -or $Test -or $Run -or $All)) {
    $All = $true
}

if ($All) {
    $Configure = $true
    $Build = $true
    $Test = $true
    $Run = $true
}

$cmakeCommand = Get-Command cmake -ErrorAction SilentlyContinue
$cmakeExe = "cmake"
if ($cmakeCommand) {
    $cmakeExe = $cmakeCommand.Source
} else {
    $cmakeFallback = "C:\Program Files\CMake\bin\cmake.exe"
    if (-not (Test-Path $cmakeFallback)) {
        throw "CMake was not found. Install CMake or update PATH."
    }
    $cmakeExe = $cmakeFallback
}

$cmakeDir = Split-Path -Parent $cmakeExe
$ctestExe = Join-Path $cmakeDir "ctest.exe"
if (-not (Test-Path $ctestExe)) {
    $ctestFallback = "C:\Program Files\CMake\bin\ctest.exe"
    if (Test-Path $ctestFallback) {
        $ctestExe = $ctestFallback
    } else {
        $ctestExe = "ctest"
    }
}

$qtPrefix = "C:\Qt\6.5.3\mingw_64"
$mingwBin = "C:\project\qt-tools\Tools\mingw1310_64\bin"
$ninjaBin = "C:\Qt\Tools\Ninja"

if (-not (Test-Path $qtPrefix)) {
    throw "Qt prefix not found at $qtPrefix"
}
if (-not (Test-Path $mingwBin)) {
    throw "MinGW toolchain not found at $mingwBin"
}
if (-not (Test-Path (Join-Path $ninjaBin "ninja.exe"))) {
    throw "Ninja not found at $ninjaBin"
}

$env:PATH = "$mingwBin;$($qtPrefix)\bin;$ninjaBin;" + $env:PATH

$hasBuildNinja = Test-Path (Join-Path $buildDir "build.ninja")
if (($Configure -or $Build -or $Test -or $Run) -and -not $hasBuildNinja) {
    $Configure = $true
}

if ($Configure) {
    $gccExe = (Join-Path $mingwBin "gcc.exe").Replace("\\", "/")
    $gppExe = (Join-Path $mingwBin "g++.exe").Replace("\\", "/")
    $ninjaExe = (Join-Path $ninjaBin "ninja.exe").Replace("\\", "/")

    if (Test-Path $buildDir) {
        Remove-Item $buildDir -Recurse -Force
    }
    New-Item -ItemType Directory -Path $buildDir | Out-Null

    $configureArgs = @(
        "-S", $repoRoot,
        "-B", $buildDir,
        "-G", "Ninja",
        "-DCMAKE_PREFIX_PATH=$qtPrefix",
        "-DCMAKE_C_COMPILER=$gccExe",
        "-DCMAKE_CXX_COMPILER=$gppExe",
        "-DCMAKE_MAKE_PROGRAM=$ninjaExe"
    )

    & $cmakeExe @configureArgs

    if ($LASTEXITCODE -ne 0) {
        throw "CMake configure failed with code $LASTEXITCODE"
    }
}

if ($Build) {
    & $cmakeExe --build $buildDir -j 2
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed with code $LASTEXITCODE"
    }
}

if ($Test) {
    & $ctestExe --test-dir $buildDir --output-on-failure
    if ($LASTEXITCODE -ne 0) {
        throw "Tests failed with code $LASTEXITCODE"
    }
}

if ($Run) {
    $exePath = Join-Path $buildDir "SurgiViewImageTracker.exe"
    if (-not (Test-Path $exePath)) {
        throw "Executable not found at $exePath"
    }

    Start-Process -FilePath $exePath
    Write-Output "SurgiViewImageTracker launched"
}
