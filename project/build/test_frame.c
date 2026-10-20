#define CLONE6_TESTING
#include "../src/HookLogic.h"
#include "../src/SelectedRuntime.h"
typedef union {u64 aligned;u8 bytes[0x600];} Object;
static Object manager,list,array,host,enemy,boss,hv,ev,bv;
typedef struct {u64 klass;int length;unsigned short text[256];} ManagedString;
static ManagedString string_pool[128],account_names[2],account_prefix[2],account_custom[2];
static unsigned string_next;
static Object hud_views[2],hud_widgets[2][3];
static double clock_value=100;
static int talent=80104,requests,name_sets,name_resets,refreshes,updates,camera_refreshes;
static bool cd_ready=1,can_ready=1;
static int smite_damage=1500;
static int prepare_calls;
static int boss_type=1,slot4_talent,slot5_present=1;
static void*last_request_slot;
static void ptr(void*o,usize at,void*v){*(void**)((u8*)o+at)=v;}
static void integer(void*o,usize at,int v){*(int*)((u8*)o+at)=v;}
static double clock_now(void){return clock_value;}
static void*get_manager(void*m){(void)m;return &manager;}
static bool is_host(void*a,void*m){(void)m;return a==&host;}
static int object_type(void*a,void*m){(void)m;return a==&boss?boss_type:0;}
static int object_camp(void*a,void*m){(void)m;return *(int*)((u8*)a+0x258);}
static u32 object_config(void*a,void*m){(void)m;return *(u32*)((u8*)a+0x254);}
static int get_hp(void*a,void*m){(void)m;return *(int*)((u8*)a+0x40);}
static int get_maxhp(void*a,void*m){(void)m;return *(int*)((u8*)a+0x44);}
static GamePosition position(void*a,void*m){(void)m;return *(GamePosition*)((u8*)a+0x260);}
static void*get_slot(void*a,int index,void*m){(void)a;(void)m;
 if(index==5 && slot5_present)return &talent;
 if(index==4 && slot4_talent)return &slot4_talent;
 return 0;}
static int skill_id(void*s,void*m){(void)m;return s==&slot4_talent?slot4_talent:talent;}
static bool skill_ready(void*s,void*m){(void)s;(void)m;return cd_ready;}
static bool prepare(void*s,bool force,void*m){(void)s;(void)force;(void)m;prepare_calls++;return can_ready;}
static bool request(void*s,void*m){(void)m;requests++;last_request_slot=s;return 1;}
static int get_level(void*s,void*m){(void)s;(void)m;return 1;}
static int target_damage(void*s,u64 seq,void*actor,void*m){
 (void)m;if(actor!=&boss || seq!=1)return 0;
 return s==&slot4_talent?1500:smite_damage;
}
static void*make_string(const unsigned short*text,int start,int length,void*m){
 (void)start;(void)m;ManagedString*out=&string_pool[string_next++%128];out->length=length;
 for(int i=0;i<length;i++)out->text[i]=text[i];return out;
}
static void set_name(void*h,void*n,void*p,bool guide,void*custom,void*m){(void)guide;(void)m;
 int idx=h==&host?0:1;
 if(!n || !p || !custom){name_sets=-100;return;}
 if(same_game_text(n,account_names[idx].text,account_names[idx].length)){
  if(!same_game_text(p,account_prefix[idx].text,account_prefix[idx].length) || !same_game_text(custom,account_custom[idx].text,account_custom[idx].length)){name_resets=-100;return;}
  name_resets++;
 }else if(same_game_text(n,find_hero_name(105)->text,find_hero_name(105)->length) || same_game_text(n,find_hero_name(106)->text,find_hero_name(106)->length))name_sets++;
 else name_sets=-100;
 ptr(&hud_widgets[idx][0],0x110,n);ptr(&hud_widgets[idx][1],0x110,p);ptr(&hud_widgets[idx][2],0x110,custom);
}
static void refresh(void*a,void*m){(void)a;(void)m;refreshes++;}
static void fake_update(void*a,void*m){(void)a;(void)m;updates++;}
static void camera_change(void*a,void*m){(void)a;(void)m;camera_refreshes++;}
static bool fake_map(void*a,u32 c,bool v,bool sync,void*m){(void)a;(void)c;(void)sync;(void)m;return v;}
static float fake_zoom(void*a,void*m){(void)a;(void)m;return 1;}
static bool fake_history(void*a,void*m){(void)a;(void)m;return 0;}
static bool fake_skin(void*a,u32 h,u32 s,void*m){(void)a;(void)h;(void)s;(void)m;return 0;}
static bool fake_actor_skin(void*a,void*b,int*s,void*m){(void)a;(void)b;(void)m;*s=0;return 1;}
static void*test_game_resolve(usize rva){
 switch(rva){
 case 0x6e5368c:return(void*)get_manager;case 0x6ff3f04:return(void*)is_host;
 case 0x6fe9b6c:return(void*)object_type;case 0x6fe9b0c:return(void*)object_camp;case 0x6fe9aac:return(void*)object_config;
 case 0x702543c:return(void*)get_hp;case 0x702544c:return(void*)get_maxhp;case 0x6fe9498:return(void*)position;
 case 0x66cd324:return(void*)get_slot;case 0x65d15c0:return(void*)skill_id;case 0x65de488:return(void*)skill_ready;
 case 0x65e0b6c:return(void*)prepare;case 0x65e0ddc:return(void*)request;case 0x702549c:return(void*)get_level;case 0x65e2b74:return(void*)target_damage;
 case 0x3d7c118:return(void*)make_string;case 0x666a5ec:return(void*)set_name;
 case 0x6ff0f3c:case 0x6febba8:return(void*)refresh;
 }
 return 0;
}
static void add_actor(int index,Object*a,Object*value,int hp,int maxhp,int config,int camp){
 u64 seq=(u64)index+1;*(u64*)(a->bytes+0xe0)=seq;ptr(a,0xe8,a);ptr(a,0x28,value);ptr(a,0x18,&talent);ptr(a,0x70,a);ptr(a,0x40,a);ptr(a,0x178,0); /* Regression: model name missing, real account name still on HUD. */
 if(a!=&boss){int idx=a==&host?0:1;ptr(a,0x4e0,&hud_views[idx]);
  account_names[idx].length=3;account_names[idx].text[0]='T';account_names[idx].text[1]=0x00ea;account_names[idx].text[2]=(unsigned short)('A'+idx);
  account_prefix[idx].length=1;account_prefix[idx].text[0]='V';account_custom[idx].length=0;
  ptr(&hud_views[idx],0x188,&hud_widgets[idx][0]);ptr(&hud_views[idx],0x180,&hud_widgets[idx][1]);ptr(&hud_views[idx],0x198,&hud_widgets[idx][2]);
  ptr(&hud_widgets[idx][0],0x110,&account_names[idx]);ptr(&hud_widgets[idx][1],0x110,&account_prefix[idx]);ptr(&hud_widgets[idx][2],0x110,&account_custom[idx]);
 }
 integer(value,0x40,hp);integer(value,0x44,maxhp);integer(a,0x254,config);integer(a,0x258,camp);
 *(GamePosition*)(a->bytes+0x260)=(GamePosition){a==&host?0:1,0,0};
 *(u64*)(array.bytes+0x18+index*16)=seq;ptr(&array,0x20+index*16,a);
}
static void frame_step(void){clock_value+=1;game_frame();}
int main(void){
 game_base=1;runtime_uptime=clock_now;ptr(&manager,0x18,&list);ptr(&list,8,&array);integer(&list,16,3);*(usize*)(array.bytes+16)=3;
 /* Target callbacks can precede the host; the scheduler must still find it. */
 add_actor(0,&boss,&bv,1501,5000,7009,0);add_actor(1,&enemy,&ev,50,100,105,2);add_actor(2,&host,&hv,100,100,106,1);
 original_map=(void*)fake_map;original_zoom=(void*)fake_zoom;original_update=(void*)fake_update;original_history=(void*)fake_history;
 original_can_skin=(void*)fake_skin;original_actor_skin=(void*)fake_actor_skin;original_late_update=(void*)fake_update;
 if(map_hook(&host,1,0,0,0) || zoom_hook(&host,0)!=1 || history_hook(&host,0))return 1;
 int out=0;u32 meta=124;if(can_skin_hook(&host,124,1,0) || !actor_skin_hook(&host,&meta,&out,0))return 2;late_update_hook(&host,0);
 auto_smite_on=1;frame_step();if(requests || auto_state!=4)return 3;
 integer(&bv,0x40,1500);frame_step();if(requests!=1 || auto_state!=6)return 4;
 cd_ready=0;frame_step();if(requests!=1 || auto_state!=3)return 5;cd_ready=1;
 integer(&boss,0x254,7010);frame_step();if(requests!=1)return 6;
 boss_only=0;frame_step();if(requests!=2)return 7;
 can_ready=0;frame_step();if(requests!=3 || auto_state!=6 || prepare_calls!=3)return 8;can_ready=1;
 auto_smite_on=0;auto_bp_on=1;talent=80108;integer(&ev,0x40,14);frame_step();if(requests!=3)return 9;
 integer(&ev,0x40,13);frame_step();if(requests!=4)return 10;
 integer(&enemy,0x258,1);frame_step();if(requests!=4)return 11;integer(&enemy,0x258,2);
 *(GamePosition*)(enemy.bytes+0x260)=(GamePosition){6,0,0};frame_step();if(requests!=4)return 12;
 auto_bp_on=0;name_on=1;frame_step();if(name_sets!=2)return 13;
 name_on=0;ptr(&host,0x4e0,0);frame_step();if(name_resets!=1 || !name_was_applied)return 14;
 ptr(&host,0x4e0,&hud_views[0]);frame_step();if(name_resets!=2 || name_was_applied)return 25;
 int before=refreshes;map_revision++;frame_step();if(refreshes!=before+6)return 15;
 runtime_frame=game_frame;refresh_camera=camera_change;lock_on=1;camera_revision++;before=updates;update_hook(&host,0);
 if(camera_refreshes!=1 || updates!=before)return 16;
 update_hook(&host,0);if(camera_refreshes!=1)return 17;camera_revision++;update_hook(&host,0);if(camera_refreshes!=2)return 18;
 lock_on=0;update_hook(&host,0);if(updates!=before+1)return 19;
 /* A dedicated monster list must work when GameActors contains only heroes;
  * duplicate actors in two lists must not cause duplicate HUD updates. */
 Object monsters={0},monster_array={0};ptr(&manager,0x48,&monsters);ptr(&monsters,8,&monster_array);integer(&monsters,16,1);*(usize*)(monster_array.bytes+16)=1;
 *(u64*)(monster_array.bytes+0x18)=1;ptr(&monster_array,0x20,&boss);
 integer(&list,16,2);*(u64*)(array.bytes+0x18)=2;ptr(&array,0x20,&enemy);*(u64*)(array.bytes+0x28)=3;ptr(&array,0x30,&host);
 ptr(&manager,0x40,&monsters);auto_smite_on=1;auto_bp_on=0;talent=80104;integer(&boss,0x254,7009);boss_only=1;integer(&bv,0x40,1000);
 before=requests;frame_step();if(requests!=before+1)return 20;
 /* Same level, unchanged HP: farming raises damage; next frame must react. */
 integer(&bv,0x40,1501);frame_step();if(requests!=before+1 || auto_damage!=1500)return 21;
 smite_damage=1600;frame_step();if(requests!=before+2 || auto_damage!=1600 || auto_level!=1)return 22;
 /* A zero live value falls back to the donor level formula (1350 at level 1). */
 smite_damage=0;integer(&bv,0x40,1501);frame_step();if(requests!=before+2 || auto_state!=4)return 23;
 integer(&bv,0x40,1300);frame_step();if(requests!=before+3 || auto_damage!=1350)return 24;
 smite_damage=1500;
 /* A known config is eligible even when objType differs from 1. */
 boss_type=0;integer(&bv,0x40,1000);int mark=requests;frame_step();
 if(requests!=mark+1)return 30;
 boss_type=1;
 /* Verified boss configs must dispatch with the boss-only filter enabled. */
 const u32 verified_bosses[]={7009,7012,7024,70092,70093,7050,70504,7051,70514,702400,702411};
 boss_only=1;
 for(unsigned i=0;i<sizeof(verified_bosses)/sizeof(verified_bosses[0]);i++){
  integer(&boss,0x254,(int)verified_bosses[i]);integer(&bv,0x40,1000);mark=requests;frame_step();
  if(requests!=mark+1 || last_request_slot!=&talent)return 33;
 }
 /* Super buffs are excluded even when boss-only is disabled. */
 const u32 removed_configs[]={547010,547011};
 for(int filter=0;filter<=1;filter++)for(unsigned i=0;i<2;i++){
  boss_only=filter;integer(&boss,0x254,(int)removed_configs[i]);mark=requests;frame_step();
  if(requests!=mark)return 32;
 }
 boss_only=1;
 integer(&boss,0x254,7009);
 /* Donor reads slot 5 only: a smite in slot 4 must not be used. */
 slot5_present=0;slot4_talent=80116;integer(&bv,0x40,1000);mark=requests;frame_step();
 if(requests!=mark || auto_state!=8)return 31;
 slot5_present=1;slot4_talent=0;
 return 0;
}
