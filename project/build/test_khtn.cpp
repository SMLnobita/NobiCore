#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../menu/IMGUI/imgui.h"
#include "../menu/KHTNTheme.h"
#include "../menu/IMGUI/zzz.h"
#include "../src/Clone6Bridge.h"
static C6Settings state={1,0,0,0,0,0,0,0,1,0,.6f,13.7f};
static ImVec2 itemMin[9],itemMax[9];static int changes[9];
#define C6_MENU_TEST_ITEM(o,a,b) do{itemMin[o]=a;itemMax[o]=b;}while(0)
#include "../menu/Clone6MenuContent.h"
extern "C" void C6GetSettings(C6Settings*out){*out=state;}
extern "C" void C6SetOption(C6Option option,int value){int*fields[]={&state.map,&state.camera,&state.camera_lock,&state.stream,&state.skin,&state.bp,&state.smite,&state.boss_only,&state.hero_name};*fields[option]=value;changes[option]++;}
extern "C" void C6SetCameraDistance(float v){state.camera_distance=v;state.camera=1;}
extern "C" void C6SetBPPercent(float v){state.bp_percent=v;}
static void frame(int tab){ImGui::NewFrame();ImGui::SetNextWindowPos(ImVec2(12,12),ImGuiCond_Always);ImGui::SetNextWindowSize(ImVec2(440,ImGui::GetIO().DisplaySize.y-24),ImGuiCond_Always);ImGui::Begin(KHTN_MENU_TITLE);DrawSettings(tab);ImGui::End();ImGui::Render();}
static void capture(int tab){frame(tab);frame(tab);char path[64];snprintf(path,sizeof(path),"build/menu-preview-%d.bin",tab);FILE*f=fopen(path,"wb");ImDrawData*d=ImGui::GetDrawData();unsigned int count=d->CmdListsCount;fwrite(&count,4,1,f);for(int n=0;n<d->CmdListsCount;n++){ImDrawList*l=d->CmdLists[n];unsigned int sizes[3]={(unsigned int)l->VtxBuffer.Size,(unsigned int)l->IdxBuffer.Size,(unsigned int)l->CmdBuffer.Size};fwrite(sizes,4,3,f);fwrite(l->VtxBuffer.Data,sizeof(ImDrawVert),l->VtxBuffer.Size,f);fwrite(l->IdxBuffer.Data,sizeof(ImDrawIdx),l->IdxBuffer.Size,f);for(int c=0;c<l->CmdBuffer.Size;c++){ImDrawCmd&cmd=l->CmdBuffer[c];unsigned int values[]={cmd.ElemCount,cmd.IdxOffset,cmd.VtxOffset};fwrite(values,4,3,f);fwrite(&cmd.ClipRect,4,4,f);}}fclose(f);}
int main(){ImGui::CreateContext();ImGuiIO&io=ImGui::GetIO();io.IniFilename=nullptr;io.LogFilename=nullptr;
 const char*preview_height=getenv("C6_PREVIEW_H");int display_h=preview_height?atoi(preview_height):344;
 io.DisplaySize=ImVec2(464,display_h);io.DeltaTime=1.f/60;io.Fonts->AddFontFromMemoryCompressedTTF((void*)zzz_compressed_data,zzz_compressed_size,17,nullptr,io.Fonts->GetGlyphRangesVietnamese());unsigned char*texture;int w,h;io.Fonts->GetTexDataAsRGBA32(&texture,&w,&h);io.Fonts->SetTexID((void*)1);FILE*f=fopen("build/menu-font-atlas.bin","wb");fwrite(&w,4,1,f);fwrite(&h,4,1,f);fwrite(texture,4,w*h,f);fclose(f);ApplyKHTNTheme();
 state.smite=1;
 for(int tab=0;tab<3;tab++)capture(tab);
 int tabs[9]={0,0,0,3,2,1,1,1,2};
 for(int option=0;option<9;option++){if(option==C6_STREAM)continue;frame(tabs[option]);frame(tabs[option]);io.MousePos=ImVec2((itemMin[option].x+itemMax[option].x)/2,(itemMin[option].y+itemMax[option].y)/2);io.MouseDown[0]=true;frame(tabs[option]);io.MouseDown[0]=false;frame(tabs[option]);if(changes[option]!=1){printf("Control routing failed: %d / %d\n",option,changes[option]);return 1;}}
 puts("All 8 menu controls dispatch to their corresponding runtime options.");return 0;
}
