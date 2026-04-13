param(
    [string]$OutputRoot,
    [string]$BundleName = "SurgiView-DemoBundle",
    [bool]$SkipBuild = $false,
    [bool]$SkipZip = $false
)

$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Split-Path -Parent $repoRoot
$buildDir = Join-Path $repoRoot "build"
$exePath = Join-Path $buildDir "SurgiViewImageTracker.exe"
$qtBin = "C:\Qt\6.5.3\mingw_64\bin"
$windeployqt = Join-Path $qtBin "windeployqt.exe"

if ([string]::IsNullOrWhiteSpace($OutputRoot)) {
    $OutputRoot = Join-Path $repoRoot "dist"
}

if (-not $SkipBuild) {
    & (Join-Path $repoRoot "run.ps1") -Build
}

if (-not (Test-Path $exePath)) {
    throw "Built executable not found at $exePath"
}

if (-not (Test-Path $windeployqt)) {
    throw "windeployqt.exe not found at $windeployqt"
}

$bundleDir = Join-Path $OutputRoot $BundleName
$zipPath = Join-Path $OutputRoot ($BundleName + ".zip")

if (Test-Path $bundleDir) {
    Remove-Item $bundleDir -Recurse -Force
}
if (Test-Path $zipPath) {
    Remove-Item $zipPath -Force
}

New-Item -ItemType Directory -Path $bundleDir -Force | Out-Null

Copy-Item $exePath $bundleDir
Copy-Item (Join-Path $repoRoot "README.md") $bundleDir
Copy-Item (Join-Path $repoRoot "sample-data") (Join-Path $bundleDir "sample-data") -Recurse
Copy-Item (Join-Path $repoRoot "docs\demo") (Join-Path $bundleDir "docs\demo") -Recurse -Force
Copy-Item (Join-Path $repoRoot "docs\safety") (Join-Path $bundleDir "docs\safety") -Recurse -Force

$launchScript = @'
param()
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
Start-Process -FilePath (Join-Path $root "SurgiViewImageTracker.exe")
'@
Set-Content -Path (Join-Path $bundleDir "launch-demo.ps1") -Value $launchScript -Encoding ascii

& $windeployqt --qmldir (Join-Path $repoRoot "qml") --dir $bundleDir (Join-Path $bundleDir "SurgiViewImageTracker.exe")

if (-not $SkipZip) {
    Compress-Archive -Path (Join-Path $bundleDir "*") -DestinationPath $zipPath -Force
    Write-Output "Demo bundle zip created at $zipPath"
}

Write-Output "Demo bundle directory ready at $bundleDir"
