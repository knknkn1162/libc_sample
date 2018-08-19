#include <signal.h>
#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 100

char *getpass_r(const char *prompt, char *buf, size_t len) {
  struct termios term;
  FILE *fp, *outfp;
  int echo = 0;
  int ch, i;
  char *p;

  //  generate terminal pathname
  char *ttyName = ctermid(NULL);
  if((outfp = fp = fopen(ttyName, "w+")) == NULL) {
    outfp = stderr;
    fp = stdin;
  }

  printf("in: %d, out: %d\n", fileno(fp), fileno(outfp));

  if(tcgetattr(fileno(fp), &term) == -1) {
    perror("tcgetattr");
    exit(1);
  }
  if((term.c_lflag & ECHO) != 0) {
    echo = 1;
    term.c_lflag &= ~ECHO;
    if(tcsetattr(fileno(fp), TCSAFLUSH|TCSASOFT, &term) == -1) {
      perror("tcsetattr");
      exit(1);
    }
  }

  if(prompt != NULL) {
    fputs(prompt, outfp);
  }

  //fgets(buf, BUF_SIZE, fp); // replace '\n' with '\0' anyway!
  for(p = buf, i = 0; (ch = getc(fp)) != EOF && ch != '\n'; i++) {
    if(i < BUF_SIZE) {
      *p++ = ch;
    }
  }
  *p = '\0';

  write(fileno(outfp), "\n", 1);

  // restore termios
  if(echo) {
    term.c_lflag |= ECHO;
    tcsetattr(fileno(fp), TCSAFLUSH|TCSASOFT, &term);
  }

  if(fp != stdin) {
    fclose(fp);
  }

  return buf;
}

int main(void) {
  char buf[BUF_SIZE];
  memset(buf, 0, BUF_SIZE);
  if(getpass_r("Password: ", buf, BUF_SIZE) == NULL) {
    perror("getpass");
    exit(1);
  }

  printf("%s\n", buf);
  return 0;
}
