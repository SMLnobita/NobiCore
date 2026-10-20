#include "../src/HookLogic.h"
static int calls;static bool captured;static void*captured_info;
static bool original_map_test(void*s,u32 camp,bool vis,bool sync,void*info){(void)s;(void)camp;(void)sync;calls++;captured=vis;captured_info=info;return vis;}
static float original_zoom_test(void*s,void*m){(void)s;(void)m;return 1.25f;}
static void original_update_test(void*s,void*m){(void)s;(void)m;calls++;}
static bool original_history_test(void*s,void*m){(void)s;(void)m;return 0;}
int main(void){
 void*self=(void*)0x10;void*info=(void*)0x20;
 original_map=(void*)original_map_test;original_zoom=(void*)original_zoom_test;original_update=(void*)original_update_test;original_history=(void*)original_history_test;
 map_on=0;if(map_hook(self,1,0,1,info) || captured_info!=info || calls!=1)return 1;
 map_on=1;u32 allowed[]={1,2,255,110};for(unsigned i=0;i<4;i++)if(!map_hook(self,allowed[i],0,1,info))return 2;
 if(map_hook(self,3,0,1,info) || map_hook(0,1,0,1,info))return 3;
 camera_on=0;if(zoom_hook(self,info)!=1.25f)return 4;camera_on=1;float z=zoom_hook(self,info);if(z<1.849f || z>1.851f || zoom_hook(0,info)!=1.25f)return 5;
 calls=0;lock_on=0;update_hook(self,info);lock_on=1;update_hook(self,info);if(calls!=1)return 6;
 history_on=0;if(history_hook(self,info))return 7;history_on=1;if(!history_hook(self,info) || history_hook(0,info))return 8;
 return 0;
}
