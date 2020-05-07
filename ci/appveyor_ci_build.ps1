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
$test = if ($env:Test -eq 'False') {'OFF'} else {'ON'}

$cmake_options = "-Wno-dev -DCI_MODE=ON -DENABLE_TEST=$test"

if( $test -eq 'ON')
{
    $cmake_options += " -DCOVERAGE=$coverage -DCHECK_COMPILATION=$check_compilation"
}

if( $env:APPVEYOR_REPO_TAG -eq 'false' )
{
    $build_num = $env:APPVEYOR_BUILD_NUMBER
    $commit_id = $env:APPVEYOR_REPO_COMMIT.Substring(0,8)
    $repo_provider = $env:APPVEYOR_REPO_PROVIDER 
    $build_id = "$repo_provider-$commit_id"
    
    $cmake_options += " -DPROJECT_VERSION_BUILD=$build_num -DPROJECT_VERSION_SUFFIX=-$build_id -DINSTALLER_FILE_VERSION=Experimental -DPRERELEASE=ON -DPRIVATE_BUILD_INFO=$build_id"
}

switch -Wildcard ($env:Platform)
{
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

		$cmake_generator = "Visual Studio 16 2019"

        Invoke-Command "cmake .. -G '$cmake_generator' $cmake_options" "$build_dir"
        Invoke-Command "msbuild $logger_arg /property:Configuration=$build_config build.vcxproj" "$build_dir"
    }
	
    default
    {
        $mingw_path = Get-MinGWBin

        # Add mingw to the path
        Add-PathFolder $mingw_path

        Invoke-Command "cmake .. -G 'MinGW Makefiles' $cmake_options -DCMAKE_BUILD_TYPE=$build_config" "$build_dir"
        Invoke-Command "mingw32-make" "$build_dir"

        Remove-PathFolder $mingw_path
    }
}
