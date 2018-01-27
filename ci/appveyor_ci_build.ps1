#
# Script to manage builds in AppVeyor
#

# Load functions from the helper file
. (Join-Path (Split-Path $MyInvocation.MyCommand.Path) 'appveyor_helpers.ps1')

# Clean up some paths for any configuration
Remove-PathFolder "C:\MinGW\bin"
Remove-PathFolder "C:\Program Files\Git\bin"
Remove-PathFolder "C:\Program Files\Git\cmd"
Remove-PathFolder "C:\Program Files\Git\usr\bin"
Remove-PathFolder "C:\Program Files (x86)\Git\bin"
Remove-PathFolder "C:\Program Files (x86)\Git\cmd"
Remove-PathFolder "C:\Program Files (x86)\Git\usr\bin"

$build_dir = "build"

New-Item -ItemType Directory -Force -Path "$build_dir" | Out-Null

$build_config = $env:Configuration
$coverage = if ($env:Configuration -eq 'Coverage') {'ON'} else {'OFF'}
$check_compilation = if ($env:CheckCompilation) {'ON'} else {'OFF'}

$cmake_options = "-Wno-dev -DCI_MODE=ON -DCOVERAGE=$coverage -DCHECK_COMPILATION=$check_compilation"

switch -Wildcard ($env:Platform)
{
    'MinGW*'
    {
        $mingw_path = Get-MinGWBin

        # Add mingw to the path
        Add-PathFolder $mingw_path

        Invoke-Command "cmake .. -G 'MinGW Makefiles' $cmake_options -DCMAKE_BUILD_TYPE=$build_config" "$build_dir"
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

        Invoke-Command "cmake .. -G '$cmake_generator' $cmake_options" "$build_dir"
        Invoke-Command "msbuild /ToolsVersion:15.0 $logger_arg build.vcxproj" "$build_dir"
    }
}
