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
#include <stdint.h>
#include <time.h>
#include <errno.h>

#include "joy_RPi.h"


#include <bcm2835.h>
#include "gpio_joystick.h"

#define HC165JOY 1


static void showHelp(void);
static void showVersion(void);

bool no_key_debounce = false;
bool skip_mouse_init = false;
static unsigned int poll_us = 4000;
static unsigned int benchmark_scans = 0;
static uint64_t ns_now(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

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
    else if(!strcmp(argv[i], "--poll-us") && i+1 < argc){
      char *end = NULL;
      unsigned long v = strtoul(argv[++i], &end, 10);
      if (!end || *end || v < 500 || v > 20000) {
        fprintf(stderr, "--poll-us must be 500..20000\n"); return 2;
      }
      poll_us = (unsigned int)v;
    }
    else if(!strcmp(argv[i], "--benchmark") && i+1 < argc){
      char *end = NULL;
      unsigned long v = strtoul(argv[++i], &end, 10);
      if (!end || *end || v < 10 || v > 1000000) {
        fprintf(stderr, "--benchmark must be 10..1000000 scans\n"); return 2;
      }
      benchmark_scans = (unsigned int)v;
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

      uint64_t scan_total = 0, scan_min = UINT64_MAX, scan_max = 0;
      uint64_t cycle_total = 0, cycle_min = UINT64_MAX, cycle_max = 0;
      uint64_t previous_start = 0;
      unsigned int count = 0;
      for(;;){
        uint64_t start = ns_now();
        if (previous_start) {
          uint64_t cycle = start - previous_start;
          cycle_total += cycle;
          if (cycle < cycle_min) cycle_min = cycle;
          if (cycle > cycle_max) cycle_max = cycle;
        }
        previous_start = start;
        joy_RPi_poll();
        uint64_t elapsed = ns_now() - start;
        scan_total += elapsed;
        if (elapsed < scan_min) scan_min = elapsed;
        if (elapsed > scan_max) scan_max = elapsed;
        count++;
        if (benchmark_scans && count >= benchmark_scans) {
          printf("metric,count,min_us,mean_us,max_us\n");
          printf("scan,%u,%.3f,%.3f,%.3f\n",count,scan_min/1000.0,(double)scan_total/count/1000.0,scan_max/1000.0);
          if (count > 1) printf("cycle,%u,%.3f,%.3f,%.3f\n",count-1,cycle_min/1000.0,(double)cycle_total/(count-1)/1000.0,cycle_max/1000.0);
          break;
        }
        usleep(poll_us);
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
  printf("  --poll-us N   sleep after scan (500..20000 us; default 4000)\n");
  printf("  --benchmark N measure N scans and print timings\n");
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
