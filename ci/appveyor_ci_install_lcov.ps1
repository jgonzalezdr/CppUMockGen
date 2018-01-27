#
# Script to install LCOV in AppVeyor
#

# Load functions from the helper file
. (Join-Path (Split-Path $MyInvocation.MyCommand.Path) 'appveyor_helpers.ps1')

if ($env:Configuration -eq 'Coverage')
{
    Start-FileDownload 'https://github.com/jgonzalezdr/lcov/releases/download/v1.11w/lcov-v1.11w.rar'
    Invoke-Command "7z x -oC:\ lcov-v1.11w.rar"
}
