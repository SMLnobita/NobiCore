#pragma once
static void Feature(const char*label,C6Option option,int value){
 bool v=value!=0,changed=false;
 const float needed=ImGui::GetFrameHeight()+ImGui::GetStyle().ItemInnerSpacing.x+ImGui::CalcTextSize(label).x;
 if(needed<=ImGui::GetContentRegionAvail().x)changed=ImGui::Checkbox(label,&v);
 else{
  ImGui::PushID(label);ImGui::BeginGroup();changed=ImGui::Checkbox("##value",&v);
  ImGui::SameLine();ImGui::AlignTextToFramePadding();ImGui::PushTextWrapPos(ImGui::GetWindowContentRegionMax().x);
  ImGui::TextUnformatted(label);ImGui::PopTextWrapPos();
  if(ImGui::IsItemClicked()){v=!v;changed=true;}
  ImGui::EndGroup();ImGui::PopID();
 }
 if(changed)C6SetOption(option,v);
#ifdef C6_MENU_TEST_ITEM
 C6_MENU_TEST_ITEM(option,ImGui::GetItemRectMin(),ImGui::GetItemRectMax());
#endif
}
static void DrawSettings(int selectedTab=-1){
 C6Settings s;C6GetSettings(&s);
 if(s.ready!=1){ImGui::TextUnformatted(s.ready<0?"Phiên bản game chưa được hỗ trợ":"Đang khởi tạo...");return;}
 if(!ImGui::BeginTabBar("MainTabBar"))return;
 if(ImGui::BeginTabItem("Map & camera",nullptr,selectedTab==0?ImGuiTabItemFlags_SetSelected:0)){
  Feature("Hack map",C6_MAP,s.map);Feature("Camera",C6_CAMERA,s.camera);ImGui::SameLine();Feature("Khóa camera",C6_CAMERA_LOCK,s.camera_lock);
  float v=s.camera_distance;if(ImGui::SliderFloat("Gần - xa",&v,-2,5,"%.2f"))C6SetCameraDistance(v);ImGui::EndTabItem();
 }
 if(ImGui::BeginTabItem("Bổ trợ",nullptr,selectedTab==1?ImGuiTabItemFlags_SetSelected:0)){
  Feature("Tự Bộc phá",C6_BP,s.bp);float v=s.bp_percent;if(ImGui::SliderFloat("Máu địch",&v,1,100,"%.1f%%"))C6SetBPPercent(v);
  ImGui::Separator();Feature("Tự Trừng trị / Tê tái",C6_SMITE,s.smite);Feature("Chỉ Rồng / Tà Thần / KingKong / Rồng bạo chúa",C6_BOSS_ONLY,s.boss_only);ImGui::EndTabItem();
 }
 if(ImGui::BeginTabItem("Skin & tên",nullptr,selectedTab==2?ImGuiTabItemFlags_SetSelected:0)){Feature("Mod skin",C6_SKIN,s.skin);Feature("Hiện tên tướng",C6_HERO_NAME,s.hero_name);ImGui::EndTabItem();}

 ImGui::EndTabBar();
}
