#pragma once
#define KHTN_MENU_TITLE "Liên Quân Menu By Nguyễn Hoà"
static void ApplyKHTNTheme(){
 ImGui::StyleColorsDark();ImGuiStyle&s=ImGui::GetStyle();
 s.WindowRounding=12;s.FrameRounding=6;s.TabRounding=6;s.GrabRounding=6;s.WindowBorderSize=1;s.FrameBorderSize=0;
 s.WindowPadding=ImVec2(14,14);s.ItemSpacing=ImVec2(10,10);s.FramePadding=ImVec2(8,5);
 ImVec4*c=s.Colors;
 c[ImGuiCol_Text]=ImVec4(.92f,.96f,.98f,1);c[ImGuiCol_TextDisabled]=ImVec4(.50f,.61f,.68f,1);
 c[ImGuiCol_WindowBg]=ImVec4(.045f,.065f,.10f,.97f);c[ImGuiCol_Border]=ImVec4(.17f,.30f,.37f,.85f);
 c[ImGuiCol_TitleBg]=ImVec4(.055f,.105f,.16f,1);c[ImGuiCol_TitleBgActive]=ImVec4(.055f,.21f,.26f,1);c[ImGuiCol_TitleBgCollapsed]=ImVec4(.055f,.105f,.16f,.9f);
 c[ImGuiCol_FrameBg]=ImVec4(.09f,.14f,.20f,1);c[ImGuiCol_FrameBgHovered]=ImVec4(.12f,.23f,.30f,1);c[ImGuiCol_FrameBgActive]=ImVec4(.14f,.30f,.36f,1);
 c[ImGuiCol_CheckMark]=ImVec4(.24f,.88f,.76f,1);c[ImGuiCol_SliderGrab]=ImVec4(.22f,.78f,.80f,1);c[ImGuiCol_SliderGrabActive]=ImVec4(.40f,.96f,.86f,1);
 c[ImGuiCol_Header]=ImVec4(.09f,.25f,.32f,1);c[ImGuiCol_HeaderHovered]=ImVec4(.12f,.38f,.43f,1);c[ImGuiCol_HeaderActive]=ImVec4(.13f,.46f,.47f,1);
 c[ImGuiCol_Tab]=ImVec4(0,0,0,0);c[ImGuiCol_TabHovered]=c[ImGuiCol_Tab];c[ImGuiCol_TabActive]=ImVec4(.09f,.31f,.36f,1);
 c[ImGuiCol_TabUnfocused]=c[ImGuiCol_Tab];c[ImGuiCol_TabUnfocusedActive]=c[ImGuiCol_TabActive];
 c[ImGuiCol_Separator]=ImVec4(.16f,.27f,.34f,1);c[ImGuiCol_Button]=ImVec4(.10f,.24f,.31f,1);c[ImGuiCol_ButtonHovered]=ImVec4(.13f,.38f,.43f,1);c[ImGuiCol_ButtonActive]=ImVec4(.15f,.49f,.48f,1);
 c[ImGuiCol_ResizeGrip]=ImVec4(.22f,.78f,.80f,.3f);c[ImGuiCol_ResizeGripHovered]=ImVec4(.22f,.78f,.80f,.7f);c[ImGuiCol_ResizeGripActive]=ImVec4(.40f,.96f,.86f,1);
}
