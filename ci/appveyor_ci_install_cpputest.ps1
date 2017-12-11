# Load functions from the helper file
. (Join-Path (Split-Path $MyInvocation.MyCommand.Path) 'appveyor_helpers.ps1')

$cpputest_dir  = "C:\cpputest"

Start-FileDownload 'https://github.com/jgonzalezdr/cpputest/releases/download/v3.7.2-custom/cpputest-3.7.2-custom.zip'
New-Item -ItemType Directory -Force -Path "$cpputest_dir" | Out-Null
Invoke-Command '7z x -o$cpputest_dir cpputest-3.7.2-custom.zip'
