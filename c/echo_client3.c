#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main (int argc, char *argv[]) {
  int sockfd, nbytes;
  FILE *fp;
  char buf[BUFSIZ];
  char mesg[BUFSIZ] = "Hello, world!\n";
  char *portname = "10000";
  char *servername = "localhost";
  struct servent *serv;
  struct hostent *servhost;
  struct sockaddr_in servaddr;

  if (argc >= 2) {
    servername = argv[1];	/* 第1引数: サーバー名 */
    if (argc >= 3) {
      portname = argv[2];	/* 第2引数: ポート名 */
      if (argc >= 4) {
	snprintf (mesg, BUFSIZ, "%s\n", argv[3]); /* 第3引数: 送信文字列 */
      }
    }
  }
  serv = getservbyname (portname, "tcp");
  if (serv == NULL) {
    int port;
    port = atoi (portname);
    serv = getservbyport (port, "tcp");
    if (serv == NULL) {
      perror ("getservbyport"); exit (1);
    }
  }
  servhost = gethostbyname (servername);
  if (servhost == NULL) {
    in_addr_t addr;
    addr = inet_addr (servername);
    servhost = gethostbyaddr ((char *)&addr, sizeof(addr), AF_INET);
    if (servhost == NULL) {
      perror ("gethostbyaddr"); exit (1);
    }
  }
  if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
    perror ("socket"); exit (1);
  }
  memset (&servaddr, 0, sizeof (servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons (serv->s_port);
  memcpy ((char *)&servaddr.sin_addr, servhost->h_addr, servhost->h_length);
  if (connect (sockfd, (struct sockaddr *)&servaddr, sizeof (servaddr))
      < 0) {
    perror ("connect"); exit (1);
  }
  /* これ以降は標準入出力ライブラリを使用できる */
  if ((fp = fdopen (sockfd, "r+")) == NULL) { /* read/write */
    perror ("fdopen"); exit (1);
  }
  setlinebuf (fp);		/* 行バッファリングを指定 */

  fputs (mesg, fp);
  fgets (buf, sizeof (buf), fp);
  printf ("echo: %s", buf);
  fclose (fp);
  return 0;
}
