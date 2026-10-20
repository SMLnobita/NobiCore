#!/bin/sh
set -eu
cd "$(dirname "$0")"
sdk="$(xcrun --sdk iphoneos --show-sdk-path)"
mkdir -p build/imgui-macos
xcrun --sdk iphoneos clang -arch arm64 -isysroot "$sdk" -miphoneos-version-min=13.0 -O2 -DCLONE6_IMGUI -fno-builtin -c src/Clone6.c -o build/imgui-macos/runtime.o
for file in menu/ImGuiDrawView.mm menu/IMGUI/imgui.cpp menu/IMGUI/imgui_draw.cpp menu/IMGUI/imgui_widgets.cpp menu/IMGUI/imgui_tables.cpp menu/IMGUI/imgui_impl_metal.mm; do
  name="$(basename "$file")"; arc=""
  case "$file" in *.mm) arc="-fobjc-arc -fblocks";; esac
  xcrun --sdk iphoneos clang++ -arch arm64 -isysroot "$sdk" -miphoneos-version-min=13.0 -DMetalContext=C6MenuMetalContext -DMetalBuffer=C6MenuMetalBuffer -DFramebufferDescriptor=C6MenuFramebufferDescriptor -DImGuiDrawView=C6MenuImGuiDrawView -std=c++11 -O2 -fno-rtti -fno-exceptions -DNDEBUG -Wno-deprecated-declarations $arc -c "$file" -o "build/imgui-macos/$name.o"
done
xcrun --sdk iphoneos clang++ -arch arm64 -isysroot "$sdk" -miphoneos-version-min=13.0 -dynamiclib build/imgui-macos/*.o -framework Foundation -framework UIKit -framework Metal -framework MetalKit -framework QuartzCore -framework CoreGraphics -Wl,-install_name,@rpath/Clone6.framework/Clone6 -Wl,-headerpad,0x400 -o build/Clone6-khtn-raw
