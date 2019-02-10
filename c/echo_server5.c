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
  int listenfd, connfd,daikiti_cnt=0;
  char buf[BUFSIZ], line_zero[1] = "\0",line_n[1] = "\n",line_r[1] = "\r";
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
      //子プロセスが呼び出された時の処理．変数の宣言は先頭で行っておく
      FILE *fp;
      int i = 0,cnt = 0,money = 0,cash = 0, money_flg = 0,pick_flg = 0,randnum;
      
      fputs ("child\n", stderr);
      close (listenfd);

      // これ以降は標準入出力ライブラリを使用できる
      if ((fp = fdopen (connfd, "r+")) == NULL) {
	      perror ("fdopen"); exit (1);
      }
       
      fputs("ようこそ．\nこのサーバーはおみくじを100円で引くことができるサーバーです．\n事前に入金をしないとおみくじを引くことができません．\n[charge]で入金モード，[pick]でおみくじモードへ移行できます．\n終了した場合，入金したお金はリセットされます．\n",fp);
     

      setlinebuf (fp);		/* 行バッファリングを指定 */
    

      while (fgets (buf, sizeof (buf), fp) != NULL) {
        cnt++;
        //このbufの中に文章が入るため、その中身を判定
	     fputs ("child: ", stderr);
	     fputs (buf, stderr);
       fprintf(stderr,"%d回目",cnt);

       //改行コードの排斥
       //全てのOSに対応した改行コード排斥を行い，文字のみにする
       if(strcmp(&buf[strlen(buf)-1],"\n") == 0 || strcmp(&buf[strlen(buf)-1],"\r")){
         buf[strlen(buf)-1] = line_zero[0];
         if((strcmp(&buf[strlen(buf)-1],"\r") == 0)){
           buf[strlen(buf)-1] = line_zero[0];
         }
         fprintf(stderr,"文字コードの排斥が実行された\n");
       }

       fprintf(stderr,"%s\n",buf);
     
      
       if(strcmp(buf,"charge")==0){
         fputs("入金モードに移行...\n",fp);
         money_flg = 1;
         pick_flg = 0;
       }
       else if(strcmp(buf,"pick")==0){
         fputs("おみくじモードに移行...\n",fp);
         money_flg = 0;
         pick_flg = 1;
       }     
       else{
          cash = strtol(buf,NULL,10);
          if(money_flg == 1){
            //入金モードの挙動

            if(cash > 1000){
              fputs("一度に入金しすぎです！やり直してください\n",fp);
            }
            else if(cash <= 0){
              fputs("0円以下は入金できません!やり直してください\n",fp);
            }
            else{
                money += cash;
                fprintf(fp,"%d円入金されました．",cash);
                //fputs(money,fp);

            }
          
          }
          else if(pick_flg == 1){
            //おみくじモードの挙動
            if(cash == 100){
              if(money >=100){
                randnum = rand()%6+1;
                fprintf(stderr,"randnum = %d\n",randnum);
                switch(randnum){
                  case 1:
                    fputs("大吉!\n",fp);
                    daikiti_cnt++;
                  
                    break;
                  case 2:
                    fputs("中吉！\n",fp);
                    break;
                  case 3:
                    fputs("小吉!\n",fp);
                    break;
                  case 4:
                    fputs("吉！\n",fp);
                    break;
                  case 5:
                    fputs("凶!\n",fp);
                    break;
                  case 6:
                    fputs("大凶！\n",fp);
                    break;
                }
                money -= 100;
              }
              else fputs("お金が足りません！[charge]で入金モードに移行してください\n",fp);
              
            }
            else fputs("このおみくじサーバーは100円専用です！[100]と入力してください\n",fp);
          }
          else fputs("モードを選択してください\n",fp);
        }
        fprintf(fp,"現在の価格 %d円\n",money);
        if(money_flg == 1)     fputs("現在のモード 入金モード\t[pick]でおみくじモードへ移行できます\n入金額の一度の限界額は1000円です\n",fp);
        else if(pick_flg == 1) fputs("現在のモード おみくじモード\t[charge]で入金モードへ移行できます\n",fp);
        else                   fputs("現在のモード なし\t[pick]でおみくじモード,[charge]で入金モードへ移行できます\n",fp);
       
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
