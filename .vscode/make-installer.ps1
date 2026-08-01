param(
    [switch]$DisableSigning
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

function Set-XmlNodeValue {
    param(
        [string]$FilePath,
        [string]$XPath,
        [string]$Value
    )

    if (-not (Test-Path -LiteralPath $FilePath)) {
        throw "Required XML file not found: $FilePath"
    }

    $xml = New-Object System.Xml.XmlDocument
    $xml.PreserveWhitespace = $true
    $xml.Load($FilePath)

    $node = $xml.SelectSingleNode($XPath)
    if (-not $node) {
        throw "Could not find XML node '$XPath' in $FilePath"
    }

    $node.InnerText = $Value
    $xml.Save($FilePath)
}

function Update-InstallerMetadata {
    param(
        [string]$InstallerVersion,
        [string]$ConfigFile,
        [string[]]$PackageFiles
    )

    Write-Step 'Updating installer metadata versions'
    Set-XmlNodeValue -FilePath $ConfigFile -XPath '/Installer/Version' -Value $InstallerVersion

    foreach ($packageFile in $PackageFiles) {
        Set-XmlNodeValue -FilePath $packageFile -XPath '/Package/Version' -Value $InstallerVersion
    }
}

function Ensure-Directory {
    param([string]$Path)

    if (-not (Test-Path -LiteralPath $Path)) {
        New-Item -ItemType Directory -Path $Path -Force | Out-Null
    }
}

function Copy-RequiredFile {
    param(
        [string]$Source,
        [string]$Destination
    )

    if (-not (Test-Path -LiteralPath $Source)) {
        throw "Required file not found: $Source"
    }

    Copy-Item -LiteralPath $Source -Destination $Destination -Force
}

function Invoke-External {
    param(
        [string]$FilePath,
        [string[]]$ArgumentList,
        [string]$WorkingDirectory
    )

    $displayArgs = ($ArgumentList | ForEach-Object {
        if ($_ -match '\s') { '"' + $_ + '"' } else { $_ }
    }) -join ' '
    Write-Host "$FilePath $displayArgs"

    Push-Location -LiteralPath $WorkingDirectory
    try {
        & $FilePath @ArgumentList
        if ($LASTEXITCODE -ne 0) {
            throw "Command failed with exit code ${LASTEXITCODE}: $FilePath"
        }
    }
    finally {
        Pop-Location
    }
}

function Invoke-Signing {
    param(
        [string]$SignTool,
        [string]$CertThumbprint,
        [string]$TimestampUrl,
        [string]$FileToSign,
        [string]$WorkingDirectory
    )

    if ($DisableSigning) {
        Write-Host "Skipping signing for $FileToSign because -DisableSigning was supplied."
        return
    }

    # Matches the proven-working manual command documented in sendsysex's
    # RELEASING.md (signed successfully from this same token/terminal): plain
    # /a cert auto-select (no /sha1 needed - the token's cert is the only one
    # with an accessible private key) and /fd sha256 as the digest algorithm.
    # The previous /fd certHash (not a valid digest algorithm - only sha1/
    # sha256 are) combined with /v /debug and an explicit /sha1 alongside /a
    # is copied from 12StepEditor.pro's own deploy target, which had never
    # actually been run to completion before, so this bug went uncaught.
    Invoke-External -FilePath $SignTool -ArgumentList @(
        'sign', '/fd', 'sha256', '/a',
        '/tr', $TimestampUrl,
        '/td', 'SHA256',
        $FileToSign
    ) -WorkingDirectory $WorkingDirectory
}

function Copy-EditorPayload {
    param(
        [string]$PackageDir,
        [string]$ReleaseExe,
        [string]$DisplayName,
        [string]$QtDeployTool,
        [string]$SslCrypto,
        [string]$SslTls,
        [string]$SignTool,
        [string]$CertThumbprint
    )

    Ensure-Directory -Path $PackageDir

    $mainDest = Join-Path $PackageDir "$DisplayName.exe"

    Write-Step "Staging $DisplayName"
    Copy-RequiredFile -Source $ReleaseExe -Destination $mainDest

    Invoke-Signing -SignTool $SignTool -CertThumbprint $CertThumbprint -TimestampUrl 'http://timestamp.digicert.com' -FileToSign $mainDest -WorkingDirectory $PackageDir

    Write-Step "Running windeployqt for $DisplayName"
    Invoke-External -FilePath $QtDeployTool -ArgumentList @(
        '--release',
        '--compiler-runtime',
        '--dir', $PackageDir,
        $mainDest
    ) -WorkingDirectory $PackageDir

    Write-Step "Copying SSL runtime for $DisplayName"
    Copy-RequiredFile -Source $SslCrypto -Destination $PackageDir
    Copy-RequiredFile -Source $SslTls -Destination $PackageDir
}

$workspaceRoot = Split-Path -Parent $PSScriptRoot
$winBuildRoot = Join-Path $workspaceRoot 'win-build'

$editorPro = Join-Path $workspaceRoot 'QT\12StepEditor.pro'

$installerVersion = Get-VersionFromProFile -ProFilePath $editorPro
$installerName = "12 Step Editor v$installerVersion Windows Installer"
$installerFile = "$installerName.exe"

$qtDeployTool = 'C:\qt6\6.3.2\msvc2019_64\bin\windeployqt.exe'
$binaryCreator = 'C:\Qt6\QtIFW-4.6.0\bin\binarycreator.exe'
$signTool = 'C:\Program Files (x86)\Windows Kits\10\bin\10.0.22000.0\x64\signtool.exe'
$certThumbprint = '66aef8acbdc187562ba7af2fec2b070de1612267'

$sslCrypto = Join-Path $workspaceRoot 'QT\ssl\libcrypto-1_1-x64.dll'
$sslTls = Join-Path $workspaceRoot 'QT\ssl\libssl-1_1-x64.dll'

$releaseDir = Join-Path $workspaceRoot 'build-12StepEditor-Desktop_Qt_6_3_2_MSVC2019_64bit-Release\release'
$releaseExe = Join-Path $releaseDir '12 Step Editor.exe'

$packageDir = Join-Path $winBuildRoot 'packages\com.keithmcmillen.12stepeditor\data\12 Step Editor'
$contentDir = Join-Path $winBuildRoot 'packages\com.keithmcmillen.12stepeditor\data\Content'

$contentSource = Join-Path $workspaceRoot 'Content'
$changelogSource = Join-Path $workspaceRoot 'CHANGELOG.md'
$configFile = Join-Path $winBuildRoot 'config\config.xml'
$packagesDir = Join-Path $winBuildRoot 'packages'
$installerOutputPath = Join-Path $winBuildRoot $installerFile
$packageMetaFile = Join-Path $winBuildRoot 'packages\com.keithmcmillen.12stepeditor\meta\package.xml'

Write-Host "Installer version: $installerVersion"
if ($DisableSigning) {
    Write-Host 'Signing is disabled for this run.'
}

Update-InstallerMetadata -InstallerVersion $installerVersion -ConfigFile $configFile -PackageFiles @(
    $packageMetaFile
)

Copy-EditorPayload -PackageDir $packageDir -ReleaseExe $releaseExe -DisplayName '12 Step Editor' -QtDeployTool $qtDeployTool -SslCrypto $sslCrypto -SslTls $sslTls -SignTool $signTool -CertThumbprint $certThumbprint

Write-Step 'Refreshing installer content package'
if (Test-Path -LiteralPath $contentDir) {
    Remove-Item -LiteralPath $contentDir -Recurse -Force
}
Ensure-Directory -Path $contentDir
Copy-Item -LiteralPath (Join-Path $contentSource '*') -Destination $contentDir -Recurse -Force
Copy-RequiredFile -Source $changelogSource -Destination $contentDir

Write-Step 'Creating installer'
if (Test-Path -LiteralPath $installerOutputPath) {
    Remove-Item -LiteralPath $installerOutputPath -Force
}
Invoke-External -FilePath $binaryCreator -ArgumentList @(
    '--verbose',
    '--offline-only',
    '-c', $configFile,
    '-p', $packagesDir,
    $installerName
) -WorkingDirectory $winBuildRoot

Write-Step 'Signing installer'
Invoke-Signing -SignTool $signTool -CertThumbprint $certThumbprint -TimestampUrl 'http://timestamp.globalsign.com/tsa/advanced' -FileToSign $installerOutputPath -WorkingDirectory $winBuildRoot

Write-Host "`nInstaller created at $installerOutputPath"
