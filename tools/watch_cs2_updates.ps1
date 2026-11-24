#requires -Version 5.1
[CmdletBinding(DefaultParameterSetName = 'Watch')]
param(
    [Parameter(ParameterSetName = 'Watch')]
    [ValidateRange(30, 86400)]
    [int]$IntervalSeconds = 300,
    [Parameter(ParameterSetName = 'Watch')]
    [switch]$Once,
    [Parameter(Mandatory = $true, ParameterSetName = 'Dialog', DontShow = $true)]
    [string]$DialogMessage
)

$ErrorActionPreference = 'Stop'
if ($PSCmdlet.ParameterSetName -eq 'Dialog') {
    Add-Type -AssemblyName Microsoft.VisualBasic
    $style = [Microsoft.VisualBasic.MsgBoxStyle]::Information -bor
        [Microsoft.VisualBasic.MsgBoxStyle]::SystemModal -bor
        [Microsoft.VisualBasic.MsgBoxStyle]::MsgBoxSetForeground
    [void][Microsoft.VisualBasic.Interaction]::MsgBox($DialogMessage, $style, 'Counter-Strike 2 update')
    return
}

$appInfoUrl = 'https://api.steamcmd.net/v1/info/730'
$stateDirectory = Join-Path $env:LOCALAPPDATA 'Shroudfire'
$statePath = Join-Path $stateDirectory 'cs2-update-build.txt'
$pendingPath = "$statePath.tmp"
$userSid = [System.Security.Principal.WindowsIdentity]::GetCurrent().User.Value
$created = $false
$mutex = [System.Threading.Mutex]::new($true, "Local\ShroudfireCs2UpdateWatcher-$userSid", [ref]$created)

try {
    if (-not $created) {
        throw 'The CS2 update watcher is already running in this Windows session.'
    }

    [void][System.IO.Directory]::CreateDirectory($stateDirectory)
    $lastBuild = $null
    if ([System.IO.File]::Exists($statePath)) {
        $lastBuild = [System.IO.File]::ReadAllText($statePath).Trim()
        if ($lastBuild -notmatch '^[1-9][0-9]*$') {
            Write-Warning 'The saved build ID is invalid. The next successful check will establish a new baseline.'
            $lastBuild = $null
        }
    }

    if (-not $Once) {
        Write-Host "Checking for CS2 updates every $IntervalSeconds seconds. Press Ctrl+C to stop."
    }

    do {
        try {
            $appInfo = Invoke-RestMethod -Uri $appInfoUrl -UseBasicParsing -TimeoutSec 20
            $build = [string]$appInfo.data.'730'.depots.branches.public.buildid
            if ($appInfo.status -ne 'success' -or $build -notmatch '^[1-9][0-9]*$') {
                throw 'The SteamCMD API did not return a valid CS2 public build ID.'
            }

            $timestamp = Get-Date -Format 'yyyy-MM-dd HH:mm:ss'
            if ($build -ne $lastBuild) {
                [System.IO.File]::WriteAllText($pendingPath, $build)

                if ($null -ne $lastBuild) {
                    $message = "CS2 updated from build $lastBuild to $build."
                    $dialog = [System.Diagnostics.ProcessStartInfo]::new(
                        (Join-Path $env:SystemRoot 'System32\WindowsPowerShell\v1.0\powershell.exe'))
                    $dialog.Arguments = '-NoProfile -NonInteractive -ExecutionPolicy RemoteSigned -File "{0}" -DialogMessage "{1}"' -f $PSCommandPath, $message
                    $dialog.UseShellExecute = $false
                    $dialog.CreateNoWindow = $true
                    # Keep polling while the dialog waits for the user to dismiss it.
                    [System.Diagnostics.Process]::Start($dialog).Dispose()
                }

                if ([System.IO.File]::Exists($statePath)) {
                    [System.IO.File]::Replace($pendingPath, $statePath, [NullString]::Value)
                } else {
                    [System.IO.File]::Move($pendingPath, $statePath)
                }

                if ($null -eq $lastBuild) {
                    Write-Host "[$timestamp] Saved CS2 public build $build as the baseline."
                } else {
                    Write-Host "[$timestamp] $message"
                }
                $lastBuild = $build
            } elseif ($Once) {
                Write-Host "[$timestamp] CS2 public build $build is unchanged."
            }
        } catch {
            if ($Once) {
                throw
            }
            Write-Warning "CS2 update check failed. $($_.Exception.Message) Retrying in $IntervalSeconds seconds."
        }

        if (-not $Once) {
            Start-Sleep -Seconds $IntervalSeconds
        }
    } until ($Once)
} finally {
    if ($created) {
        $mutex.ReleaseMutex()
    }
    $mutex.Dispose()
}
