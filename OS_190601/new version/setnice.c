#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char **argv)
{
  if(argc != 3){
    printf(2, "usage: setnice pid nice_value\n");
    exit();
  }
  //setnice(atoi(argv[1]), atoi(argv[2]));  //no print

  //yes print below
  int result, nice1, nice2;
  nice1 = getnice(atoi(argv[1]));
  result = setnice(atoi(argv[1]), atoi(argv[2]));
  nice2 = getnice(atoi(argv[1]));
  if (!result) {
    printf(1, "nice value is changed from %d to %d\n", nice1, nice2);
  }
  else {
    printf(1, "setnice error occured\n");
  }

  exit();
}
