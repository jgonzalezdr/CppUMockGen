#
# Common helper functions for AppVeyor
#

# Helper function to provide the bin-folder path to mingw
function Get-MinGWBin() {
	if ($env:Platform -like 'MinGW*') 
	{
		if ($env:PlatformToolset -eq "8.1.0" ) 
		{
			if ($env:Platform -like '*64') {
				Write-Output 'C:\mingw-w64\x86_64-8.1.0-posix-seh-rt_v6-rev0\mingw64\bin'
			}
			else {
				Write-Output 'C:\mingw-w64\i686-8.1.0-posix-dwarf-rt_v6-rev0\mingw64\bin'
			}
		}
		elseif ($env:PlatformToolset -eq "7.3.0" ) 
		{
			if ($env:Platform -like '*64') {
				Write-Output 'C:\mingw-w64\x86_64-7.3.0-posix-seh-rt_v5-rev0\mingw64\bin'
			}
			else {
				Write-Output 'C:\mingw-w64\i686-7.3.0-posix-dwarf-rt_v5-rev0\mingw64\bin'
			}
		}
		elseif ($env:PlatformToolset -eq "6.3.0" ) 
		{
			if ($env:Platform -like '*64') {
				Write-Output 'C:\mingw-w64\x86_64-6.3.0-posix-seh-rt_v5-rev1\mingw64\bin'
			}
			else {
				Write-Output 'C:\mingw-w64\i686-6.3.0-posix-dwarf-rt_v5-rev1\mingw64\bin'
			}
		}
	}
	elseif ($env:Platform -like 'TDM-GCC*') 
	{
		if ($env:Platform -like '*64') {
			Write-Output 'C:\TDM-GCC-64\bin'
		}
		else {
			Write-Output 'C:\TDM-GCC\bin'
		}
	}
}

function Get-MsBuildCmd()
{
    if ($env:APPVEYOR_BUILD_WORKER_IMAGE -like "*2019")
    {
        Write-Output "msbuild"
    }
    elseif ($env:APPVEYOR_BUILD_WORKER_IMAGE -like "*2017")
    {
        Write-Output "msbuild /ToolsVersion:15.0"
    }
    else
    {
        Write-Output "msbuild /ToolsVersion:14.0"
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
