param(
    [bool]$RunApp = $true,
    [bool]$OpenGuide = $true
)

$ErrorActionPreference = "Stop"
$repoRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Split-Path -Parent $repoRoot

$sampleFolder = Join-Path $repoRoot "sample-data\demo_ct_series"
$sessionFile = Join-Path $repoRoot "sample-data\demo_session.json"
$guideFile = Join-Path $repoRoot "docs\demo\walkthrough.md"

Write-Output "SurgiView demo assets"
Write-Output "  Slice folder : $sampleFolder"
Write-Output "  Session file : $sessionFile"
Write-Output "  Guide        : $guideFile"
Write-Output ""
Write-Output "Recommended live demo steps"
Write-Output "  1. In the app, load the sample slice folder above."
Write-Output "  2. Show simulated tracking, target selection, and measurement drag."
Write-Output "  3. Switch source mode to external and start TCP on port 45454."
Write-Output "  4. Run the VS Code task 'SurgiView: Demo Sender TCP'."
Write-Output "  5. Optionally load the sample session JSON for playback."

if ($RunApp) {
    & (Join-Path $repoRoot "run.ps1") -Run
}

if ($OpenGuide) {
    Start-Process $guideFile
}
