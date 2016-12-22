#include <stdio.h>
#include <stdlib.h>

int main()
{
  char str[100];

  fprintf(stdout, "in test2\n");

  fscanf(stdin,"%s", str);

  printf("%s\n", str);
  fprintf(stdout, "scaned str: %s \n", str);

  return 0;
}
