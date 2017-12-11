
# Load functions from the helper file
. (Join-Path (Split-Path $MyInvocation.MyCommand.Path) 'appveyor_helpers.ps1')

function Invoke-CygwinCommand($command, $directory = '.')
{
    # Assume cygwin is located at C:\cygwin on x86 and C:\cygwin64 on x64 for now
    $cygwin_bin = Get-CygwinBin

    $cygwin_directory = (. "${cygwin_bin}\cygpath.exe" (Resolve-Path $directory))
    $command_wrapped = "${cygwin_bin}\bash.exe --login -c 'cd $cygwin_directory ; $command'"
    
    Write-Host "Executing <$command> in <$cygwin_directory>"
    Invoke-Expression $command_wrapped

    if ($LASTEXITCODE -ne 0)
    {
        Write-Host "Command Returned error: $LASTEXITCODE"
        Exit $LASTEXITCODE
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

$env:CPPUTEST_HOME='C:\cpputest\'

$build_dir = "build"

New-Item -ItemType Directory -Force -Path "$build_dir" | Out-Null

switch -Wildcard ($env:Platform)
{
    'Cygwin*'
    {
        Invoke-CygwinCommand "autoreconf -i .." "$build_dir"
        Invoke-CygwinCommand "../configure" "$build_dir"
        Invoke-CygwinCommand "make CppUTestTests.exe CppUTestExtTests.exe" "$build_dir"
    }

    'MinGW*'
    {
        $mingw_path = Get-MinGWBin

        # Add mingw to the path
        Add-PathFolder $mingw_path

        Invoke-Command "cmake -G 'MinGW Makefiles' .." '$build_dir'
        Invoke-Command "mingw32-make all" '$build_dir'

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
