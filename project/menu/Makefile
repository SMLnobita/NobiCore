ARCHS = arm64
TARGET = iphone:clang:latest:13.0
FINALPACKAGE = 1
FOR_RELEASE = 1
include $(THEOS)/makefiles/common.mk
LIBRARY_NAME = Clone6
Clone6_FILES = ../src/Clone6.c ImGuiDrawView.mm IMGUI/imgui.cpp IMGUI/imgui_draw.cpp IMGUI/imgui_widgets.cpp IMGUI/imgui_tables.cpp IMGUI/imgui_impl_metal.mm
Clone6_FRAMEWORKS = UIKit Foundation Metal MetalKit QuartzCore CoreGraphics
Clone6_LIBRARIES = c++ objc
Clone6_CFLAGS = -O2 -DCLONE6_IMGUI -DMetalContext=C6MenuMetalContext -DMetalBuffer=C6MenuMetalBuffer -DFramebufferDescriptor=C6MenuFramebufferDescriptor -DImGuiDrawView=C6MenuImGuiDrawView -DNDEBUG -DIMGUI_DISABLE_METRICS_WINDOW -DIMGUI_DISABLE_DEMO_WINDOWS -Wno-deprecated-declarations
Clone6_CCFLAGS = -std=c++11 -fno-rtti -fno-exceptions
ImGuiDrawView.mm_CFLAGS = -fobjc-arc -fblocks
IMGUI/imgui_impl_metal.mm_CFLAGS = -fobjc-arc -fblocks
Clone6_INSTALL_PATH = /Library/Frameworks/Clone6.framework
include $(THEOS_MAKE_PATH)/library.mk
