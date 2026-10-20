#pragma once
#include "IMGUI/imgui_internal.h"

struct C6MenuViewport {ImVec2 min,max;};
struct C6MenuGeometry {ImVec2 position,size;};

static inline C6MenuViewport C6MenuSafeViewport(ImVec2 display,float left,float top,float right,float bottom){
 const float margin=16.f;
 ImVec2 lo(ImMin(ImMax(0.f,left)+margin,ImMax(0.f,display.x-1.f)),
           ImMin(ImMax(0.f,top)+margin,ImMax(0.f,display.y-1.f)));
 ImVec2 hi(ImMax(lo.x+1.f,display.x-ImMax(0.f,right)-margin),
           ImMax(lo.y+1.f,display.y-ImMax(0.f,bottom)-margin));
 return {lo,hi};
}

static inline C6MenuGeometry C6ClampMenuGeometry(ImVec2 position,ImVec2 size,C6MenuViewport viewport){
 const ImVec2 available(viewport.max.x-viewport.min.x,viewport.max.y-viewport.min.y);
 size.x=ImClamp(size.x,ImMin(280.f,available.x),available.x);
 size.y=ImClamp(size.y,ImMin(180.f,available.y),available.y);
 position.x=ImClamp(position.x,viewport.min.x,viewport.max.x-size.x);
 position.y=ImClamp(position.y,viewport.min.y,viewport.max.y-size.y);
 return {position,size};
}

/* Call before Begin, using the view's current size and UIWindow safeAreaInsets.
 * Re-evaluate every visible frame, including the first frame after reopening. */
static inline void C6PrepareMenuLayout(const char*title,C6MenuViewport viewport){
 const ImVec2 available(viewport.max.x-viewport.min.x,viewport.max.y-viewport.min.y);
 ImGui::SetNextWindowSizeConstraints(ImVec2(ImMin(280.f,available.x),ImMin(180.f,available.y)),available);
 ImGuiWindow*window=ImGui::FindWindowByName(title);
 if(window){
  const C6MenuGeometry next=C6ClampMenuGeometry(window->Pos,window->SizeFull,viewport);
  if(next.position.x!=window->Pos.x || next.position.y!=window->Pos.y)ImGui::SetNextWindowPos(next.position,ImGuiCond_Always);
  if(next.size.x!=window->SizeFull.x || next.size.y!=window->SizeFull.y)ImGui::SetNextWindowSize(next.size,ImGuiCond_Always);
 }else{
  const ImVec2 size(ImMin(440.f,available.x),ImMin(320.f,available.y));
  const ImVec2 position(viewport.min.x+(available.x-size.x)*.5f,viewport.min.y+(available.y-size.y)*.5f);
  ImGui::SetNextWindowPos(position,ImGuiCond_FirstUseEver);ImGui::SetNextWindowSize(size,ImGuiCond_FirstUseEver);
 }
}
