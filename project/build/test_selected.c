#include "../src/HookLogic.h"
#include "../src/SelectedRuntime.h"
static int calls;
static void*last_info;
static bool test_map(void*s,u32 c,bool v,bool sync,void*m){(void)s;(void)c;(void)sync;last_info=m;return v;}
static float test_zoom(void*s,void*m){(void)s;(void)m;return 1;}
static void test_void(void*s,void*m){(void)s;last_info=m;calls++;}
static bool test_history(void*s,void*m){(void)s;(void)m;return 0;}
static bool test_can_skin(void*s,u32 h,u32 k,void*m){(void)s;(void)h;(void)k;last_info=m;return 0;}
static bool test_actor_skin(void*s,void*a,int*out,void*m){(void)s;(void)a;*out=2;last_info=m;return 1;}
int main(void){
 void*self=(void*)16,*info=(void*)32;int skin=0;u32 meta=123;
 original_map=(void*)test_map;original_zoom=(void*)test_zoom;original_update=(void*)test_void;original_history=(void*)test_history;
 if(map_hook(self,1,0,0,info) || zoom_hook(self,info)!=1 || history_hook(self,info))return 1;
 update_hook(self,info);
 if(bp_eligible(0,100,20,1) || bp_eligible(-1,100,20,1) || bp_eligible(1,0,20,1))return 2;
 if(!bp_eligible(20,100,20,25) || bp_eligible(21,100,20,25) || bp_eligible(10,100,20,27))return 3;
 if(bp_eligible(1,100,0,1) || bp_eligible(1,100,101,1) || bp_eligible(1,100,20,-1))return 4;
 const u32 bosses[]={7009,7012,7024,70092,70093,7050,70504,7051,70514,702400,702411};
 for(unsigned i=0;i<sizeof(bosses)/sizeof(bosses[0]);i++)if(!is_boss_config(bosses[i]) || !is_known_monster_config(bosses[i]))return 5;
 if(is_boss_config(7010) || is_boss_config(7011) || !is_known_monster_config(7010) || !is_known_monster_config(7011) || is_known_monster_config(1) || is_known_monster_config(547010) || is_known_monster_config(547011))return 5;
 original_can_skin=(void*)test_can_skin;original_actor_skin=(void*)test_actor_skin;
 if(can_skin_hook(self,123,5,info) || last_info!=info)return 6;
 skin_on=1;if(!can_skin_hook(self,123,5,info))return 7;
 if(!actor_skin_hook(self,&meta,&skin,info) || skin!=5 || last_info!=info)return 8;
 meta=124;if(!actor_skin_hook(self,&meta,&skin,info) || skin!=2)return 9;
 skin_on=0;meta=123;actor_skin_hook(self,&meta,&skin,info);if(skin!=2)return 10;
 runtime_frame=game_frame;original_late_update=(void*)test_void;int before=calls;late_update_hook(self,info);
 if(calls!=before+1 || last_info!=info)return 11; /* Features OFF never enter game helpers. */
 if(!find_hero_name(105) || find_hero_name(999999))return 13;
 return 0;
}
