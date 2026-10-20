#ifndef CLONE6_MAP_RUNTIME_H
#define CLONE6_MAP_RUNTIME_H
/* Runs on LActorRoot.UpdateLogic's own thread, never from UIKit/camera thread. */
static _Atomic(void*) original_root_update;
typedef struct {void*root;void*linker;u64 sequence;u32 revision;bool forced;} MapActorState;
static MapActorState map_actors[32];
static unsigned map_actor_replace;
static __attribute__((unused)) void root_update_hook(void*self,int delta,void*info){
 ((void(*)(void*,int,void*))original_root_update)(self,delta,info);
 if(!self || *(int*)((u8*)self+0x68)!=0)return; /* ActorMeta.ActorType: heroes */
 void*linker=member_ptr(self,0x370);
 if(!linker || member_ptr(linker,0x20)!=self || member_ptr(self,0x300)!=self)return;
 u64 sequence=*(u64*)((u8*)self+0x2f8);MapActorState*state=0;
 for(unsigned i=0;i<32;i++)if(map_actors[i].root==self && map_actors[i].sequence==sequence && map_actors[i].linker==linker){state=&map_actors[i];break;}
 if(!state){state=&map_actors[map_actor_replace++%32];*state=(MapActorState){self,linker,sequence,0,0};}
 u32 revision=map_revision;bool enabled=map_on;
 if(state->revision==revision)return;
 if(enabled){
  /* ForceSetVisible iterates the game's valid camps and calls SetVisible with
   * forceSync=true: sends enter-view now even when visibility has not changed. */
  GAME_FN(0x4a383f4,void(*)(void*,bool,bool,void*))(linker,1,1,0);state->forced=1;
 }else if(state->forced){
  /* Recalculate from real HorizonMarker state and sync the camp visibility. */
  GAME_FN(0x4a388bc,void(*)(void*,void*))(linker,0);state->forced=0;
 }
 state->revision=revision;
}
#endif
