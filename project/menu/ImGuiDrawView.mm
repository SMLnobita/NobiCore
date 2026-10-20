#import <UIKit/UIKit.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import "Esp/ImGuiDrawView.h"
#include "IMGUI/imgui.h"
#include "IMGUI/imgui_internal.h"
#include "KHTNTheme.h"
#include "KHTNMenuLayout.h"
#include "IMGUI/imgui_impl_metal.h"
#include "IMGUI/zzz.h"
#include "../src/Clone6Bridge.h"
static bool menuVisible=false;
static CGRect menuRect=CGRectZero;
static ImGuiContext* menuContext;
@interface C6MetalView:MTKView
@end
@implementation C6MetalView
-(BOOL)pointInside:(CGPoint)p withEvent:(UIEvent*)e{(void)e;return menuVisible&&CGRectContainsPoint(menuRect,p);}
@end
@interface KHTNOverlayView:UIView
@property(nonatomic,weak)UIView*metalView;
@end
@implementation KHTNOverlayView
-(UIView*)hitTest:(CGPoint)p withEvent:(UIEvent*)e{
 if(self.hidden||!self.userInteractionEnabled||!menuVisible)return nil;
 UIView*v=self.metalView;if(!v||v.hidden)return nil;
 return [v hitTest:[v convertPoint:p fromView:self] withEvent:e];
}
-(void)layoutSubviews{[super layoutSubviews];self.metalView.frame=self.bounds;}
@end
@interface ImGuiDrawView()<MTKViewDelegate,UIGestureRecognizerDelegate>
@property(nonatomic,strong)id<MTLDevice>device;
@property(nonatomic,strong)id<MTLCommandQueue>commandQueue;
@property(nonatomic,strong)KHTNOverlayView*overlayRoot;
@property(nonatomic,strong)UITapGestureRecognizer*menuGesture;
@end
#include "Clone6MenuContent.h"
@implementation ImGuiDrawView
+(void)showChange:(BOOL)open{menuVisible=open;}
-(instancetype)initWithNibName:(NSString*)name bundle:(NSBundle*)bundle{
 self=[super initWithNibName:name bundle:bundle];if(!self)return nil;
 _device=MTLCreateSystemDefaultDevice();_commandQueue=[_device newCommandQueue];if(!_device||!_commandQueue)return nil;
 IMGUI_CHECKVERSION();menuContext=ImGui::CreateContext();ImGui::SetCurrentContext(menuContext);ImGuiIO&io=ImGui::GetIO();io.IniFilename=nullptr;io.LogFilename=nullptr;
 io.Fonts->AddFontFromMemoryCompressedTTF((void*)zzz_compressed_data,zzz_compressed_size,17,nullptr,io.Fonts->GetGlyphRangesVietnamese());
 ApplyKHTNTheme();
 if(!ImGui_ImplMetal_Init(_device))return nil;return self;
}
-(void)loadView{
 C6MetalView*v=[[C6MetalView alloc]initWithFrame:UIScreen.mainScreen.bounds device:self.device];v.delegate=self;v.multipleTouchEnabled=YES;v.hidden=!menuVisible;v.paused=!menuVisible;v.preferredFramesPerSecond=60;v.opaque=NO;v.backgroundColor=UIColor.clearColor;v.clearColor=MTLClearColorMake(0,0,0,0);v.autoresizingMask=UIViewAutoresizingFlexibleWidth|UIViewAutoresizingFlexibleHeight;self.view=v;
}
-(void)toggleMenu:(UITapGestureRecognizer*)gesture{
 if(gesture.state!=UIGestureRecognizerStateRecognized)return;
 menuVisible=!menuVisible;menuRect=CGRectZero;self.view.hidden=!menuVisible;((MTKView*)self.view).paused=!menuVisible;
 ImGui::SetCurrentContext(menuContext);ImGui::GetIO().MouseDown[0]=false;ImGui::ClearActiveID();
}
-(BOOL)gestureRecognizer:(UIGestureRecognizer*)g shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer*)other{(void)g;(void)other;return YES;}
-(BOOL)mountInWindow:(UIWindow*)w{
 self.overlayRoot=[[KHTNOverlayView alloc]initWithFrame:w.bounds];self.overlayRoot.backgroundColor=UIColor.clearColor;self.overlayRoot.autoresizingMask=UIViewAutoresizingFlexibleWidth|UIViewAutoresizingFlexibleHeight;
 self.overlayRoot.metalView=self.view;[self.overlayRoot addSubview:self.view];[w addSubview:self.overlayRoot];[self.overlayRoot setNeedsLayout];[self.overlayRoot layoutIfNeeded];
 // Install on UIWindow so the gesture still receives game touches while menu is hidden.
 self.menuGesture=[[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(toggleMenu:)];
 self.menuGesture.numberOfTouchesRequired=3;self.menuGesture.numberOfTapsRequired=2;
 self.menuGesture.cancelsTouchesInView=NO;self.menuGesture.delaysTouchesBegan=NO;self.menuGesture.delaysTouchesEnded=NO;self.menuGesture.delegate=self;
 [w addGestureRecognizer:self.menuGesture];return YES;
}
-(void)updateIOWithTouchEvent:(UIEvent*)e{
 ImGui::SetCurrentContext(menuContext);
 if(e.allTouches.count>1){ImGui::GetIO().MouseDown[0]=false;ImGui::ClearActiveID();return;}
 CGPoint p=[e.allTouches.anyObject locationInView:self.view];ImGuiIO&io=ImGui::GetIO();io.MousePos=ImVec2(p.x,p.y);io.MouseDown[0]=false;
 for(UITouch*t in e.allTouches)if(t.phase!=UITouchPhaseEnded&&t.phase!=UITouchPhaseCancelled)io.MouseDown[0]=true;
}
-(void)touchesBegan:(NSSet<UITouch*>*)t withEvent:(UIEvent*)e{(void)t;[self updateIOWithTouchEvent:e];}
-(void)touchesMoved:(NSSet<UITouch*>*)t withEvent:(UIEvent*)e{(void)t;[self updateIOWithTouchEvent:e];}
-(void)touchesEnded:(NSSet<UITouch*>*)t withEvent:(UIEvent*)e{(void)t;[self updateIOWithTouchEvent:e];}
-(void)touchesCancelled:(NSSet<UITouch*>*)t withEvent:(UIEvent*)e{(void)t;[self updateIOWithTouchEvent:e];}
-(void)drawInMTKView:(MTKView*)v{
 if(v.bounds.size.width<=0 || v.bounds.size.height<=0)return;
 ImGui::SetCurrentContext(menuContext);
 ImGuiIO&io=ImGui::GetIO();io.DisplaySize=ImVec2(v.bounds.size.width,v.bounds.size.height);CGFloat scale=v.window.screen.scale?:UIScreen.mainScreen.scale;io.DisplayFramebufferScale=ImVec2(scale,scale);io.DeltaTime=1.f/(float)(v.preferredFramesPerSecond?:60);
 id<MTLCommandBuffer>b=[self.commandQueue commandBuffer];MTLRenderPassDescriptor*p=v.currentRenderPassDescriptor;
 if(p&&b){id<MTLRenderCommandEncoder>encoder=[b renderCommandEncoderWithDescriptor:p];ImGui_ImplMetal_NewFrame(p);ImGui::NewFrame();
  if(menuVisible){
   UIEdgeInsets safe=v.window?v.window.safeAreaInsets:v.safeAreaInsets;
   C6PrepareMenuLayout(KHTN_MENU_TITLE,C6MenuSafeViewport(io.DisplaySize,(float)safe.left,(float)safe.top,(float)safe.right,(float)safe.bottom));
   bool expanded=ImGui::Begin(KHTN_MENU_TITLE,&menuVisible);ImVec2 pos=ImGui::GetWindowPos(),size=ImGui::GetWindowSize();menuRect=CGRectMake(pos.x,pos.y,size.x,size.y);if(expanded)DrawSettings();ImGui::End();
  }else menuRect=CGRectZero;
  ImGui::Render();ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(),b,encoder);[encoder endEncoding];if(v.currentDrawable)[b presentDrawable:v.currentDrawable];
 }[b commit];if(!menuVisible){v.hidden=YES;v.paused=YES;}
}
-(void)mtkView:(MTKView*)v drawableSizeWillChange:(CGSize)s{(void)v;(void)s;}
@end
static ImGuiDrawView*installedController;
extern "C" void*C6ImGuiInstall(void){
 if(installedController)return(__bridge void*)installedController.view;UIWindow*w=nil;
 for(UIWindow*c in UIApplication.sharedApplication.windows)if(c.isKeyWindow){w=c;break;}if(!w)return nullptr;
 ImGuiDrawView*c=[[ImGuiDrawView alloc]initWithNibName:nil bundle:nil];if(!c||![c mountInWindow:w])return nullptr;installedController=c;return(__bridge void*)c.view;
}
