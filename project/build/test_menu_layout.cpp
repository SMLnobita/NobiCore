#include <stdio.h>
#include <math.h>
#include "../menu/IMGUI/imgui.h"
#include "../menu/IMGUI/zzz.h"
#include "../menu/KHTNTheme.h"
#include "../menu/KHTNMenuLayout.h"
#include "../src/Clone6Bridge.h"
static C6Settings state={1,0,0,0,0,0,0,1,1,0,.6f,13.7f};
static ImVec2 item_min[9],item_max[9];static int changes[9];
#define C6_MENU_TEST_ITEM(o,a,b) do{item_min[o]=a;item_max[o]=b;}while(0)
#include "../menu/Clone6MenuContent.h"
extern "C" void C6GetSettings(C6Settings*out){*out=state;}
extern "C" void C6SetOption(C6Option option,int value){int*fields[]={&state.map,&state.camera,&state.camera_lock,&state.stream,&state.skin,&state.bp,&state.smite,&state.boss_only,&state.hero_name};*fields[option]=value;changes[option]++;}
extern "C" void C6SetCameraDistance(float value){state.camera_distance=value;}
extern "C" void C6SetBPPercent(float value){state.bp_percent=value;}
static bool inside(ImVec2 pos,ImVec2 size,C6MenuViewport bounds){
 return pos.x>=bounds.min.x-.01f && pos.y>=bounds.min.y-.01f && pos.x+size.x<=bounds.max.x+.01f && pos.y+size.y<=bounds.max.y+.01f;
}
static void narrow_frame(){
 ImGui::NewFrame();ImGui::SetNextWindowPos(ImVec2(12,12),ImGuiCond_Always);ImGui::SetNextWindowSize(ImVec2(280,320),ImGuiCond_Always);
 ImGui::Begin("Narrow menu");DrawSettings(1);ImGui::End();ImGui::Render();
}
int main(){
 ImGui::CreateContext();ImGuiIO&io=ImGui::GetIO();io.IniFilename=nullptr;io.LogFilename=nullptr;io.DeltaTime=1.f/60;io.DisplaySize=ImVec2(400,400);
 io.Fonts->AddFontFromMemoryCompressedTTF((void*)zzz_compressed_data,zzz_compressed_size,17,nullptr,io.Fonts->GetGlyphRangesVietnamese());
 unsigned char*pixels;int w,h;io.Fonts->GetTexDataAsRGBA32(&pixels,&w,&h);ApplyKHTNTheme();
 for(int i=0;i<3;i++)narrow_frame();
 if(item_max[C6_BOSS_ONLY].x-item_min[C6_BOSS_ONLY].x>252.01f)return 1;
 if(item_max[C6_BOSS_ONLY].y-item_min[C6_BOSS_ONLY].y<ImGui::GetFontSize()*2)return 2;
 // The wrapped text and the square each toggle once and keep the same runtime option.
 io.MousePos=ImVec2(item_min[C6_BOSS_ONLY].x+70,item_min[C6_BOSS_ONLY].y+10);io.MouseDown[0]=true;narrow_frame();io.MouseDown[0]=false;narrow_frame();
 if(changes[C6_BOSS_ONLY]!=1 || state.boss_only)return 3;
 io.MousePos=ImVec2(item_min[C6_BOSS_ONLY].x+10,item_min[C6_BOSS_ONLY].y+10);io.MouseDown[0]=true;narrow_frame();io.MouseDown[0]=false;narrow_frame();
 if(changes[C6_BOSS_ONLY]!=2 || !state.boss_only)return 4;
 // Replay a dragged menu through rotation, using the same helper as the UIKit frontend.
 for(int frame=0;frame<9;frame++){
  io.DisplaySize=frame<4?ImVec2(844,390):ImVec2(375,812);
  C6MenuViewport safe=frame<4?C6MenuSafeViewport(io.DisplaySize,47,0,47,21):C6MenuSafeViewport(io.DisplaySize,0,44,0,34);
  ImGui::NewFrame();C6PrepareMenuLayout(KHTN_MENU_TITLE,safe);
  if(frame==2){ImGui::SetNextWindowPos(ImVec2(550,35),ImGuiCond_Always);ImGui::SetNextWindowSize(ImVec2(280,320),ImGuiCond_Always);}
  if(frame==7)ImGui::SetNextWindowCollapsed(true,ImGuiCond_Always);
  if(frame==8)ImGui::SetNextWindowCollapsed(false,ImGuiCond_Always);
  bool open=true;ImGui::Begin(KHTN_MENU_TITLE,&open);
  ImGuiWindow*window=ImGui::GetCurrentWindow();
  if(frame!=2 && !inside(window->Pos,window->SizeFull,safe))return 5;
  DrawSettings(1);ImGui::End();ImGui::Render();
 }
 // Small viewports must shrink the minimum window size rather than overflow.
 C6MenuViewport tiny=C6MenuSafeViewport(ImVec2(240,160),0,0,0,0);
 C6MenuGeometry fitted=C6ClampMenuGeometry(ImVec2(-100,500),ImVec2(440,320),tiny);
 if(!inside(fitted.position,fitted.size,tiny))return 6;
 ImGui::DestroyContext();puts("PASS: wrapped label, checkbox/text routing, safe-area rotation, collapsed/reopened size and small viewport.");return 0;
}
