param(
    [Parameter(Mandatory = $true)]
    [string]$LogPath
)

$resolvedPath = [Environment]::ExpandEnvironmentVariables($LogPath)

function Get-TargetLogFile {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path
    )

    if (Test-Path $Path -PathType Leaf) {
        return Get-Item -LiteralPath $Path
    }

    if (Test-Path $Path -PathType Container) {
        return Get-ChildItem -LiteralPath $Path -Filter *.log -File -ErrorAction SilentlyContinue |
            Sort-Object LastWriteTime -Descending |
            Select-Object -First 1
    }

    return $null
}

function Read-AppendedText {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path,

        [Parameter(Mandatory = $true)]
        [long]$Offset
    )

    $fileInfo = Get-Item -LiteralPath $Path -ErrorAction Stop
    $safeOffset = [Math]::Min($Offset, $fileInfo.Length)

    $fileStream = [System.IO.File]::Open($Path, [System.IO.FileMode]::Open, [System.IO.FileAccess]::Read, [System.IO.FileShare]::ReadWrite)
    try {
        $fileStream.Seek($safeOffset, [System.IO.SeekOrigin]::Begin) | Out-Null
        $streamReader = New-Object System.IO.StreamReader($fileStream)
        try {
            $text = $streamReader.ReadToEnd()
            return @{
                Text = $text
                NextOffset = $fileStream.Position
            }
        }
        finally {
            $streamReader.Dispose()
        }
    }
    finally {
        $fileStream.Dispose()
    }
}

if (-not (Test-Path $resolvedPath) -and -not [System.IO.Path]::HasExtension($resolvedPath)) {
    New-Item -ItemType Directory -Path $resolvedPath -Force | Out-Null
}

$currentLogFile = $null
$currentOffset = 0L

while ($true) {
    $targetLogFile = Get-TargetLogFile -Path $resolvedPath

    if ($null -eq $targetLogFile) {
        Start-Sleep -Milliseconds 500
        continue
    }

    if ($null -eq $currentLogFile -or $currentLogFile.FullName -ne $targetLogFile.FullName) {
        $currentLogFile = $targetLogFile
        Write-Host ("Tailing log file: {0}" -f $currentLogFile.FullName)

        $tailLines = Get-Content -LiteralPath $currentLogFile.FullName -Tail 40 -ErrorAction SilentlyContinue
        if ($tailLines) {
            $tailLines | ForEach-Object { Write-Output $_ }
        }

        $currentOffset = (Get-Item -LiteralPath $currentLogFile.FullName).Length
    }

    $readResult = Read-AppendedText -Path $currentLogFile.FullName -Offset $currentOffset
    $currentOffset = $readResult.NextOffset

    if (-not [string]::IsNullOrEmpty($readResult.Text)) {
        $readResult.Text -split "`r?`n" | Where-Object { $_ -ne "" } | ForEach-Object { Write-Output $_ }
    }

    Start-Sleep -Milliseconds 500
}