#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int main (int argc, char *argv[]) {
  int sockfd, nbytes;
  FILE *fp;
  char buf[BUFSIZ];
  char mesg[BUFSIZ] = "Hello, world!\n";
  char *portname = "10000";
  char *servername = "localhost";
  int error;
  struct addrinfo hints, *res, *r;

  if (argc >= 2) {
    servername = argv[1];	/* 第1引数: サーバー名 */
    if (argc >= 3) {
      portname = argv[2];	/* 第2引数: ポート名 */
      if (argc >= 4) {
	snprintf (mesg, BUFSIZ, "%s\n", argv[3]); /* 第3引数: 送信文字列 */
      }
    }
  }

  memset (&hints, 0, sizeof (hints));
  hints.ai_family = PF_UNSPEC;	   /* IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM; /* TCP */
  if ((error = getaddrinfo (servername, portname, &hints, &res)))
    return (-1);

  for (r = res; r; r = r->ai_next) {
    if ((sockfd = socket (r->ai_family, r->ai_socktype, r->ai_protocol)) < 0)
      continue;
    if (connect (sockfd, r->ai_addr, r->ai_addrlen) == 0)
      break;
    close (sockfd);
    sockfd = -1;
  }
  freeaddrinfo(res);

  if (sockfd < 0) {
    exit (1);
  }

  /* これ以降は標準入出力ライブラリを使用できる */
  if ((fp = fdopen (sockfd, "r+")) == NULL) { /* read/write */
    perror ("fdopen"); exit (1);
  }
  setlinebuf (fp);		/* 行バッファリングを指定 */

  fputs (mesg, fp);
  printf ("echo: send [%s]\n", mesg);

  fgets (buf, sizeof (buf), fp);
  printf ("echo: receive [%s]\n", buf);
  fclose (fp);
  return 0;
}
