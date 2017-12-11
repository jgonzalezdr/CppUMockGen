
# Load functions from the helper file
. (Join-Path (Split-Path $MyInvocation.MyCommand.Path) 'appveyor_helpers.ps1')

function Publish-TestResults($files)
{
    $anyFailures = $FALSE

    # Upload results to AppVeyor one by one
    $files | foreach {
        $testsuite = ([xml](get-content $_)).testsuite

        foreach ($testcase in $testsuite.testcase) {
            [double]$testtimef = $testsuite.time
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

function Invoke-CygwinTests($executable)
{
    # Assume cygwin is located at C:\cygwin for now
    $cygwin_bin = Get-CygwinBin

    # Get the full path to the executable
    $cygwin_folder = . "${cygwin_bin}\cygpath.exe" (Resolve-Path ".")
    $cygwin_exe = . "${cygwin_bin}\cygpath.exe" $executable
    
    # Run tests from the cygwin prompt
    $test_command = "${cygwin_exe} -ojunit"
    $cygwin_command = "${cygwin_bin}\bash.exe --login -c 'cd ${cygwin_folder} ; ${test_command}'"

    Write-Host $test_command
    Invoke-Expression $cygwin_command
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
    'Cygwin*'
    {
        Invoke-CygwinTests 'cpputest_build\CppUTestTests.exe'
        Invoke-CygwinTests 'cpputest_build\CppUTestExtTests.exe'
    }

    'MinGW*'
    {
        $mingw_path = Get-MinGWBin

        Add-PathFolder $mingw_path
        Invoke-Tests '.\cpputest_build\tests\CppUTestTests.exe'
        Invoke-Tests '.\cpputest_build\tests\CppUTestExt\CppUTestExtTests.exe'
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
