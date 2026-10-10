#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <poll.h>
void callback(unsigned int a,unsigned int b) {}
int main(void) {
 void *h=dlopen("libflserial.so",RTLD_NOW|RTLD_LOCAL); if(!h) { puts(dlerror()); return 1; }
 int (*init)(int)=dlsym(h,"fl_init"),(*openport)(int,char*,int)=dlsym(h,"fl_open"),(*ctrl)(int,int,int)=dlsym(h,"fl_ctrl"),(*readport)(int,int,char*)=dlsym(h,"fl_read"),(*writeport)(int,int,char*)=dlsym(h,"fl_write"),(*closeport)(int)=dlsym(h,"fl_close");
 setbuf(stdout,NULL); puts("loaded plugin"); int (*setcb)(int,void(*)(unsigned int,unsigned int))=dlsym(h,"fl_set_callback");
 int master=posix_openpt(O_RDWR|O_NOCTTY|O_NONBLOCK); if(master<0 || grantpt(master) || unlockpt(master)) { perror("pty"); return 1; }
 char *slave=ptsname(master); printf("init=%d\n",init(16)); int port=openport(-1,slave,115200); printf("opened handle=%d\n",port); setcb(port,callback);
 if(ctrl(port,1,0)!=1) { fprintf(stderr,"PTY open unsupported by upstream serial plugin, error=%d\n",ctrl(port,0,0)); closeport(port); return 2; }
 char message[]="meshcore-portable-serial-test",buffer[256]={0}; write(master,message,sizeof message); int n=0;
 for(int i=0;i<100 && n<sizeof message;i++) { int got=readport(port,sizeof buffer-n,buffer+n); if(got>0) n+=got; usleep(10000); }
 if(n!=sizeof message || memcmp(buffer,message,n)) { fprintf(stderr,"serial RX mismatch n=%d\n",n); return 1; }
 if(writeport(port,sizeof message,message)!=0) return 1;
 struct pollfd p={master,POLLIN,0}; if(poll(&p,1,2000)!=1) return 1; n=read(master,buffer,sizeof buffer);
 if(n!=sizeof message || memcmp(buffer,message,n)) return 1;
 closeport(port); close(master); puts("PASS flserial pseudoterminal bidirectional native I/O"); return 0;
}
