#
# Script to manage tests in AppVeyor
#

# Load functions from the helper file
. (Join-Path (Split-Path $MyInvocation.MyCommand.Path) 'appveyor_helpers.ps1')

function Publish-TestResults($files)
{
    $anyFailures = $FALSE

    # Upload results to AppVeyor one by one
    foreach ($file in $files)
    {
        Write-Host "Analyzing test results file '$file'" 
    
        $testsuite = ([xml](get-content $file)).testsuite

        foreach ($testcase in $testsuite.testcase)
        {
            [double]$testtimef = $testcase.time
            [int]$testtimei = $testtimef * 1000
            if ($testcase.failure)
            {
                Add-AppveyorTest $testcase.name -Outcome Failed -FileName $testsuite.name -ErrorMessage $testcase.failure.message -Duration $testtimei
                Add-AppveyorMessage "$($testcase.name) failed" -Category Error
                $anyFailures = $TRUE
            }
            elseif ($testcase.skipped)
            {
                Add-AppveyorTest $testcase.name -Outcome Ignored -Filename $testsuite.name
            }
            else
            {
                Add-AppveyorTest $testcase.name -Outcome Passed -FileName $testsuite.name -Duration $testtimei
            }
        }

        Remove-Item $file
    }

    if ($anyFailures -eq $TRUE)
    {
        Write-Host -ForegroundColor Red "Failing build as there are broken tests"
        $host.SetShouldExit(1)
    }
}

$TestCount = 0

if (-not $env:APPVEYOR)
{
    function Add-AppveyorTest()
    {
        # Wacky way to access a script variable, but it works
        $count = Get-Variable -Name TestCount -Scope script
        Set-Variable -Name TestCount -Scope script -Value ($count.Value + 1)
    }

    function Add-AppveyorMessage($Message, $Category)
    {
        if ($Category -eq 'Error')
        {
            Write-Error $Message
        }
        else
        {
            Write-Host $Message
        }
    }
}

if (!($env:Test -eq 'False'))
{
    $build_dir = "build"
    $build_config = $env:Configuration
    
    switch -Wildcard ($env:Platform)
    {
        'MSVC*'
        {
            $msbuild_cmd = Get-MsBuildCmd

            Invoke-Command "$msbuild_cmd $logger_arg /property:Configuration=$build_config execute_tests.vcxproj" "$build_dir\test"
        }
		
        'LINUX-GCC'
        {
            $build_target = if ($env:Configuration -eq 'Coverage') {'coverage_process'} else {'execute_tests'}

            Invoke-Command "make $build_target" "$build_dir"
        }
        
        default
        {
            $mingw_path = Get-MinGWBin

            # Add mingw to the path
            Add-PathFolder $mingw_path

            $build_target = if ($env:Configuration -eq 'Coverage') {'coverage_process'} else {'execute_tests'}

            Invoke-Command "mingw32-make $build_target" "$build_dir"

            Remove-PathFolder $mingw_path
        }
    }

    Publish-TestResults (Get-ChildItem -Include 'cpputest_*.xml' -Recurse -Name)

    if (-not $env:APPVEYOR)
    {
        Write-Host "Tests Ran: $TestCount"
    }
}
