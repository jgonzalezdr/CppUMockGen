#
# Script to after tests phase in AppVeyor
#

$build_dir = "build"

if (-not $env:APPVEYOR)
{
    function Push-AppveyorArtifact($Filename)
    {
        Write-Host "Publishing artifact: $Filename"
    }
}

if ($env:PublishArtifacts)
{
    Get-ChildItem $build_dir\app\*.exe | % { Push-AppveyorArtifact $_.FullName -FileName $_.Name }
}
