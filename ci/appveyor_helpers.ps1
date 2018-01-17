#
# Common helper functions for AppVeyor
#

# Helper function to provide the bin-folder path to mingw
function Get-MinGWBin() {
    if ($env:PlatformToolset -eq "6.3.0" ) {
        if ($env:Platform -like '*64') {
            Write-Output 'C:\mingw-w64\x86_64-6.3.0-posix-seh-rt_v5-rev1\mingw64\bin'
        }
        else {
            Write-Output 'C:\mingw-w64\i686-6.3.0-posix-dwarf-rt_v5-rev1\mingw64\bin'
        }
    }
}

function Add-PathFolder($folder)
{
    if (-not (Test-Path $folder))
    {
        Write-Host "Not adding $folder to the PATH, it does not exist"
    }

    [bool]$alreadyInPath = $false
    [System.Collections.ArrayList]$pathFolders = New-Object System.Collections.ArrayList
    $env:Path -split ";" | foreach { $pathFolders.Add($_) | Out-Null }

    for ([int]$i = 0; $i -lt $pathFolders.Count; $i++)
    {
        if ([string]::Compare($pathFolders[$i], $folder, $true) -eq 0)
        {
            $alreadyInPath = $true
            break
        }
    }

    if (-not $alreadyInPath)
    {
        Write-Host "Adding $folder to the PATH"
        $pathFolders.Insert(0, $folder)
        $env:Path = $pathFolders -join ";"
    }
}

function Remove-PathFolder($folder)
{
    [System.Collections.ArrayList]$pathFolders = New-Object System.Collections.ArrayList
    $env:Path -split ";" | foreach { $pathFolders.Add($_) | Out-Null }

    for ([int]$i = 0; $i -lt $pathFolders.Count; $i++)
    {
        if ([string]::Compare($pathFolders[$i], $folder, $true) -eq 0)
        {
            Write-Host "Removing $folder from the PATH"
            $pathFolders.RemoveAt($i)
            $i--
        }
    }

    $env:Path = $pathFolders -join ";"
}

function Invoke-Command($command, $directory = '.')
{
    $command_wrapped = "$command;`$err = `$?"
    Write-Host -ForegroundColor Yellow $command

    Push-Location $directory
    Invoke-Expression $command_wrapped

    if ($LASTEXITCODE -ne 0)
    {
        Pop-Location
        Write-Host "Command Returned error: $LASTEXITCODE"
        Exit $LASTEXITCODE
    }

    Pop-Location
}
