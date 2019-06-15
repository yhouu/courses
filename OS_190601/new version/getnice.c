#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char **argv)
{
  if(argc < 2){
    printf(2, "usage: getnice pid...\n");
    exit();
  }
  for(int i=1; i<argc; i++)
    //getnice(atoi(argv[i]));  //no print
    printf(1, "%d\n", getnice(atoi(argv[i])));  //print
  exit();
}
