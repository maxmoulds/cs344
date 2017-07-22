#ifndef HEADER_H_
#define HEADER_H_

/*Change HEADER_H to whatever */
/* use #DEFINE DEBUG {1 2 3} to get debug */
#define DEBUG 3

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/* file and dir stuffs */
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h> /*grr */

#include <string.h>

/* Room names */
#define ROOM_GREEN "green"
#define ROOM_YELLOW "yellow"
#define ROOM_BLUE "blue"
#define ROOM_RED "red"
#define ROOM_BLACK "black"
#define ROOM_BROWN "brown"
#define ROOM_WHITE "white"
#define ROOM_PURPLE "purple"
#define ROOM_ORANGE "orange"
#define ROOM_GOLD "gold"
#define ROOM_PINK "pink"

#define ONID "mouldsm"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#if defined(DEBUG) && DEBUG > 2
#define info(fmt, args...) fprintf(stderr, ANSI_COLOR_GREEN "DEBUG %s:%d:%s(): " ANSI_COLOR_RESET fmt "\n", __FILE__, __LINE__, __func__, ##args)
#else
#define info(fmt, args...) /* Don't do anything in release builds */
#endif

#if defined(DEBUG) && DEBUG > 1
#define trace(fmt, args...) fprintf(stderr, ANSI_COLOR_YELLOW "DEBUG %s:%d:%s(): " ANSI_COLOR_RESET fmt "\n", __FILE__, __LINE__, __func__, ##args)
#else
#define trace(fmt, args...) /* Don't do anything in release builds */
#endif

#if defined(DEBUG) && DEBUG > 0
#define err(fmt, args...) fprintf(stderr, ANSI_COLOR_RED "DEBUG %s:%d:%s(): " ANSI_COLOR_RESET fmt "\n", __FILE__, __LINE__, __func__, ##args)
#else
#define err(fmt, args...) /* Don't do anything in release builds */
#endif

int  _test_debug(int argc, char **args) {
  int i = 10;
  info("Welcome");
  info("Here is some info");
  err("oops an error, i was = %d", i);
  trace("Debugging is enabled.");
  trace("Debug level: %d", i);
  
  /* Some time demo stuff */
  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  info( "Current local time and date: %s", asctime (timeinfo) );
  return 0;
}

#endif
