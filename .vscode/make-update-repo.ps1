param(
    [string]$OutputDir
)

$ErrorActionPreference = 'Stop'

function Write-Step {
    param([string]$Message)
    Write-Host "`n==> $Message"
}

function Get-VersionFromProFile {
    param([string]$ProFilePath)

    $versionLine = Select-String -Path $ProFilePath -Pattern '^VERSION\s*=\s*(.+)$' | Select-Object -First 1
    if (-not $versionLine) {
        throw "Could not find VERSION in $ProFilePath"
    }

    return $versionLine.Matches[0].Groups[1].Value.Trim()
}

$workspaceRoot = Split-Path -Parent $PSScriptRoot
$winBuildRoot = Join-Path $workspaceRoot 'win-build'
$editorPro = Join-Path $workspaceRoot 'QT\12StepEditor.pro'
$packagesDir = Join-Path $winBuildRoot 'packages'
$repogen = 'C:\Qt6\QtIFW-4.6.0\bin\repogen.exe'

if (-not $OutputDir) {
    $OutputDir = Join-Path $winBuildRoot 'update-repository'
}

$version = Get-VersionFromProFile -ProFilePath $editorPro
Write-Host "Building update repository for version $version"

$packageDataDir = Join-Path $packagesDir 'com.keithmcmillen.12stepeditor\data'
if (-not (Test-Path -LiteralPath $packageDataDir)) {
    throw "Package data not found at $packageDataDir. Run make-installer.ps1 first so the package payload is staged."
}

Write-Step "Running repogen -> $OutputDir"
& $repogen -p $packagesDir -v -r $OutputDir
if ($LASTEXITCODE -ne 0) {
    throw "repogen failed with exit code $LASTEXITCODE"
}

Write-Host "`nUpdate repository generated at $OutputDir"
Write-Host "Contents:"
Get-ChildItem -Recurse -File $OutputDir | ForEach-Object {
    Write-Host "  $($_.FullName.Substring($OutputDir.Length + 1))  ($([math]::Round($_.Length / 1KB, 1)) KB)"
}
