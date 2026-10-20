#define CLONE6_TESTING
#include "../src/HookLogic.h"
#include "../src/SelectedRuntime.h"
typedef union {u64 align;u8 data[0x400];} Object;
static Object root,linker;static int updates,forced,recalcs;static void*last_info;
static void update(void*self,int delta,void*info){(void)self;if(delta==17)updates++;last_info=info;}
static void force(void*self,bool enabled,bool sync,void*info){(void)info;if(self==&linker && enabled && sync)forced++;else forced=-100;}
static void calc(void*self,void*info){(void)info;if(self==&linker)recalcs++;}
static void*test_game_resolve(usize rva){switch(rva){case 0x4a383f4:return(void*)force;case 0x4a388bc:return(void*)calc;}return 0;}
#include "../src/Clone6Bridge.h"
static bool installed,wrong_build,stream_on;
#include "../src/Clone6BridgeImpl.h"
int main(void){
 C6Settings settings;C6GetSettings(&settings);if(settings.ready)return 9;
 C6SetOption(C6_MAP,1);if(map_on)return 10;installed=1;
 (void)map_hook;(void)zoom_hook;(void)update_hook;(void)history_hook;(void)can_skin_hook;(void)actor_skin_hook;(void)game_frame;(void)late_update_hook;
 original_root_update=(void*)update;
 *(void**)(root.data+0x370)=&linker;*(void**)(linker.data+0x20)=&root;
 *(void**)(root.data+0x300)=&root;*(u64*)(root.data+0x2f8)=123;
 root_update_hook(&root,17,(void*)5);if(updates!=1 || forced || recalcs || last_info!=(void*)5)return 1;
 C6SetOption(C6_MAP,1);root_update_hook(&root,17,0);if(forced!=1 || updates!=2)return 2;
 root_update_hook(&root,17,0);if(forced!=1)return 3;
 C6SetOption(C6_MAP,0);root_update_hook(&root,17,0);if(recalcs!=1)return 4;
 root_update_hook(&root,17,0);if(recalcs!=1)return 5;
 C6SetOption(C6_MAP,1);root_update_hook(&root,17,0);if(forced!=2)return 6;
 *(u64*)(root.data+0x2f8)=124;root_update_hook(&root,17,0);if(forced!=3)return 7;
 *(int*)(root.data+0x68)=1;map_revision++;root_update_hook(&root,17,0);if(forced!=3)return 8;
 C6SetCameraDistance(9);C6GetSettings(&settings);if(!settings.camera || settings.camera_distance!=5)return 11;
 C6SetCameraDistance(-9);C6GetSettings(&settings);if(settings.camera_distance!=-2)return 12;
 return 0;
}
