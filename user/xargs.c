#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

#define MAXLINE 512

static void
run(char *argv[])
{
  int pid = fork();
  if(pid < 0){
    fprintf(2, "xargs: fork failed\n");
    exit(1);
  }
  if(pid == 0){
    exec(argv[0], argv);
    fprintf(2, "xargs: exec %s failed\n", argv[0]); 
    exit(1);
  }
  wait(0);
}

int
main(int argc, char *argv[])
{
  char *args[MAXARG];   
  char line[MAXLINE];
  int nbase = 0, i = 1;

  if(i + 1 < argc && strcmp(argv[i], "-n") == 0)
    i += 2;

  if(i >= argc){
    fprintf(2, "usage: xargs command [args...]\n");
    exit(1);
  }

  // Chep doi so goc
  for(; i < argc; i++){
    if(nbase >= MAXARG - 1){
      fprintf(2, "xargs: too many arguments\n");
      exit(1);
    }
    args[nbase++] = argv[i];
  }

  // Doc stdin tung ky tu
  int len = 0, n;
  char c;
  for(;;){
    n = read(0, &c, 1);
    if(n < 0){
      fprintf(2, "xargs: read error\n");
      exit(1);
    }

    if(n == 0 || c == '\n'){        
      if(len > 0){
        line[len] = 0;
        int k = nbase;
        char *p = line;
        while(*p){                   
          while(*p == ' ' || *p == '\t')
            *p++ = 0;
          if(*p == 0)
            break;
          if(k >= MAXARG - 1){
            fprintf(2, "xargs: too many arguments\n");
            exit(1);
          }
          args[k++] = p;
          while(*p && *p != ' ' && *p != '\t')
            p++;
        }
        args[k] = 0;                  
        if(k > nbase)
          run(args);                
      }
      len = 0;
      if(n == 0)
        break;
    } else {
      if(len >= MAXLINE - 1){
        fprintf(2, "xargs: line too long\n");
        exit(1);
      }
      line[len++] = c;
    }
  }
  exit(0);
}