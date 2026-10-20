#ifndef CLONE6_BRIDGE_H
#define CLONE6_BRIDGE_H
#ifdef __cplusplus
extern "C" {
#endif
typedef enum {C6_MAP,C6_CAMERA,C6_CAMERA_LOCK,C6_STREAM,C6_SKIN,C6_BP,C6_SMITE,C6_BOSS_ONLY,C6_HERO_NAME} C6Option;
typedef struct {int ready;int map,camera,camera_lock,stream,skin,bp,smite,boss_only,hero_name;float camera_distance,bp_percent;} C6Settings;
void C6GetSettings(C6Settings*out);
void C6SetOption(C6Option option,int enabled);
void C6SetCameraDistance(float value);
void C6SetBPPercent(float value);
void*C6ImGuiInstall(void);
#ifdef __cplusplus
}
#endif
#endif
