/* Independent reconstruction. No licensing/key/UDID client is included.
 * Exact supported Unity UUID: e73ce98da67a3fd6ad83a16d13984811.
 * Hook table format is verified against the supplied 6bohuel snapshot.
 * This file deliberately uses a small declared ABI instead of an iOS SDK.
 */
#include "HookLogic.h"
#include "SelectedRuntime.h"
#ifdef CLONE6_IMGUI
#include "Clone6Bridge.h"
#endif
typedef void *id; typedef void *SEL; typedef void *Class;
typedef struct { double x,y; } Point;
typedef struct { double width,height; } Size;
typedef struct { Point origin; Size size; } Rect;
typedef struct { double top,left,bottom,right; } Insets;
extern id objc_msgSend(id,SEL,...);
extern id objc_msgSendSuper(void*,SEL,...);
extern Class objc_getClass(const char*);
extern SEL sel_registerName(const char*);
extern Class objc_allocateClassPair(Class,const char*,usize);
extern bool class_addMethod(Class,SEL,void*,const char*);
extern void objc_registerClassPair(Class);
extern const char *object_getClassName(id);
extern void dispatch_after_f(u64,void*,void*,void(*)(void*));
extern u64 dispatch_time(u64,long long);
extern char _dispatch_main_q[];
extern u32 _dyld_image_count(void);
extern const char *_dyld_get_image_name(u32);
extern const void *_dyld_get_image_header(u32);
extern isize _dyld_get_image_vmaddr_slide(u32);
extern void NSLog(id,...);
#define S(x) sel_registerName(x)
#define M0(o,s) ((id(*)(id,SEL))objc_msgSend)((o),S(s))
#define M1(o,s,a) ((id(*)(id,SEL,id))objc_msgSend)((o),S(s),(id)(a))
#define V1(o,s,a) ((void(*)(id,SEL,id))objc_msgSend)((o),S(s),(id)(a))
#define VI(o,s,a) ((void(*)(id,SEL,isize))objc_msgSend)((o),S(s),(a))
#define VB(o,s,a) ((void(*)(id,SEL,signed char))objc_msgSend)((o),S(s),(a))
static id str(const char *s){return ((id(*)(id,SEL,const char*))objc_msgSend)(objc_getClass("NSString"),S("stringWithUTF8String:"),s);}
static int eq(const char*a,const char*b){while(*a && *a==*b){++a;++b;}return *a==*b;}
static int contains(const char*a,const char*b){for(;*a;++a){const char*x=a,*y=b;while(*x && *y && *x==*y){++x;++y;}if(!*y)return 1;}return 0;}
static int same16(const u8*a,const u8*b){for(int i=0;i<16;i++)if(a[i]!=b[i])return 0;return 1;}
static u32 read32(const void*p){return *(const u32*)p;}
static u64 read64(const void*p){return *(const u64*)p;}
static id panel, menu_button, controller, status_label, zoom_label, bp_label, camera_button;
static id overlay_field, overlay_canvas;
static bool stream_on;
static bool installed, wrong_build;
static unsigned attempts;
static void logmsg(const char*s){NSLog(str("[Clone6] %@"),str(s));}
struct Header {u32 magic,cpu,sub,filetype,ncmds,cmdbytes,flags,reserved;};
static int bind_hooks(void){
 const u8 expected[]={0xe7,0x3c,0xe9,0x8d,0xa6,0x7a,0x3f,0xd6,0xad,0x83,0xa1,0x6d,0x13,0x98,0x48,0x11};
 for(u32 i=0;i<_dyld_image_count();i++){
  const char*name=_dyld_get_image_name(i);
  if(!name || !contains(name,"/UnityFramework.framework/UnityFramework"))continue;
  const struct Header*h=_dyld_get_image_header(i);if(!h || h->magic!=0xfeedfacf || h->ncmds>256 || h->cmdbytes>65536)return -1;
  const u8*cmd=(const u8*)(h+1);const u8*end=cmd+h->cmdbytes;u64 tableva=0,tablesz=0;int uuidok=0;
  for(u32 j=0;j<h->ncmds && cmd+8<=end;j++){
   u32 type=read32(cmd),sz=read32(cmd+4);if(sz<8 || cmd+sz>end)return -1;
   if(type==0x1b && sz>=24)uuidok=same16(cmd+8,expected);
   if(type==0x19 && sz>=72 && eq((const char*)cmd+8,"__HOOK_DATA")){tableva=read64(cmd+24);tablesz=read64(cmd+32);}
   cmd+=sz;
  }
  if(!uuidok || tableva!=0xda80000 || tablesz<0x58*11)return -1;
  isize slide=_dyld_get_image_vmaddr_slide(i);u8*table=(u8*)(tableva+slide);
  const u64 targets[]={0x4a380e8,0x541142c,0x540d828,0x5adf5a8,0x6156594,0x45ee6cc,0x6feffb8,0x4a15524};
  void*callbacks[]={(void*)map_hook,(void*)zoom_hook,(void*)update_hook,(void*)history_hook,(void*)can_skin_hook,(void*)actor_skin_hook,(void*)late_update_hook,(void*)root_update_hook};
  _Atomic(void*) *originals[]={&original_map,&original_zoom,&original_update,&original_history,&original_can_skin,&original_actor_skin,&original_late_update,&original_root_update};
  u8*records[8]={0,0,0,0,0,0,0,0};
  for(unsigned k=0;k<11;k++)for(unsigned t=0;t<8;t++)if(read64(table+k*0x58)==targets[t])records[t]=table+k*0x58;
  /* Validate the entire set before writing any callback slot. */
  for(unsigned t=0;t<8;t++){
   if(!records[t] || read64(records[t]+0x48) || read64(records[t]+0x28)<0xda7c000 || read64(records[t]+0x28)>=0xda80000)return -1;
  }
  game_base=(usize)h;runtime_frame=game_frame;refresh_camera=(void(*)(void*,void*))(game_base+0x540f2c8);
  for(unsigned t=0;t<8;t++)*originals[t]=(void*)(slide+read64(records[t]+0x28));
  for(unsigned t=0;t<8;t++)__atomic_store_n((void**)(records[t]+0x48),callbacks[t],__ATOMIC_RELEASE);
  return 1;
 }
 return 0;
}
static id color(double r,double g,double b,double a){return ((id(*)(id,SEL,double,double,double,double))objc_msgSend)(objc_getClass("UIColor"),S("colorWithRed:green:blue:alpha:"),r,g,b,a);}
static void frame(id v,double x,double y,double w,double h){Rect r={{x,y},{w,h}};((void(*)(id,SEL,Rect))objc_msgSend)(v,S("setFrame:"),r);}
static void title(id b,const char*s){((void(*)(id,SEL,id,u64))objc_msgSend)(b,S("setTitle:forState:"),str(s),0);}
static void rounded(id v,double radius){id layer=M0(v,"layer");((void(*)(id,SEL,double))objc_msgSend)(layer,S("setCornerRadius:"),radius);VB(v,"setClipsToBounds:",1);}
static void click(id b,const char*selector){((void(*)(id,SEL,id,SEL,u64))objc_msgSend)(b,S("addTarget:action:forControlEvents:"),controller,S(selector),1ULL<<6);}
static void label(id parent,const char*text,double y){id v=M0(M0(objc_getClass("UILabel"),"alloc"),"init");frame(v,12,y,276,28);V1(v,"setText:",str(text));V1(v,"setTextColor:",color(0.92,0.94,0.97,1));VI(v,"setNumberOfLines:",1);V1(parent,"addSubview:",v);}
static id button(id parent,const char*text,double y,const char*selector){id v=((id(*)(id,SEL,isize))objc_msgSend)(objc_getClass("UIButton"),S("buttonWithType:"),1);frame(v,12,y,276,34);title(v,text);V1(v,"setBackgroundColor:",color(0.16,0.19,0.25,1));((void(*)(id,SEL,id,u64))objc_msgSend)(v,S("setTitleColor:forState:"),color(0.92,0.96,1,1),0);rounded(v,7);click(v,selector);V1(parent,"addSubview:",v);return v;}
static void toggle_panel(id self,SEL cmd,id sender){(void)self;(void)cmd;(void)sender;signed char hidden=((signed char(*)(id,SEL))objc_msgSend)(panel,S("isHidden"));VB(panel,"setHidden:",!hidden);}
static void set_status(void){if(status_label)V1(status_label,"setText:",str(installed?"Sẵn sàng — cập nhật trong trận":wrong_build?"Phiên bản game chưa được hỗ trợ":"Đang chờ game khởi tạo..."));}
static void toggle_map(id s,SEL c,id b){(void)s;(void)c;if(!installed)return;map_on=!map_on;map_revision++;title(b,map_on?"Hack map: BẬT":"Hack map: TẮT");}
static void toggle_camera(id s,SEL c,id b){(void)s;(void)c;if(!installed)return;camera_on=!camera_on;camera_revision++;title(b,camera_on?"Camera: BẬT":"Camera: TẮT");}
static void toggle_lock(id s,SEL c,id b){(void)s;(void)c;if(!installed)return;lock_on=!lock_on;camera_revision++;title(b,lock_on?"Khóa camera: BẬT":"Khóa camera: TẮT");}
static void toggle_skin(id s,SEL c,id b){(void)s;(void)c;if(!installed)return;skin_on=!skin_on;title(b,skin_on?"Skin thường + Evo: BẬT":"Skin thường + Evo: TẮT");}
static void toggle_bp(id s,SEL c,id b){(void)s;(void)c;if(!installed)return;auto_bp_on=!auto_bp_on;title(b,auto_bp_on?"Tự Bộc phá: BẬT":"Tự Bộc phá: TẮT");}
static void toggle_smite(id s,SEL c,id b){(void)s;(void)c;if(!installed)return;auto_smite_on=!auto_smite_on;title(b,auto_smite_on?"Tự Trừng trị: BẬT":"Tự Trừng trị: TẮT");}
static void toggle_boss(id s,SEL c,id b){(void)s;(void)c;boss_only=!boss_only;title(b,boss_only?"Chỉ Rồng / Tà Thần: BẬT":"Chỉ Rồng / Tà Thần: TẮT");}
static void toggle_name(id s,SEL c,id b){(void)s;(void)c;if(!installed)return;name_on=!name_on;title(b,name_on?"Hiện tên tướng: BẬT":"Hiện tên tướng: TẮT");}
static double uptime_seconds(void){return ((double(*)(id,SEL))objc_msgSend)(M0(objc_getClass("NSProcessInfo"),"processInfo"),S("systemUptime"));}
static id overlay_hit_test(id self,SEL cmd,Point point,id event){
 (void)cmd;id views[]={menu_button,panel};
 for(unsigned i=0;i<2;i++){
  id view=views[i];if(!view || ((signed char(*)(id,SEL))objc_msgSend)(view,S("isHidden")))continue;
  Point local=((Point(*)(id,SEL,Point,id))objc_msgSend)(view,S("convertPoint:fromView:"),point,self);
  id hit=((id(*)(id,SEL,Point,id))objc_msgSend)(view,S("hitTest:withEvent:"),local,event);if(hit)return hit;
 }
 return 0;
}
static id find_secure_canvas(id field){
 id children=M0(field,"subviews");usize n=((usize(*)(id,SEL))objc_msgSend)(children,S("count"));
 for(usize i=0;i<n;i++){
  id child=((id(*)(id,SEL,usize))objc_msgSend)(children,S("objectAtIndex:"),i);const char*name=object_getClassName(child);
  if(name && contains(name,"Canvas"))return child;
 }
 return 0;
}
static void move_view(id view,Point delta){
 id parent=M0(view,"superview");if(!parent)return;
 Point center=((Point(*)(id,SEL))objc_msgSend)(view,S("center"));
 Rect bounds=((Rect(*)(id,SEL))objc_msgSend)(parent,S("bounds")),own=((Rect(*)(id,SEL))objc_msgSend)(view,S("bounds"));
 Insets safe=((Insets(*)(id,SEL))objc_msgSend)(M0(view,"window"),S("safeAreaInsets"));
 double minx=bounds.origin.x+safe.left+own.size.width*.5+4,miny=bounds.origin.y+safe.top+own.size.height*.5+4;
 double maxx=bounds.origin.x+bounds.size.width-safe.right-own.size.width*.5-4,maxy=bounds.origin.y+bounds.size.height-safe.bottom-own.size.height*.5-4;
 center.x+=delta.x;center.y+=delta.y;if(center.x<minx)center.x=minx;if(center.x>maxx)center.x=maxx;if(center.y<miny)center.y=miny;if(center.y>maxy)center.y=maxy;
 ((void(*)(id,SEL,Point))objc_msgSend)(view,S("setCenter:"),center);
}
static void overlay_layout(id self,SEL cmd){
 struct {id receiver;Class superclass;} super={self,objc_getClass("UITextField")};
 ((void(*)(void*,SEL))objc_msgSendSuper)(&super,cmd);
 Rect bounds=((Rect(*)(id,SEL))objc_msgSend)(self,S("bounds"));id canvas=find_secure_canvas(self);
 if(canvas){frame(canvas,0,0,bounds.size.width,bounds.size.height);VI(canvas,"setAutoresizingMask:",(1<<1)|(1<<4));}
 if(panel){
  Rect f=((Rect(*)(id,SEL))objc_msgSend)(panel,S("frame"));double height=bounds.size.height-16;if(height>590)height=590;
  double x=f.origin.x;if(x+300>bounds.size.width-8)x=bounds.size.width-308;if(x<8)x=8;frame(panel,x,8,300,height);
 }
 if(menu_button){Point zero={0,0};move_view(menu_button,zero);}
}
static void toggle_stream(id s,SEL c,id b){
 (void)s;(void)c;bool enabled=!stream_on;VB(overlay_field,"setSecureTextEntry:",enabled);M0(overlay_field,"layoutIfNeeded");
 id canvas=find_secure_canvas(overlay_field);
 if(enabled && !canvas){VB(overlay_field,"setSecureTextEntry:",0);title(b,"Stream: chưa hỗ trợ iOS này");return;}
 if(!canvas)canvas=overlay_field;
 if(canvas!=overlay_canvas){
  id views[]={panel,menu_button};for(unsigned i=0;i<2;i++){M0(views[i],"retain");M0(views[i],"removeFromSuperview");V1(canvas,"addSubview:",views[i]);M0(views[i],"release");}
  overlay_canvas=canvas;
 }
 stream_on=enabled;title(b,stream_on?"Chế độ Stream: BẬT":"Chế độ Stream: TẮT");
}
static void pan_view(id self,SEL cmd,id gesture){
 (void)self;(void)cmd;
 id view=M0(gesture,"view"),parent=M0(view,"superview");if(!view || !parent)return;
 Point delta=((Point(*)(id,SEL,id))objc_msgSend)(gesture,S("translationInView:"),parent);
 move_view(view,delta);
 Point zero={0,0};((void(*)(id,SEL,Point,id))objc_msgSend)(gesture,S("setTranslation:inView:"),zero,parent);
}
static void draggable(id view){
 id recognizer=((id(*)(id,SEL,id,SEL))objc_msgSend)(M0(objc_getClass("UIPanGestureRecognizer"),"alloc"),S("initWithTarget:action:"),controller,S("pan:"));
 VB(recognizer,"setCancelsTouchesInView:",1);V1(view,"addGestureRecognizer:",recognizer);M0(recognizer,"release");
}
static void zoom_changed(id self,SEL cmd,id slider){
 (void)self;(void)cmd;
 float value=((float(*)(id,SEL))objc_msgSend)(slider,S("value"));
 if(value < -2)value=-2;if(value>5)value=5;camera_add=value;camera_revision++;if(self){camera_on=1;title(camera_button,"Camera: BẬT");}
 id text=((id(*)(id,SEL,id,...))objc_msgSend)(objc_getClass("NSString"),S("stringWithFormat:"),str("Độ xa camera: %+.2f"),(double)value);
 V1(zoom_label,"setText:",text);
}
static void bp_changed(id self,SEL cmd,id slider){
 (void)self;(void)cmd;float value=((float(*)(id,SEL))objc_msgSend)(slider,S("value"));if(value<1)value=1;if(value>100)value=100;bp_percent=value;
 id text=((id(*)(id,SEL,id,...))objc_msgSend)(objc_getClass("NSString"),S("stringWithFormat:"),str("Máu địch ≤ %.1f%%"),(double)value);V1(bp_label,"setText:",text);
}
static id value_slider(double y,float minimum,float maximum,float value,const char*action){
 id slider=M0(M0(objc_getClass("UISlider"),"alloc"),"init");frame(slider,12,y,276,32);
 ((void(*)(id,SEL,float))objc_msgSend)(slider,S("setMinimumValue:"),minimum);((void(*)(id,SEL,float))objc_msgSend)(slider,S("setMaximumValue:"),maximum);((void(*)(id,SEL,float))objc_msgSend)(slider,S("setValue:"),value);
 ((void(*)(id,SEL,id,SEL,u64))objc_msgSend)(slider,S("addTarget:action:forControlEvents:"),controller,S(action),1ULL<<12);V1(panel,"addSubview:",slider);return slider;
}
static int make_menu(void){
 if(menu_button)return 1;
#ifdef CLONE6_IMGUI
 menu_button=C6ImGuiInstall();return menu_button?1:0;
#endif
 id app=M0(objc_getClass("UIApplication"),"sharedApplication");id window=M0(app,"keyWindow");if(!window){id windows=M0(app,"windows");window=M0(windows,"firstObject");}if(!window)return 0;
 Class cl=objc_getClass("Clone6MenuController");if(!cl){cl=objc_allocateClassPair(objc_getClass("NSObject"),"Clone6MenuController",0);
  class_addMethod(cl,S("toggle:"),(void*)toggle_panel,"v@:@");class_addMethod(cl,S("map:"),(void*)toggle_map,"v@:@");class_addMethod(cl,S("camera:"),(void*)toggle_camera,"v@:@");class_addMethod(cl,S("lock:"),(void*)toggle_lock,"v@:@");class_addMethod(cl,S("pan:"),(void*)pan_view,"v@:@");class_addMethod(cl,S("zoom:"),(void*)zoom_changed,"v@:@");
  class_addMethod(cl,S("skin:"),(void*)toggle_skin,"v@:@");class_addMethod(cl,S("bp:"),(void*)toggle_bp,"v@:@");class_addMethod(cl,S("smite:"),(void*)toggle_smite,"v@:@");class_addMethod(cl,S("boss:"),(void*)toggle_boss,"v@:@");class_addMethod(cl,S("name:"),(void*)toggle_name,"v@:@");class_addMethod(cl,S("stream:"),(void*)toggle_stream,"v@:@");class_addMethod(cl,S("bpValue:"),(void*)bp_changed,"v@:@");objc_registerClassPair(cl);}
 controller=M0(M0(cl,"alloc"),"init");
 Rect bounds=((Rect(*)(id,SEL))objc_msgSend)(window,S("bounds"));
 Class overlay_class=objc_getClass("Clone6OverlayField");if(!overlay_class){overlay_class=objc_allocateClassPair(objc_getClass("UITextField"),"Clone6OverlayField",0);class_addMethod(overlay_class,S("hitTest:withEvent:"),(void*)overlay_hit_test,"@@:{CGPoint=dd}@");class_addMethod(overlay_class,S("layoutSubviews"),(void*)overlay_layout,"v@:");objc_registerClassPair(overlay_class);}
 overlay_field=M0(M0(overlay_class,"alloc"),"init");((void(*)(id,SEL,Rect))objc_msgSend)(overlay_field,S("setFrame:"),bounds);VI(overlay_field,"setAutoresizingMask:",(1<<1)|(1<<4));VB(overlay_field,"setUserInteractionEnabled:",1);V1(window,"addSubview:",overlay_field);M0(overlay_field,"layoutIfNeeded");overlay_canvas=find_secure_canvas(overlay_field);if(!overlay_canvas)overlay_canvas=overlay_field;
 double width=300,height=bounds.size.height-16;if(height>590)height=590;double px=bounds.size.width>width+72?72:8;double py=8;
 panel=M0(M0(objc_getClass("UIScrollView"),"alloc"),"init");frame(panel,px,py,width,height);Size content={width,590};((void(*)(id,SEL,Size))objc_msgSend)(panel,S("setContentSize:"),content);VB(panel,"setAlwaysBounceVertical:",1);V1(panel,"setBackgroundColor:",color(.07,.09,.13,.96));rounded(panel,14);V1(overlay_canvas,"addSubview:",panel);
 label(panel,"Clone6 — Final",8);
 status_label=M0(M0(objc_getClass("UILabel"),"alloc"),"init");frame(status_label,12,36,276,24);V1(status_label,"setTextColor:",color(.65,.8,.95,1));((void(*)(id,SEL,id))objc_msgSend)(status_label,S("setFont:"),((id(*)(id,SEL,double))objc_msgSend)(objc_getClass("UIFont"),S("systemFontOfSize:"),11));V1(panel,"addSubview:",status_label);
 button(panel,"Hack map: TẮT",67,"map:");camera_button=button(panel,"Camera: TẮT",106,"camera:");
 zoom_label=M0(M0(objc_getClass("UILabel"),"alloc"),"init");frame(zoom_label,12,144,276,24);V1(zoom_label,"setTextColor:",color(.92,.96,1,1));V1(panel,"addSubview:",zoom_label);
 id slider=value_slider(170,-2,5,camera_add,"zoom:");zoom_changed(0,S("zoom:"),slider);
 button(panel,"Khóa camera: TẮT",211,"lock:");button(panel,"Chế độ Stream: TẮT",250,"stream:");button(panel,"Skin thường + Evo: TẮT",289,"skin:");button(panel,"Tự Bộc phá: TẮT",328,"bp:");
 bp_label=M0(M0(objc_getClass("UILabel"),"alloc"),"init");frame(bp_label,12,366,276,24);V1(bp_label,"setTextColor:",color(.92,.96,1,1));V1(panel,"addSubview:",bp_label);id bp_slider=value_slider(392,1,100,bp_percent,"bpValue:");bp_changed(controller,S("bpValue:"),bp_slider);
 button(panel,"Tự Trừng trị: TẮT",435,"smite:");button(panel,"Chỉ Rồng / Tà Thần: BẬT",474,"boss:");button(panel,"Hiện tên tướng: TẮT",513,"name:");
 label(panel,"Chạm C6: mở/ẩn; kéo C6: di chuyển",552);
 menu_button=((id(*)(id,SEL,isize))objc_msgSend)(objc_getClass("UIButton"),S("buttonWithType:"),1);frame(menu_button,12,30,48,42);M0(menu_button,"retain");title(menu_button,"C6");V1(menu_button,"setBackgroundColor:",color(.13,.36,.64,.95));((void(*)(id,SEL,id,u64))objc_msgSend)(menu_button,S("setTitleColor:forState:"),color(1,1,1,1),0);rounded(menu_button,12);click(menu_button,"toggle:");V1(overlay_canvas,"addSubview:",menu_button);draggable(menu_button);set_status();return 1;
}
static void tick(void*context){(void)context;
 id pool=M0(M0(objc_getClass("NSAutoreleasePool"),"alloc"),"init");
 if(!installed && !wrong_build){int status=bind_hooks();if(status>0){installed=1;logmsg("validated hook table; callbacks installed");}else if(status<0){wrong_build=1;logmsg("hook table/build mismatch; no callbacks written");}}
 make_menu();set_status();

 M0(pool,"drain");
 if(!menu_button || (!installed && !wrong_build)){if(++attempts<60)dispatch_after_f(dispatch_time(0,2000000000LL),_dispatch_main_q,0,tick);}
 else dispatch_after_f(dispatch_time(0,500000000LL),_dispatch_main_q,0,tick);
}
__attribute__((constructor)) static void start_clone(void){runtime_uptime=uptime_seconds;dispatch_after_f(dispatch_time(0,2000000000LL),_dispatch_main_q,0,tick);}

#ifdef CLONE6_IMGUI
#include "Clone6BridgeImpl.h"
#endif
