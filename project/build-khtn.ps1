param(
 [string]$InstallName='@rpath/Clone6.framework/Clone6'
)
$ErrorActionPreference = 'Stop'
$cloneRoot=$PSScriptRoot
$zigExe=Join-Path $cloneRoot 'build\zig013\ziglang\zig.exe'
$env:ZIG_LIB_DIR=Join-Path $cloneRoot 'build\zig013\ziglang\lib'
$iosSdk=Join-Path $cloneRoot 'build\apple-sdks\iPhoneOS16.5.sdk'
$objects=Join-Path $cloneRoot 'build\imgui-objects'
New-Item -ItemType Directory -Force -Path $objects | Out-Null
$common=@('-target','aarch64-ios.13.0','-isysroot',$iosSdk,'-isystem',"$iosSdk\usr\include",'-iframework',"$iosSdk\System\Library\Frameworks",'-O2','-fno-sanitize=all','-fno-stack-protector','-fno-exceptions','-fno-rtti','-DNDEBUG','-DIMGUI_DISABLE_METRICS_WINDOW','-DIMGUI_DISABLE_DEMO_WINDOWS','-Wno-deprecated-declarations','-DMetalContext=C6MenuMetalContext','-DMetalBuffer=C6MenuMetalBuffer','-DFramebufferDescriptor=C6MenuFramebufferDescriptor','-DImGuiDrawView=C6MenuImGuiDrawView')
$cxx=@('-include',"$cloneRoot\build\ios-cxx-compat.h",'-std=c++11','-nostdinc++','-isystem',"$iosSdk\usr\include\c++\v1")
& $zigExe cc @common -DCLONE6_IMGUI -Wall -Wextra -Werror -fno-builtin -c (Join-Path $cloneRoot 'src\Clone6.c') -o "$objects\runtime.o"
if($LASTEXITCODE -ne 0){throw 'Runtime compilation failed'}
$sources=@('menu\ImGuiDrawView.mm','menu\IMGUI\imgui.cpp','menu\IMGUI\imgui_draw.cpp','menu\IMGUI\imgui_widgets.cpp','menu\IMGUI\imgui_tables.cpp','menu\IMGUI\imgui_impl_metal.mm')
foreach($relative in $sources){
 $name=[IO.Path]::GetFileNameWithoutExtension($relative)
 $arc=@();if($relative.EndsWith('.mm')){$arc=@('-fobjc-arc','-fblocks')}
 $owned=@();if($relative -eq 'menu\ImGuiDrawView.mm'){$owned=@('-Wall','-Wextra','-Werror')}
 & $zigExe cc @cxx @common @arc @owned -c (Join-Path $cloneRoot $relative) -o "$objects\$name.o"
 if($LASTEXITCODE -ne 0){throw "Compilation failed: $relative"}
 Write-Output "Compiled $relative"
}
$compiled=Get-ChildItem -LiteralPath $objects -Filter '*.o' | ForEach-Object FullName
& $zigExe cc -target aarch64-ios.13.0 -isysroot $iosSdk -nostdlib -shared @compiled "-L$iosSdk\usr\lib" "-F$iosSdk\System\Library\Frameworks" -lobjc -lSystem "$iosSdk\usr\lib\libc++.tbd" -framework Foundation -framework UIKit -framework Metal -framework MetalKit -framework QuartzCore -framework CoreGraphics "-Wl,-install_name,$InstallName" '-Wl,-headerpad,0x400' -o (Join-Path $cloneRoot 'build\Clone6-khtn-raw')
if($LASTEXITCODE -ne 0){throw "ImGui module link failed: exit $LASTEXITCODE"}
Write-Output 'ImGui ARM64 module compiled and linked.'
