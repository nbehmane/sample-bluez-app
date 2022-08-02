#include "app.h"

//! MAIN START
int main(int argc, char **argv)
{
   app_init();
   #if CLI
   app_main(argc, argv);
   #else
   app_run();
   #endif
   app_quit();

   return 0;
}
