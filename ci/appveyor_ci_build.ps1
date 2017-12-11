
# Load functions from the helper file
. (Join-Path (Split-Path $MyInvocation.MyCommand.Path) 'appveyor_helpers.ps1')

# Helper function to provide the bin-folder path to mingw
function Get-MinGWBin() {
    if ($env:PlatformToolset -eq "6.3.0" ) {
        if ($env:Platform -like '*64') {
            Write-Output 'C:\mingw-w64\x86_64-6.3.0-posix-seh-rt_v5-rev1\bin'
        }
        else {
            Write-Output 'C:\mingw-w64\i686-6.3.0-posix-dwarf-rt_v5-rev1\bin'
        }
    }
}

# Clean up some paths for any configuration
#Remove-PathFolder "C:\MinGW\bin"
#Remove-PathFolder "C:\Program Files\Git\bin"
#Remove-PathFolder "C:\Program Files\Git\cmd"
#Remove-PathFolder "C:\Program Files\Git\usr\bin"
#Remove-PathFolder "C:\Program Files (x86)\Git\bin"
#Remove-PathFolder "C:\Program Files (x86)\Git\cmd"
#Remove-PathFolder "C:\Program Files (x86)\Git\usr\bin"

$build_dir = "build"

New-Item -ItemType Directory -Force -Path "$build_dir" | Out-Null

switch -Wildcard ($env:Platform)
{
    'MinGW*'
    {
        $mingw_path = Get-MinGWBin

        # Add mingw to the path
        Add-PathFolder $mingw_path

        Invoke-Command "cmake .. -G 'MinGW Makefiles' -Wno-dev -DCI_MODE=ON -DCOVERAGE=OFF" "$build_dir"
        Invoke-Command "mingw32-make" "$build_dir"

        Remove-PathFolder $mingw_path
    }

    'MSVC*'
    {
        if ($env:APPVEYOR)
        {
            $logger_arg = '/logger:"C:\Program Files\AppVeyor\BuildAgent\Appveyor.MSBuildLogger.dll"'
        }
        else
        {
            $logger_arg = ''
        }

        if ($env:Platform -like '*32')
        {
            $cmake_generator = "Visual Studio 15 Win32"
        }
        else
        {
            $cmake_generator = "Visual Studio 15 Win64"
        }

        Invoke-Command "cmake .. -G '$cmake_generator' -Wno-dev -DCI_MODE=ON -DCOVERAGE=OFF" "$build_dir"
        Invoke-Command "msbuild /ToolsVersion:15.0 $logger_arg build.vcxproj" "$build_dir"
    }
}
