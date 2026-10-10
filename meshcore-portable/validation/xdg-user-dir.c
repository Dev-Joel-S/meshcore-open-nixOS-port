#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
int main(int argc,char **argv) {
 if(argc!=2) return 1; for(char *s=argv[1];*s;s++) if(!isupper((unsigned char)*s) && *s!='_') return 1;
 const char *home=getenv("HOME"); if(!home || home[0]!='/') return 1;
 char *config,*key; const char *xdg=getenv("XDG_CONFIG_HOME");
 if(xdg && *xdg) asprintf(&config,"%s/user-dirs.dirs",xdg); else asprintf(&config,"%s/.config/user-dirs.dirs",home);
 asprintf(&key,"XDG_%s_DIR=",argv[1]); FILE *f=fopen(config,"r"); char *line=NULL; size_t size=0;
 if(f) while(getline(&line,&size,f)>=0) {
  char *v=line; while(isspace((unsigned char)*v)) v++; if(strncmp(v,key,strlen(key))) continue; v+=strlen(key); while(isspace((unsigned char)*v)) v++;
  int quote=*v=='"'; if(quote) v++; char *out=calloc(strlen(v)+strlen(home)+2,1),*p=out;
  while(*v && (quote ? *v!='"' : *v!='\n')) {
   if(*v=='\\' && v[1]) { v++; *p++=*v++; }
   else if(!strncmp(v,"$HOME",5)) { strcpy(p,home); p+=strlen(home); v+=5; }
   else if(!strncmp(v,"${HOME}",7)) { strcpy(p,home); p+=strlen(home); v+=7; }
   else *p++=*v++;
  }
  if(out[0]=='/') { puts(out); free(out); free(line); fclose(f); return 0; } free(out);
 }
 if(f) fclose(f); if(!strcmp(argv[1],"DESKTOP")) printf("%s/Desktop\n",home); else puts(home); free(line); return 0;
}
