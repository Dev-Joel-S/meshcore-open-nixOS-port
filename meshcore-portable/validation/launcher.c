#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <locale.h>
static char root[PATH_MAX];
static char *path(const char *suffix) { char *p; if(asprintf(&p,"%s/%s",root,suffix)<0) exit(1); return p; }
static void setting(const char *key,const char *suffix) { setenv(key,path(suffix),1); }
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
static int checked_hash(const char *file,const char *expected) {
 int fds[2]; if(pipe(fds)) return -1; pid_t pid=fork();
 if(pid==0) { close(fds[0]); dup2(fds[1],1); close(fds[1]); execl(path("bin/busybox"),"busybox","sha256sum",file,(char*)NULL); _exit(127); }
 close(fds[1]); if(pid<0) { close(fds[0]); return -1; }
 FILE *f=fdopen(fds[0],"r"); char line[PATH_MAX+100]={0}; if(f) { if(!fgets(line,sizeof line,f)) line[0]=0; fclose(f); } else close(fds[0]);
 int status; if(waitpid(pid,&status,0)<0 || !WIFEXITED(status) || WEXITSTATUS(status)) return -1;
 return strncmp(line,expected,64)?-1:0;
}
static int prepare_runtime(void) {
 const char *files[]={"lib/libLLVM.so.19.1","data/flutter_assets/kernel_blob.bin"};
 const char *hashes[]={"6837a797a59c8cda5033dac674bb43b45e317045a4c1b916ebeb24232e3526fd","9508a399ea2bc54e7e8a996b9efe12f3eb5ffafb778e915c5ba9c9983b2fa9d3"};
 off_t sizes[]={129673080,116926600}; struct stat info; int missing=0;
 for(int i=0;i<2;i++) { if(stat(path(files[i]),&info)) missing=1; else if(info.st_size!=sizes[i]) { fprintf(stderr,"Unexpected runtime file: %s\n",files[i]); return -1; } }
 if(!missing) return 0;
 int lock=open(path(".runtime-unpack.lock"),O_CREAT|O_RDWR,0600); if(lock<0 || flock(lock,LOCK_EX)) { perror("Package directory must be writable on first start"); if(lock>=0) close(lock); return -1; }
 for(int i=0;i<2;i++) {
  char *dest=path(files[i]), *compressed, *temp; if(!stat(dest,&info)) continue;
  asprintf(&compressed,"%s.xz",dest); asprintf(&temp,"%s.unpack-%ld",dest,(long)getpid());
  int fd=open(temp,O_WRONLY|O_CREAT|O_EXCL,0644); if(fd<0) { perror(temp); close(lock); return -1; }
  pid_t pid=fork();
  if(pid==0) { dup2(fd,1); close(fd); execl(path("bin/busybox"),"busybox","xz","-d","-c","--",compressed,(char*)NULL); _exit(127); }
  close(fd); int status=0;
  if(pid<0 || waitpid(pid,&status,0)<0 || !WIFEXITED(status) || WEXITSTATUS(status) || checked_hash(temp,hashes[i]) || rename(temp,dest)) {
   fprintf(stderr,"Runtime extraction or SHA-256 verification failed: %s\n",files[i]); unlink(temp); close(lock); return -1;
  }
 }
 close(lock); return 0;
}

#include <sys/stat.h>
static int restore_archive_links(void) {
 static const struct { const char *name; const char *target; } links[]={
  {"data/icons/Adwaita/cursors/arrow","default"},
  {"data/icons/Adwaita/cursors/bd_double_arrow","nwse-resize"},
  {"data/icons/Adwaita/cursors/bottom_left_corner","sw-resize"},
  {"data/icons/Adwaita/cursors/bottom_right_corner","se-resize"},
  {"data/icons/Adwaita/cursors/bottom_side","s-resize"},
  {"data/icons/Adwaita/cursors/cross","crosshair"},
  {"data/icons/Adwaita/cursors/cross_reverse","crosshair"},
  {"data/icons/Adwaita/cursors/diamond_cross","crosshair"},
  {"data/icons/Adwaita/cursors/dnd-move","default"},
  {"data/icons/Adwaita/cursors/fd_double_arrow","nesw-resize"},
  {"data/icons/Adwaita/cursors/fleur","all-resize"},
  {"data/icons/Adwaita/cursors/hand1","grab"},
  {"data/icons/Adwaita/cursors/hand2","pointer"},
  {"data/icons/Adwaita/cursors/left_ptr","default"},
  {"data/icons/Adwaita/cursors/left_side","w-resize"},
  {"data/icons/Adwaita/cursors/move","default"},
  {"data/icons/Adwaita/cursors/question_arrow","help"},
  {"data/icons/Adwaita/cursors/right_side","e-resize"},
  {"data/icons/Adwaita/cursors/sb_h_double_arrow","ew-resize"},
  {"data/icons/Adwaita/cursors/sb_v_double_arrow","ns-resize"},
  {"data/icons/Adwaita/cursors/tcross","crosshair"},
  {"data/icons/Adwaita/cursors/top_left_arrow","default"},
  {"data/icons/Adwaita/cursors/top_left_corner","nw-resize"},
  {"data/icons/Adwaita/cursors/top_right_corner","ne-resize"},
  {"data/icons/Adwaita/cursors/top_side","n-resize"},
  {"data/icons/Adwaita/cursors/watch","wait"},
  {"data/icons/Adwaita/cursors/xterm","text"},
  {"lib/dri/kms_swrast_dri.so","libdril_dri.so"},
  {"lib/dri/swrast_dri.so","libdril_dri.so"},
  {"lib/libggml-base.so.0","libggml-base.so"},
  {"lib/libggml-cpu.so.0","libggml-cpu.so"},
  {"lib/libggml-vulkan.so.0","libggml-vulkan.so"},
  {"lib/libggml.so.0","libggml.so"},
  {"lib/libllama-common.so.0","libllama-common.so"},
  {"lib/libllama.so.0","libllama.so"},
  {"lib/libllamadart.so.0","libllamadart.so"},
  {"lib/libmtmd.so.0","libmtmd.so"},
  {"lib/libonnxruntime.so","libonnxruntime.so.1"},
  {"lib/libonnxruntime.so.1","libonnxruntime.so.1.22.0"},
 };
 for(size_t i=0;i<sizeof(links)/sizeof(links[0]);i++) {
  char *name=path(links[i].name); struct stat st;
  if(lstat(name,&st)) { perror(name); return -1; }
  if(S_ISLNK(st.st_mode)) continue;
  size_t len=strlen(links[i].target); if(!S_ISREG(st.st_mode) || st.st_size!=(off_t)len) { fprintf(stderr,"Unexpected archive link entry: %s\n",name); return -1; }
  FILE *f=fopen(name,"rb"); char text[PATH_MAX]={0}; size_t n=f?fread(text,1,sizeof(text)-1,f):0; if(f) fclose(f);
  if(n!=len || memcmp(text,links[i].target,len)) { fprintf(stderr,"Invalid archive link entry: %s\n",name); return -1; }
  char *temporary; asprintf(&temporary,"%s.restore-%ld",name,(long)getpid());
  if(symlink(links[i].target,temporary) || rename(temporary,name)) { perror(name); unlink(temporary); return -1; }
 }
 return 0;
}

int main(int argc,char **argv) {
 char exe[PATH_MAX]; ssize_t n=readlink("/proc/self/exe",exe,sizeof(exe)-1);
 if(n<0 || n>=sizeof(exe)-1) { perror("readlink /proc/self/exe"); return 1; } exe[n]=0;
 char *base=strrchr(exe,'/'); if(!base) return 1;
 int helper=strcmp(base+1,"meshcore-open")!=0; char name[128]; snprintf(name,sizeof name,"%s",base+1);
 *base=0; snprintf(root,sizeof root,"%s",exe); if(helper) { char *p=strrchr(root,'/'); if(!p) return 1; *p=0; }
 const char *remove[]={"LD_LIBRARY_PATH","LD_PRELOAD","LD_AUDIT","GLIBC_TUNABLES","GTK_PATH","GTK_MODULES","GIO_EXTRA_MODULES","GDK_PIXBUF_MODULE_FILE","LIBGL_DRIVERS_PATH",NULL};
 for(int i=0;remove[i];i++) unsetenv(remove[i]);
 char *binary=path("meshcore_open");
 if(helper) { char suffix[180]; snprintf(suffix,sizeof suffix,"libexec/%s",name); binary=path(suffix); }
 else {
  char *p; asprintf(&p,"%s/bin:%s",root,getenv("PATH")?getenv("PATH"):"/run/current-system/sw/bin:/bin"); setenv("PATH",p,1);
  setting("XKB_CONFIG_ROOT","data/xkb"); setting("XKB_CONFIG_EXTRA_PATH","data/xkb");
  unsetenv("LOCALE_ARCHIVE"); setting("LOCPATH","data/locale");
  if(!setlocale(LC_ALL,"")) setenv("LC_ALL","C.UTF-8",1);
  setting("XCURSOR_PATH","data/icons"); setenv("XCURSOR_THEME","Adwaita",1);
  setting("GIO_MODULE_DIR","lib/gio/modules"); setting("GSETTINGS_SCHEMA_DIR","data/glib-2.0/schemas");
  setting("FONTCONFIG_FILE","data/fonts.conf"); setting("FONTCONFIG_PATH","data");
  setting("SSL_CERT_FILE","data/ca-certificates.crt"); setting("G_TLS_CA_FILE","data/ca-certificates.crt");
  setting("LIBGL_DRIVERS_PATH","lib/dri"); setting("__EGL_VENDOR_LIBRARY_FILENAMES","data/egl/50_mesa.json");
  setting("VK_DRIVER_FILES","data/vulkan/lvp_icd.json"); setting("VK_ICD_FILENAMES","data/vulkan/lvp_icd.json");
  setenv("LIBGL_ALWAYS_SOFTWARE","1",1); setenv("GALLIUM_DRIVER","llvmpipe",1);
  setenv("GTK_IM_MODULE","gtk-im-context-simple",1); setenv("GTK_THEME","Adwaita",1);
  // A relative loader cache stays valid after relocation. Preserve user arguments;
  // the GUI has no documented positional-file command line interface.
  setting("GDK_PIXBUF_MODULE_FILE","data/loaders.cache"); setting("GDK_PIXBUF_MODULEDIR","lib/gdk-pixbuf/loaders");
  char *dirs; asprintf(&dirs,"%s/data:%s",root,getenv("XDG_DATA_DIRS")?getenv("XDG_DATA_DIRS"):"/run/current-system/sw/share:/usr/local/share:/usr/share"); setenv("XDG_DATA_DIRS",dirs,1);
 }
 if(!helper && restore_archive_links()!=0) return 1;
 if(!helper && prepare_runtime()!=0) return 1;
 if(chdir(root)!=0) { perror(root); return 1; }
 if(argc==2 && !helper && strcmp(argv[1],"--portable-info")==0) { printf("MeshCore Open PRE-BETA-9.5.2; Debian 13 coherent runtime; software OpenGL\nRoot: %s\n",root); return 0; }
 char **args=calloc(argc+9,sizeof(char*)); int i=0;
 args[i++]=path(".ld-linux-x86-64.so.2"); args[i++]="--inhibit-cache"; args[i++]="--library-path"; args[i++]=path("lib");
 args[i++]="--argv0"; args[i++]=binary; args[i++]=binary;
 for(int j=1;j<argc;j++) args[i++]=argv[j]; args[i]=NULL;
 execv(args[0],args); fprintf(stderr,"Cannot start %s: %s\n",binary,strerror(errno)); return 1;
}
