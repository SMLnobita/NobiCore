#ifndef CLONE6_SELECTED_RUNTIME_H
#define CLONE6_SELECTED_RUNTIME_H
/* Exact-build native entry points, checked against the matching dump and ARM64.
 * Unity calls these hooks on its attached update thread; no game calls from UI.
 */
typedef struct { float x,y,z; } GamePosition;
#include "HeroNames.h"
static _Atomic bool skin_on, auto_bp_on, auto_smite_on, boss_only=1, name_on;
static _Atomic float bp_percent=13.7f;
static _Atomic u64 selected_skin;
static _Atomic(void*) original_can_skin, original_actor_skin, original_late_update;
static usize game_base;
static double (*runtime_uptime)(void);
static void *runtime_host;
static double runtime_cast_time, runtime_frame_time;
static double runtime_name_time, runtime_status_time;
static int runtime_cast_state;
static bool name_was_applied, runtime_busy;
static u32 applied_map_revision;
static _Atomic int auto_state, detected_talent;
static _Atomic int auto_target_hp, auto_damage, auto_level;
static _Atomic int seen_monsters, matching_monsters, nearby_monsters;
static int frame_monsters,frame_matching,frame_nearby;
#ifdef CLONE6_TESTING
static void *test_game_resolve(usize rva);
#define GAME_FN(rva,type) ((type)test_game_resolve(rva))
#else
#define GAME_FN(rva,type) ((type)(game_base+(rva)))
#endif
static void *member_ptr(void *object,usize offset){return object?*(void**)((u8*)object+offset):0;}
/* Verified normalMonster + Vietnamese name tables; see analysis/kraydus.
 * 547010/547011 are super blue/red buffs and are excluded by user request. */
static bool is_boss_config(u32 id){return id==7009 || id==7012 || id==7024 || id==70092 || id==70093 || id==7050 || id==70504 || id==7051 || id==70514 || id==702400 || id==702411;}
static bool is_known_monster_config(u32 id){return is_boss_config(id) || id==7010 || id==7011;}
static bool bp_eligible(int hp,int maxhp,float percent,float distance2){
 return hp>0 && maxhp>0 && percent>0 && percent<=100 && distance2>=0 && distance2<=5.1f*5.1f && (float)hp*100.0f<=(float)maxhp*percent;
}
static bool can_skin_hook(void*self,u32 hero,u32 skin,void*info){
 if(self && skin_on){selected_skin=((u64)hero<<32)|skin;return 1;}
 return ((bool(*)(void*,u32,u32,void*))original_can_skin)(self,hero,skin,info);
}
static bool actor_skin_hook(void*self,void*meta,int*out,void*info){
 bool result=((bool(*)(void*,void*,int*,void*))original_actor_skin)(self,meta,out,info);
 u64 choice=selected_skin;
 if(self && meta && out && skin_on && (u32)(choice>>32)==*(u32*)meta){*out=(int)(u32)choice;return 1;}
 return result;
}
static int actor_hp(void*actor,bool maximum){
 void*value=member_ptr(actor,0x28);if(!value)return 0;
 return GAME_FN(maximum?0x702544c:0x702543c,int(*)(void*,void*))(value,0);
}
/* Own UTF16 copies: no managed pointer is retained outside the game frame. */
typedef struct {void*actor;u64 sequence;bool active;int n,p,c;unsigned short name[256],prefix[128],custom[128];} SavedHudName;
static SavedHudName saved_names[32];
static bool copy_game_string(void*string,unsigned short*out,int capacity,int*length){
 if(!string){*length=0;out[0]=0;return 1;}
 int n=*(int*)((u8*)string+8);if(n<0 || n>=capacity)return 0;
 const unsigned short*text=(const unsigned short*)((u8*)string+12);
 for(int i=0;i<n;i++)out[i]=text[i];out[n]=0;*length=n;return 1;
}
static void*name_widget_string(void*view,usize field){return member_ptr(member_ptr(view,field),0x110);}
static void*new_game_string(const unsigned short*text,int length){return GAME_FN(0x3d7c118,void*(*)(const unsigned short*,int,int,void*))(text,0,length,0);}
static bool same_game_text(void*string,const unsigned short*text,int length){
 if(!string)return length==0;
 if(*(int*)((u8*)string+8)!=length)return 0;
 const unsigned short*s=(const unsigned short*)((u8*)string+12);
 for(int i=0;i<length;i++)if(s[i]!=text[i])return 0;return 1;
}
static void update_display_name(void*actor,bool enabled){
 void*hud=member_ptr(actor,0x70),*view=member_ptr(hud,0x4e0);
 if(!view || !member_ptr(view,0x188))return;
 u64 sequence=*(u64*)((u8*)actor+0xe0);SavedHudName*saved=0,*free_slot=0;
 for(unsigned i=0;i<32;i++){
  SavedHudName*x=&saved_names[i];
  if(x->actor==actor && x->sequence==sequence){saved=x;break;}
  if(!x->active && !free_slot)free_slot=x;
 }
 if(enabled && (!saved || !saved->active)){
  if(!saved)saved=free_slot;if(!saved)return;
  saved->active=0;saved->actor=actor;saved->sequence=sequence;
  /* HudComponentView.ownerNameData.Name is +0x188; Text3D.m_text is +0x110.
   * ActorLinker.name can be empty even while this UI displays the account name. */
  if(!copy_game_string(name_widget_string(view,0x188),saved->name,256,&saved->n) || saved->n==0 ||
     !copy_game_string(name_widget_string(view,0x180),saved->prefix,128,&saved->p) ||
     !copy_game_string(name_widget_string(view,0x198),saved->custom,128,&saved->c))return;
  saved->active=1;
 }
 if(!saved || !saved->active)return;
 void*name,*prefix,*custom;static const unsigned short empty_text[]={0};
 if(enabled){
  u32 hero=GAME_FN(0x6fe9aac,u32(*)(void*,void*))(actor,0);
  const HeroName*entry=find_hero_name(hero);if(!entry)return;
  name=new_game_string(entry->text,entry->length);prefix=custom=new_game_string(empty_text,0);
 }else{
  name=new_game_string(saved->name,saved->n);prefix=new_game_string(saved->prefix,saved->p);custom=new_game_string(saved->custom,saved->c);
 }
 if(!name || !prefix || !custom)return;
 GAME_FN(0x666a5ec,void(*)(void*,void*,void*,bool,void*,void*))(hud,name,prefix,0,custom,0);
 if(!enabled && same_game_text(name_widget_string(view,0x188),saved->name,saved->n))saved->active=0;
}
static bool prune_saved_names(void**actors,unsigned count){
 bool pending=0;
 for(unsigned n=0;n<32;n++)if(saved_names[n].active){
  bool present=0;for(unsigned i=0;i<count;i++)if(saved_names[n].actor==actors[i] && saved_names[n].sequence==*(u64*)((u8*)actors[i]+0xe0)){present=1;break;}
  if(!present)saved_names[n].active=0;else pending=1;
 }
 return pending;
}
/* Same accessor used by CSkillButtonManager.RefreshPunishDamage at
 * 0x5fa9bc0. Query for the actual candidate on each evaluation; no level formula
 * and no extra modifier that could apply the game's adjustment twice. */
static int live_smite_damage(void*slot,void*actor){
 u64 sequence=*(u64*)((u8*)actor+0xe0);
 void*target=member_ptr(actor,0xe8);
 if(target!=actor)return 0;
 return GAME_FN(0x65e2b74,int(*)(void*,u64,void*,void*))(slot,sequence,target,0);
}
/* 6bohuel donor formula: 1350 + 100*(soul level - 1). */
static int smite_level_damage(int level){return level>0?1350+100*(level-1):0;}
/* Live accessor first; the donor formula covers a zero/unavailable result. */
static int smite_estimate(void*slot,void*actor,int base){
 int live=live_smite_damage(slot,actor);
 return live>0?live:base;
}
static bool smite_can_cast(int hp,int damage,int*state){
 if(damage<=0){*state=7;return 0;}
 if(hp>damage){*state=4;return 0;}
 return 1;
}
static void try_auto_on_target(void*actor,double now){
 if(!runtime_host || actor==runtime_host || now-runtime_cast_time<.3)return;
 if(actor_hp(runtime_host,0)<=0)return;
 int hp=actor_hp(actor,0);if(hp<=0)return;
 int type=GAME_FN(0x6fe9b6c,int(*)(void*,void*))(actor,0);
 u32 config=GAME_FN(0x6fe9aac,u32(*)(void*,void*))(actor,0);
 /* Config ID is the precise target identity; objType is only a fallback
  * for actors whose known config arrives with a different type. */
 bool bp=auto_bp_on && type==0, smite=auto_smite_on && (type==1 || is_known_monster_config(config));
 if(!bp && !smite)return;
 if(bp){
  int a=GAME_FN(0x6fe9b0c,int(*)(void*,void*))(actor,0),h=GAME_FN(0x6fe9b0c,int(*)(void*,void*))(runtime_host,0);
  if(a==h || a<1 || a>2)return;
 }
 if(smite){
  frame_monsters++;
  if(!is_known_monster_config(config))return;
  if(boss_only && !is_boss_config(config))return;
  frame_matching++;
 }
 GamePosition p=GAME_FN(0x6fe9498,GamePosition(*)(void*,void*))(actor,0),h=GAME_FN(0x6fe9498,GamePosition(*)(void*,void*))(runtime_host,0);
 float dx=p.x-h.x,dy=p.y-h.y,dz=p.z-h.z,d2=dx*dx+dy*dy+dz*dz;
 if(bp && !bp_eligible(hp,actor_hp(actor,1),bp_percent,d2))return;
 if(smite && !(d2>=0 && d2<=6.5f*6.5f))return;
 if(smite)frame_nearby++;
 void*skill=member_ptr(runtime_host,0x18);if(!skill)return;
 bool matched=0;
 int level=0,base_damage=0;
 if(smite){
  void*value=member_ptr(runtime_host,0x28);
  if(value){level=GAME_FN(0x702549c,int(*)(void*,void*))(value,0);base_damage=smite_level_damage(level);}
  auto_level=level;
 }
 if(smite){
  /* Donor reads summoner slot 5 only; its id changes 80104 -> 80116 when the
   * jungle item completes. No fallback to other summoner slots. */
  void*slot=GAME_FN(0x66cd324,void*(*)(void*,int,void*))(skill,5,0);
  if(slot){
   int id=GAME_FN(0x65d15c0,int(*)(void*,void*))(slot,0);
   if(id==80104 || id==80116){
    matched=1;detected_talent=id;
    if(!GAME_FN(0x65de488,bool(*)(void*,void*))(slot,0))auto_state=3;
    else{
     int damage=smite_estimate(slot,actor,base_damage),state=0;
     auto_target_hp=hp;auto_damage=damage;
     if(!smite_can_cast(hp,damage,&state)){if(state)auto_state=state;}
     else{
      runtime_cast_time=now;
      /* Donor always calls RequestUseSkill after ReadyUseSkill, even when
       * Ready returns false (lock-target mode can return false after updating). */
      (void)GAME_FN(0x65e0b6c,bool(*)(void*,bool,void*))(slot,0,0);
      auto_state=GAME_FN(0x65e0ddc,bool(*)(void*,void*))(slot,0)?6:5;
      runtime_cast_state=auto_state;
      runtime_status_time=now;
     }
    }
   }
  }
 }else{
  for(int index=4;index<=6;index++){
   void*slot=GAME_FN(0x66cd324,void*(*)(void*,int,void*))(skill,index,0);if(!slot)continue;
   int id=GAME_FN(0x65d15c0,int(*)(void*,void*))(slot,0);
   if(id!=80108)continue;
   matched=1;detected_talent=id;
   if(!GAME_FN(0x65de488,bool(*)(void*,void*))(slot,0)){auto_state=3;continue;}
   runtime_cast_time=now;
   (void)GAME_FN(0x65e0b6c,bool(*)(void*,bool,void*))(slot,0,0);
   auto_state=GAME_FN(0x65e0ddc,bool(*)(void*,void*))(slot,0)?6:5;
   runtime_cast_state=auto_state;
   runtime_status_time=now;
   break;
  }
 }
 if(!matched)auto_state=8;
}
static void game_frame(void){
 if(!game_base || !runtime_uptime || runtime_busy)return;
 bool names=name_on;u32 revision=map_revision;
 if(!auto_bp_on && !auto_smite_on && !names && !name_was_applied && revision==applied_map_revision)return;
 double now=runtime_uptime();if(now-runtime_frame_time<.025)return;
 runtime_frame_time=now;runtime_busy=1;runtime_host=0;
 bool keep_status=now-runtime_status_time<1.0;
 if(!keep_status){auto_state=1;detected_talent=0;}
 frame_monsters=frame_matching=frame_nearby=0;
 void*manager=GAME_FN(0x6e5368c,void*(*)(void*))(0);
 static void*actors[512];unsigned valid=0;
 /* This game's ARM64 IL2CPP uses array data at +0x18 and 16-byte
  * PoolObjHandle values. Verified from native ActorManager methods. */
 /* Read the dedicated lists too; do not assume GameActors contains every
  * neutral objective in every mode. ActorManager fields verified in dump. */
 const usize lists[]={0x18,0x20,0x40,0x48};
 for(unsigned l=0;l<sizeof(lists)/sizeof(lists[0]);l++){
 void*list=member_ptr(manager,lists[l]),*array=member_ptr(list,0x8);
 int count=list?*(int*)((u8*)list+0x10):0;
 usize capacity=array?*(usize*)((u8*)array+0x10):0;
 if(!array || count<1 || count>4096 || capacity<(usize)count || capacity>4096)continue;
 for(int i=0;i<count && valid<512;i++){
  u8*entry=(u8*)array+0x18+(usize)i*16;u64 sequence=*(u64*)entry;void*actor=*(void**)(entry+8);
  if(!actor || member_ptr(actor,0xe8)!=actor || *(u32*)((u8*)actor+0xe0)!=(u32)sequence)continue;
  bool duplicate=0;for(unsigned a=0;a<valid;a++)if(actors[a]==actor){duplicate=1;break;}
  if(duplicate)continue;
  actors[valid++]=actor;
  if(GAME_FN(0x6ff3f04,bool(*)(void*,void*))(actor,0))runtime_host=actor;
 }
 }
 if(runtime_host && !keep_status)auto_state=2;
 bool refresh_names=now-runtime_name_time>=.25;
 for(unsigned i=0;i<valid;i++){
  void*actor=actors[i];
  if(revision!=applied_map_revision){
   GAME_FN(0x6ff0f3c,void(*)(void*,void*))(actor,0);
   GAME_FN(0x6febba8,void(*)(void*,void*))(actor,0);
  }
  if(((names && refresh_names) || (!names && name_was_applied)) && GAME_FN(0x6fe9b6c,int(*)(void*,void*))(actor,0)==0)update_display_name(actor,names);
  if(runtime_host && (auto_bp_on || auto_smite_on))try_auto_on_target(actor,now);
 }
 seen_monsters=frame_monsters;matching_monsters=frame_matching;nearby_monsters=frame_nearby;
 if(auto_smite_on && auto_state==2){
  if(!frame_monsters)auto_state=9;
  else if(!frame_matching)auto_state=10;
  else if(!frame_nearby)auto_state=11;
 }
 if(keep_status)auto_state=runtime_cast_state;
 if(names && refresh_names)runtime_name_time=now;
 name_was_applied=prune_saved_names(actors,valid);applied_map_revision=revision;runtime_host=0;runtime_busy=0;
}
static void late_update_hook(void*self,void*info){
 ((void(*)(void*,void*))original_late_update)(self,info);
 /* One frame scheduler, independent of the actor callback ordering. */
}
#include "MapRuntime.h"
#endif
