#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/wait.h>

void waitall (int sig) {
  int stat;
  pid_t pid;
  while ((pid = waitpid (-1, &stat, WNOHANG)) >= 0) {
    // -1: 任意のプロセス
    // WNOHANG: 終了したプロセスがないときに待たない
    fprintf (stderr, "waitpid: %d %d\n", pid, stat);
  }
}

int main (void) {
  int listenfd, connfd,randnum;
  char buf[BUFSIZ], data[10]="100\r\n";
  pid_t pid;
  struct sockaddr_in servaddr;
  if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
    perror ("socket"); exit (1);
  }
  memset (&servaddr, 0, sizeof (servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons (10000); /* echo port 7 is reserved */
  servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
  if (bind (listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr))
      < 0) {
    perror ("bind"); exit (1);
  }
  if (listen (listenfd, 5) < 0) {
    perror ("listen"); exit (1);
  }
  // 子プロセスが終了したら直ぐに全てwaitするための準備
  signal (SIGCHLD, waitall);

  for (;;) {
    if ((connfd = accept (listenfd, (struct sockaddr *)NULL, NULL)) < 0) {
      perror ("accept"); exit (1);
    }
    fprintf (stderr, "accept\n");
    pid = fork ();
    fprintf (stderr, "fork: %d\n", pid);
    if (pid == 0) {	/* child */
      FILE *fp;
      fputs ("child\n", stderr);
      close (listenfd);

      // これ以降は標準入出力ライブラリを使用できる
      if ((fp = fdopen (connfd, "r+")) == NULL) {
	      perror ("fdopen"); exit (1);
      }
      setlinebuf (fp);		/* 行バッファリングを指定 */
      int cnt = 0;

      while (fgets (buf, sizeof (buf), fp) != NULL) {
        cnt++;
        //このbufの中に文章が入るため、その中身を判定
	     // fputs ("child: ", stderr);
	    //  fputs (buf, stderr);
	      //fputs (buf, fp);	/* クライアントへ送信 */
        //printf("getbuf: %s\n",buf);
        //printf("get100: %s\n",data);
         //if文を作成して判定式を作る
       // puts(buf);
       //  printf("buf:%d\n",buf);
       //  printf("dame:%d\n", "100");
       //printf("strcmpbuf:%d\n",strcmp(buf,data));
       
        if(strcmp(buf,data)==0){
          randnum = rand()%6+1;
          printf("randnum = %d\n",randnum);
          switch(randnum){
            case 1:
             fputs("大吉\n",fp);
            break;
            default:
            fputs("外れ！\n",fp);
          }
        }
        else{
          fputs("適切な文章を入力してください\n",fp);
        }
      }
      fclose (fp);
      fputs ("child: exit\n", stderr);
      exit (0);
    } else {		/* parent */
      fputs ("parent\n", stderr);
      close (connfd);
    }
  }
}
