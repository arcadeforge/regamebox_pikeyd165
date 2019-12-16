/**** uinput.c *****************************/
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
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include "config.h"
#include "daemon.h"
#include "time.h"
#include "uinput.h"
#include <stdbool.h>

int sendKey(int key, int value);
int sendRel(int devm, int dx, int dy);
static int sendSync(int);

static struct input_event     uidev_ev;
static int uidev_fd_key;
int uidev_fd_mouse1;
int uidev_fd_mouse2;



#define die(str, args...) do { \
        perror(str); \
        return(EXIT_FAILURE); \
    } while(0)

int init_uinput(bool skip_mouse_init)
{
  int fd;
  struct uinput_user_dev uidev;
  int i;

  fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  if(fd < 0)
    die("/dev/uinput");

  if(ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
    die("error: ioctl");

  if(ioctl(fd, UI_SET_EVBIT, EV_REP) < 0)
    die("error: ioctl");


  //alter code (mouse im keydevice)  
#if 0  
    if( skip_mouse_init == false) {
    if(ioctl(fd, UI_SET_KEYBIT, BTN_LEFT) < 0)
      die("error: ioctl");
    
       if(ioctl(fd, UI_SET_EVBIT, EV_REL) < 0)
       die("error: ioctl");
     if(ioctl(fd, UI_SET_RELBIT, REL_X) < 0)
       die("error: ioctl");
     if(ioctl(fd, UI_SET_RELBIT, REL_Y) < 0)
       die("error: ioctl");
     }
#endif
  
  /* don't forget to add all the keys! */
  for(i=0; i<256; i++){
    if(ioctl(fd, UI_SET_KEYBIT, i) < 0)
      die("error: ioctl");
  }

  memset(&uidev, 0, sizeof(uidev));
  snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample0");
  uidev.id.bustype = BUS_USB;
  uidev.id.vendor  = 0x1;
  uidev.id.product = 0x1;
  uidev.id.version = 1;

  if(write(fd, &uidev, sizeof(uidev)) < 0)
    die("error: write");

  if(ioctl(fd, UI_DEV_CREATE) < 0)
    die("error: ioctl");

  uidev_fd_key = fd;



  

 /******************mouses******************/

  if( skip_mouse_init == false) {

  printf("Mouse/Trackball enabled\n");

  //mouse 1
  fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  if(fd < 0)
    die("/dev/uinput");


  //    if(ioctl(fd, UI_SET_KEYBIT, BTN_LEFT) < 0)
  //    die("error: ioctl");

    if(ioctl(fd, UI_SET_EVBIT, EV_REL) < 0)
      die("error: ioctl");
    if(ioctl(fd, UI_SET_RELBIT, REL_X) < 0)
      die("error: ioctl");
    if(ioctl(fd, UI_SET_RELBIT, REL_Y) < 0)
      die("error: ioctl");
  
  memset(&uidev, 0, sizeof(uidev));
  snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample1");
  uidev.id.bustype = BUS_USB;
  uidev.id.vendor  = 0x1;
  uidev.id.product = 0x1;
  uidev.id.version = 1;

  if(write(fd, &uidev, sizeof(uidev)) < 0)
    die("error: write");

  if(ioctl(fd, UI_DEV_CREATE) < 0)
    die("error: ioctl");

  uidev_fd_mouse1 = fd;

  //mouse 2
  fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  if(fd < 0)
    die("/dev/uinput");

  //    if(ioctl(fd, UI_SET_KEYBIT, BTN_LEFT) < 0)
  //    die("error: ioctl");

  if(ioctl(fd, UI_SET_EVBIT, EV_REL) < 0)
    die("error: ioctl");
  if(ioctl(fd, UI_SET_RELBIT, REL_X) < 0)
    die("error: ioctl");
  if(ioctl(fd, UI_SET_RELBIT, REL_Y) < 0)
    die("error: ioctl");

  memset(&uidev, 0, sizeof(uidev));
  snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample2");
  uidev.id.bustype = BUS_USB;
  uidev.id.vendor  = 0x1;
  uidev.id.product = 0x1;
  uidev.id.version = 1;

  if(write(fd, &uidev, sizeof(uidev)) < 0)
    die("error: write");

  if(ioctl(fd, UI_DEV_CREATE) < 0)
    die("error: ioctl");

  uidev_fd_mouse2 = fd;


  
  }
 /******************mouses******************/








  
  return 0;
}

int test_uinput(void)
{
  int                    dx, dy;
  int                    i,k;

  srand(time(NULL));

  while(1) {
    switch(rand() % 4) {
    case 0:
      dx = -10;
      dy = -1;
      break;
    case 1:
      dx = 10;
      dy = 1;
      break;
    case 2:
      dx = -1;
      dy = 10;
      break;
    case 3:
      dx = 1;
      dy = -10;
      break;
    }

    k = send_gpio_keys(21, 1);
    sendKey(k, 0);

    for(i = 0; i < 20; i++) {
      //sendKey(KEY_D, 1);
      //sendKey(KEY_D, 0);
      sendRel(uidev_fd_mouse1,dx, dy);

      usleep(15000);
    }

    sleep(10);
  }
}

int close_uinput(bool skip_mouse_init)
{
  sleep(2);

  if(ioctl(uidev_fd_key, UI_DEV_DESTROY) < 0)
    die("error: ioctl");

  close(uidev_fd_key);


/******************mouses******************/
  if( skip_mouse_init == false) {

  if(ioctl(uidev_fd_mouse1, UI_DEV_DESTROY) < 0)
    die("error: ioctl");

  close(uidev_fd_mouse1);


  if(ioctl(uidev_fd_mouse2, UI_DEV_DESTROY) < 0)
    die("error: ioctl");

  close(uidev_fd_mouse2);
  }

  
/******************mouses******************/


  
  return 0;
}

int sendKey(int key, int value)
{
  memset(&uidev_ev, 0, sizeof(struct input_event));
  gettimeofday(&uidev_ev.time, NULL);
  uidev_ev.type = EV_KEY;
  uidev_ev.code = key;

  //CM sanity fix
  if(value!=0)
    value=1;
  
  uidev_ev.value = value;
  if(write(uidev_fd_key, &uidev_ev, sizeof(struct input_event)) < 0)
    die("error: write");

  sendSync(uidev_fd_key);

  return 0;
}

int sendRel(int devm,int dx, int dy)
{
  //printf("dx: %i dy: %i\n",dx,dy);

  memset(&uidev_ev, 0, sizeof(struct input_event));
  uidev_ev.type = EV_REL;
  uidev_ev.code = REL_X;
  uidev_ev.value = dx;
  if(write(devm, &uidev_ev, sizeof(struct input_event)) < 0)
    die("error: write");
  
  memset(&uidev_ev, 0, sizeof(struct input_event));
  uidev_ev.type = EV_REL;
  uidev_ev.code = REL_Y;
  uidev_ev.value = dy;
  if(write(devm, &uidev_ev, sizeof(struct input_event)) < 0)
    die("error: write");

  sendSync(devm);

  return 0;
}

static int sendSync(int devm)
{
  //memset(&uidev_ev, 0, sizeof(struct input_event));
  uidev_ev.type = EV_SYN;
  uidev_ev.code = SYN_REPORT;
  uidev_ev.value = 0;
  if(write(devm, &uidev_ev, sizeof(struct input_event)) < 0)
    die("error: sendSync - write");

  return 0;
}

int send_gpio_keys(int gpio, int value)
{
  int k;
  restart_keys();
  while( got_more_keys(gpio) ){
    k = get_next_key(gpio);
    sendKey(k, value);
    if(value && got_more_keys(gpio)){
      /* release the current key, so the next one can be pressed */
      sendKey(k, 0);
    }
  }
  return k;
}
