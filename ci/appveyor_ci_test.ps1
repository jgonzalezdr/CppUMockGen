#
# Script to manage tests in AppVeyor
#

# Load functions from the helper file
. (Join-Path (Split-Path $MyInvocation.MyCommand.Path) 'appveyor_helpers.ps1')

function Publish-TestResults($files)
{
    $anyFailures = $FALSE

    # Upload results to AppVeyor one by one
    $files | foreach {
        $testsuite = ([xml](get-content $_)).testsuite

        foreach ($testcase in $testsuite.testcase) {
            [double]$testtimef = $testcase.time
            [int]$testtimei = $testtimef * 1000
            if ($testcase.failure) {
                Add-AppveyorTest $testcase.name -Outcome Failed -FileName $testsuite.name -ErrorMessage $testcase.failure.message -Duration $testtimei
                Add-AppveyorMessage "$($testcase.name) failed" -Category Error
                $anyFailures = $TRUE
            }
            elseif ($testcase.skipped) {
                Add-AppveyorTest $testcase.name -Outcome Ignored -Filename $testsuite.name
            }
            else {
                Add-AppveyorTest $testcase.name -Outcome Passed -FileName $testsuite.name -Duration $testtimei
            }
        }

        Remove-Item $_
    }

    if ($anyFailures -eq $TRUE){
        Write-Host -ForegroundColor Red "Failing build as there are broken tests"
        $host.SetShouldExit(1)
    }
}

$build_dir = "build"

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

switch -Wildcard ($env:Platform)
{
    'MinGW*'
    {
        $mingw_path = Get-MinGWBin

        # Add mingw to the path
        Add-PathFolder $mingw_path

        Invoke-Command "mingw32-make run_tests" "$build_dir"

        Remove-PathFolder $mingw_path
    }

    'MSVC*'
    {
        Invoke-Command "msbuild /ToolsVersion:15.0 $logger_arg run_tests.vcxproj" "$build_dir\test"
    }
}

Publish-TestResults (Get-ChildItem -Include 'cpputest_*.xml' -Recurse -Name)

if (-not $env:APPVEYOR)
{
    Write-Host "Tests Ran: $TestCount"
}
