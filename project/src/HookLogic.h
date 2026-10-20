#ifndef CLONE6_HOOK_LOGIC_H
#define CLONE6_HOOK_LOGIC_H
typedef unsigned char u8; typedef unsigned int u32; typedef unsigned long long u64;
typedef __SIZE_TYPE__ usize; typedef __PTRDIFF_TYPE__ isize; typedef _Bool bool;
static _Atomic bool map_on, camera_on, lock_on, history_on;
static _Atomic float camera_add=0.6f;
static _Atomic(void*) original_map,original_zoom,original_update,original_history;
static void (*runtime_frame)(void);
static void (*refresh_camera)(void*,void*);
static _Atomic u32 camera_revision=1;
static _Atomic u32 map_revision=1;
static u32 camera_applied_revision;
static void *camera_applied_instance;
/* Keep MethodInfo in the signature. It must not be lost through a variadic hook. */
static bool map_hook(void*self,u32 camp,bool visible,bool sync,void*method){
 if(self && map_on && (camp==1 || camp==2 || camp==255 || camp==110))visible=1;
 return ((bool(*)(void*,u32,bool,bool,void*))original_map)(self,camp,visible,sync,method);
}
static float zoom_hook(void*self,void*method){
 float value=((float(*)(void*,void*))original_zoom)(self,method);
 return self && camera_on ? value+camera_add : value;
}
static void update_hook(void*self,void*method){
 if(runtime_frame)runtime_frame();
 u32 revision=camera_revision;
 if(self && refresh_camera && (camera_applied_instance!=self || camera_applied_revision!=revision)){
  camera_applied_instance=self;camera_applied_revision=revision;refresh_camera(self,0);
 }
 if(!lock_on)((void(*)(void*,void*))original_update)(self,method);
}
static bool history_hook(void*self,void*method){
 return self && history_on ? 1 : ((bool(*)(void*,void*))original_history)(self,method);
}

#endif
