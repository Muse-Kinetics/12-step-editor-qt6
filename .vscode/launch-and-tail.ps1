param(
    [Parameter(Mandatory = $true)]
    [string]$Program,

    [Parameter(Mandatory = $true)]
    [string]$Workdir,

    [Parameter(Mandatory = $true)]
    [string]$LogDirectory,

    [Parameter(Mandatory = $false)]
    [string]$MidiBackend = ""
)

$programPath = [Environment]::ExpandEnvironmentVariables($Program)
$workingDirectory = [Environment]::ExpandEnvironmentVariables($Workdir)
$resolvedLogDirectory = [Environment]::ExpandEnvironmentVariables($LogDirectory)
$programDirectory = Split-Path -Parent $programPath

function Get-QtRuntimePaths {
    $candidates = @(
        $env:QMAKE,
        'C:\qt6\6.3.2\msvc2019_64\bin\qmake.exe',
        'C:\Qt6\6.3.2\msvc2019_64\bin\qmake.exe'
    ) | Where-Object { -not [string]::IsNullOrWhiteSpace($_) }

    $qmakeCommand = Get-Command qmake -ErrorAction SilentlyContinue
    if ($null -ne $qmakeCommand) {
        $candidates += $qmakeCommand.Source
    }

    foreach ($candidate in ($candidates | Select-Object -Unique)) {
        if (-not (Test-Path $candidate -PathType Leaf)) {
            continue
        }

        try {
            $qtBin = (& $candidate -query QT_INSTALL_BINS 2>$null | Select-Object -First 1).Trim()
            $qtPlugins = (& $candidate -query QT_INSTALL_PLUGINS 2>$null | Select-Object -First 1).Trim()
            $qtQml = (& $candidate -query QT_INSTALL_QML 2>$null | Select-Object -First 1).Trim()

            if (-not [string]::IsNullOrWhiteSpace($qtBin) -and (Test-Path $qtBin -PathType Container)) {
                return [pscustomobject]@{
                    Bin = $qtBin
                    Plugins = $qtPlugins
                    Qml = $qtQml
                }
            }
        }
        catch {
        }
    }

    throw 'Unable to resolve Qt runtime paths from qmake.'
}

function Add-PathEntry {
    param(
        [Parameter(Mandatory = $true)]
        [string]$PathValue
    )

    if ([string]::IsNullOrWhiteSpace($PathValue) -or -not (Test-Path $PathValue -PathType Container)) {
        return
    }

    $pathEntries = @($env:PATH -split ';' | Where-Object { -not [string]::IsNullOrWhiteSpace($_) })
    if ($pathEntries -contains $PathValue) {
        return
    }

    $env:PATH = "$PathValue;$env:PATH"
}

function Get-NewestLogFile {
    param(
        [Parameter(Mandatory = $true)]
        [string]$DirectoryPath
    )

    if (-not (Test-Path $DirectoryPath -PathType Container)) {
        return $null
    }

    return Get-ChildItem -LiteralPath $DirectoryPath -Filter *.log -File -ErrorAction SilentlyContinue |
        Sort-Object LastWriteTime -Descending |
        Select-Object -First 1
}

if (-not (Test-Path $resolvedLogDirectory -PathType Container)) {
    New-Item -ItemType Directory -Path $resolvedLogDirectory -Force | Out-Null
}

$previousNewestLog = Get-NewestLogFile -DirectoryPath $resolvedLogDirectory

$qtRuntime = Get-QtRuntimePaths

Add-PathEntry -PathValue $qtRuntime.Bin
Add-PathEntry -PathValue 'C:\Qt6\Tools\OpenSSL\Win_x64\bin'
Add-PathEntry -PathValue 'C:\Qt\Tools\OpenSSL\Win_x64\bin'

if (-not [string]::IsNullOrWhiteSpace($qtRuntime.Plugins) -and (Test-Path $qtRuntime.Plugins -PathType Container)) {
    $env:QT_PLUGIN_PATH = $qtRuntime.Plugins
}

if (-not [string]::IsNullOrWhiteSpace($qtRuntime.Qml) -and (Test-Path $qtRuntime.Qml -PathType Container)) {
    $env:QML2_IMPORT_PATH = $qtRuntime.Qml
    $env:QML_IMPORT_PATH = $qtRuntime.Qml
}

if (-not [string]::IsNullOrWhiteSpace($MidiBackend)) {
    $env:KMI_MIDI_BACKEND = $MidiBackend
}

try {
    Start-Process -FilePath $programPath -WorkingDirectory $programDirectory | Out-Null
}
catch {
    Write-Error $_
    exit 1
}

$launchDeadline = (Get-Date).AddSeconds(10)
$targetLogPath = $null

while ((Get-Date) -lt $launchDeadline) {
    $newestLog = Get-NewestLogFile -DirectoryPath $resolvedLogDirectory
    if ($null -ne $newestLog) {
        $isFreshLog = ($null -eq $previousNewestLog) -or ($newestLog.FullName -ne $previousNewestLog.FullName)
        if ($isFreshLog) {
            $targetLogPath = $newestLog.FullName
            break
        }
    }

    Start-Sleep -Milliseconds 250
}

if ([string]::IsNullOrEmpty($targetLogPath)) {
    $targetLogPath = $resolvedLogDirectory
}

& powershell -NoProfile -ExecutionPolicy Bypass -File (Join-Path $workingDirectory '.vscode\tail-log.ps1') $targetLogPath
exit $LASTEXITCODE