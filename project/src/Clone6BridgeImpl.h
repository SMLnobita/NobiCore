#ifndef CLONE6_BRIDGE_IMPL_H
#define CLONE6_BRIDGE_IMPL_H
void C6GetSettings(C6Settings*out){
 if(!out)return;*out=(C6Settings){.ready=wrong_build?-1:installed?1:0,.map=map_on,.camera=camera_on,.camera_lock=lock_on,.stream=stream_on,.skin=skin_on,.bp=auto_bp_on,.smite=auto_smite_on,.boss_only=boss_only,.hero_name=name_on,.camera_distance=camera_add,.bp_percent=bp_percent};
}
void C6SetOption(C6Option option,int enabled){
 bool value=enabled!=0;if(!installed && option!=C6_STREAM && option!=C6_BOSS_ONLY)return;
 switch(option){
 case C6_MAP:if(map_on!=value){map_on=value;map_revision++;}break;
 case C6_CAMERA:if(camera_on!=value){camera_on=value;camera_revision++;}break;
 case C6_CAMERA_LOCK:if(lock_on!=value){lock_on=value;camera_revision++;}break;
 case C6_STREAM:stream_on=0;break; /* Reserved ABI slot; feature removed. */
 case C6_SKIN:skin_on=value;break;
 case C6_BP:auto_bp_on=value;break;
 case C6_SMITE:auto_smite_on=value;break;
 case C6_BOSS_ONLY:boss_only=value;break;
 case C6_HERO_NAME:name_on=value;break;
 }
}
void C6SetCameraDistance(float value){if(!installed || !(value==value))return;if(value < -2)value=-2;if(value>5)value=5;camera_add=value;camera_on=1;camera_revision++;}
void C6SetBPPercent(float value){if(!(value==value))return;if(value<1)value=1;if(value>100)value=100;bp_percent=value;}
#endif
