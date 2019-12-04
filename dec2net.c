#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {
  if (argc > 2) {
    printf("Error\n");
    exit(0);
  }
  unsigned long dst;
  char *p = (char *)&dst;
  int conv = inet_pton(AF_INET, argv[1], (void *)&dst);
  if (conv <= 0) {
    printf("error\n");
    exit(0);
  }

  for (int i = 0; i < 4; i++) {
    printf("%02x ", (*p)&0xff);
    p++;
  }
  printf("\n");

  exit(0);
}
