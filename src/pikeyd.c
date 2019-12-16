/**** pikeyd.c *****************************/
/* M. Moller   2013-01-16                  */
/*   Universal RPi GPIO keyboard daemon    */
/*******************************************/

/*
   Copyright (C) 2013 Michael Moller.
   This file is part of the Universal Raspberry Pi GPIO keyboard daemon.

   This is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The software is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "joy_RPi.h"


#include <bcm2835.h>
#include "gpio_joystick.h"

#define HC165JOY 1


static void showHelp(void);
static void showVersion(void);

bool no_key_debounce = false;
bool skip_mouse_init = false;

int main(int argc, char *argv[])
{


  int i;
  
  if(argc>1){

  for (i = 1; i < argc; i++) {
    if(!strcmp(argv[i], "-d")){
      daemonize("/tmp", "/tmp/pikeyd.pid");
    }
    else if(!strcmp(argv[i], "-k")){
      daemonKill("/tmp/pikeyd.pid");
      exit(0);
    }
    else if(!strcmp(argv[i], "-v")){
      showVersion();
      exit(0);
    }
    else if(!strcmp(argv[i], "-h")){
      showHelp();
      exit(0);
    }
    else if(!strcmp(argv[i], "-smi")){
      printf("mouse disabled\n");
      skip_mouse_init = true;
    }
    else if(!strcmp(argv[i], "-ndb")){
      printf("no key debounce\n");
      no_key_debounce = true;
    }
    
  }        
  }

  
  



  
  init_config();
  //test_config(); exit(0);

  printf("init uinput\n");

  if(init_uinput(skip_mouse_init) == 0){
    sleep(1);
    //test_uinput();


    if(joy_RPi_init()>=0){

      for(;;){
	joy_RPi_poll();
	usleep(4000);	
      }

      joy_RPi_exit();
    }

    close_uinput(skip_mouse_init);
  }

  return 0;
}

static void showHelp(void)
{
  printf("Usage: pikeyd [option]\n");

  printf("Options:\n");
  printf("  -d    run as daemon\n");
  printf("  -k    try to terminate running daemon\n");
  printf("  -v    version\n");
  printf("  -smi  skip mouse initialisation, no mouse/trackball devices are created\n");
  printf("  -ndb  no key debounce \n");
  printf("  -h    this help\n");
}

static void showVersion(void)
{
  printf("\npikeyd - 2017 \n");
  printf("The Universal Raspberry Pi GPIO keyboard daemon.\n");
  printf("Copyright (C) 2013 Michael Moller.\n");
  printf("This is free software; see the source for copying conditions.  There is NO\n");
  printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n");

  printf("This version of pikeyd is heavily modified for use with the pi2jamma hardware. \n");
  printf("Input is read via gpio bitstream, it supports 24 buttons, 2 Trackballs and 4 analog channels \n\n");
  printf("Buildinfo:\n",BDATE);
  printf("build directory: %s\n",BVERSION);
  printf("build date: %s\n\n",BDATE);
}
