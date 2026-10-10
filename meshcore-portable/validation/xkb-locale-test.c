#define _GNU_SOURCE
#include <dlfcn.h>
#include <locale.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
struct names { const char *rules,*model,*layout,*variant,*options; };
int main(void) {
 const char *locales[]={"C.UTF-8","de_DE.UTF-8","en_US.UTF-8"};
 for(int i=0;i<3;i++) { if(!setlocale(LC_ALL,locales[i])) { fprintf(stderr,"Failed locale %s\n",locales[i]); return 1; } printf("PASS locale %s\n",locales[i]); }
 void *h=dlopen("libxkbcommon.so.0",RTLD_NOW); if(!h) { puts(dlerror()); return 1; }
 void *(*context_new)(int)=dlsym(h,"xkb_context_new"); void *(*keymap_new)(void*,struct names*,int)=dlsym(h,"xkb_keymap_new_from_names");
 int (*keysym)(void*,uint32_t,uint32_t,uint32_t,const uint32_t**)=dlsym(h,"xkb_keymap_key_get_syms_by_level");
 void *ctx=context_new(0); if(!ctx) return 1;
 struct names german={"evdev","pc105","de",NULL,NULL}; void *de=keymap_new(ctx,&german,0); if(!de) return 1;
 const uint32_t *syms; if(keysym(de,29,0,0,&syms)!=1 || syms[0]!=0x7a) { fprintf(stderr,"German Y-key mapping was not z\n"); return 1; }
 german.layout="us"; void *us=keymap_new(ctx,&german,0); if(!us || keysym(us,29,0,0,&syms)!=1 || syms[0]!=0x79) return 1;
 puts("PASS XKB evdev/pc105 German and US keymap compilation and key symbols"); return 0;
}
