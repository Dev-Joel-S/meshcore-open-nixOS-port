#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdatomic.h>
static atomic_int called;
void callback(unsigned int event,unsigned int size) { atomic_fetch_add(&called,1); }
int main(int argc, char **argv) {
 setbuf(stdout,NULL);
 void *h=dlopen("libflserial.so",RTLD_NOW|RTLD_LOCAL); if(!h){puts(dlerror());return 1;}
 int (*init)(int)=dlsym(h,"fl_init"),(*openport)(int,char*,int)=dlsym(h,"fl_open"),(*setcb)(int,void(*)(unsigned int,unsigned int))=dlsym(h,"fl_set_callback"),(*readport)(int,int,char*)=dlsym(h,"fl_read"),(*closeport)(int)=dlsym(h,"fl_close");
 int master=posix_openpt(O_RDWR|O_NOCTTY|O_NONBLOCK); if(master<0||grantpt(master)||unlockpt(master))return 2;
 init(16); int port=openport(-1,ptsname(master),115200); if(port<0)return 3;
 const char msg[]="\r\n[BOOT] MeshCore device ready\r\n";
 write(master,msg,sizeof(msg)); puts("Boot data sent before callback registration");
 usleep(150000);
 if(argc>1 && !strcmp(argv[1],"--close-before-registration")) { closeport(port);close(master);puts("PASS close with early boot data and no callback");return 0; }
 setcb(port,callback);
 char buffer[256]={0};int n=0;
 for(int i=0;i<200&&n<sizeof(msg);i++){int got=readport(port,sizeof(buffer)-n,buffer+n);if(got>0)n+=got;usleep(1000);}
 int count=atomic_load(&called);closeport(port);close(master);
 if(n!=sizeof(msg)||memcmp(buffer,msg,n)||count<1){fprintf(stderr,"FAIL bytes=%d callback=%d\n",n,count);return 4;}
 puts("PASS early boot data preserved and callback delivered");return 0;
}
