
<#
.SYNOPSIS
Decode raw esp32 backtraces in a log file. 
Requires PlatformIO build environment.

.DESCRIPTION
Capture a log file with raw 'Backtrace:' data and pass it to this script.
The script filters the file, replacing the raw backtrace entries with decoded entries.
The filtered data is written to stdout.

.PARAMETER firmwarePath
The path to the elf firmware for the device, typically sometyhing like: .pio/build/featheresp32/firmware.elf
Your path will be specific your device type and configureation. 

.PARAMETER file
The path to your logfile.

.PARAMETER pioRoot
If you installed PlatformIO in a non standard location, (not $env:USERPROFILE) you need to remedy that here.
.EXAMPLE
espdecode -file ".\espdecodetest.txt" -firmwarePath ".\.pio\build\featheresp32\firmware.elf" > log.txt
====================
.\espdecode.ps1 -file ".\espdecodetest.txt" -firmwarePath ".\.pio\build\nodemcu-32s\firmware.elf" > log.txt
================
./espdecode.ps1 -file "./espdecodetest.txt" -firmwarePath "./.pio/build/esp32-s3-devkitc-1/firmware.elf" > ./log.txt

cái này trước để lấy quyền chạy 
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope Process

#>
param(
    [Parameter(Mandatory=$true)]
    [string]$firmwarePath,
    [Parameter(Mandatory=$true)]
    [string]$file,
    [string]$pioRoot=$env:USERPROFILE
)

# $decode = "$pioRoot/.platformio/packages/toolchain-xtensa-esp32\bin\xtensa-esp32-elf-addr2line.exe"

$decode = "/home/johnR/.platformio/packages/toolchain-xtensa-esp32s3/bin/xtensa-esp32s3-elf-addr2line"

function decodeStack 
{
    param(
        [string] $stackline
    )
    $stackline -split ' ' | foreach-object {
        & $decode -fp -e $firmwarePath $_
    }
}

    
try {   
    $content = get-content -LiteralPath $file
    foreach ($line in $content) {
        if ($line -match '^Backtrace:.*') {
            $b = $line -split "Backtrace:"
            "Backtrace:"
            decodeStack $b[1].trim()

        } else {
            $line
        }
    }
}
catch {
       Write-Host  ($_.Exception.Message -split ' For')[0] -ForegroundColor Red

}