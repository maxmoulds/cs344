#ifndef HEADER_H_
#define HEADER_H_

/*Change HEADER_H to whatever */
/* use #DEFINE DEBUG {1 2 3} to get debug */
#define DEBUG 3

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
/* file and dir stuffs */
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h> /*grr */
#include <sys/fcntl.h> /*uber grr*/

#include <string.h>

/*threads? */
#include <pthread.h>

/* Room names */
#define MAX_CONNECTED_ROOMS 7
const char * room_names[10] = {"blue","red","green","purple","black","orange","yellow","gold","white","pink"};
enum room_type {START_ROOM,END_ROOM,MID_ROOM};
const char * room_type_string[3] = {"START_ROOM", "END_ROOM", "MID_ROOM"};
typedef struct Room Room;
struct Room {
  enum room_type type;
  const char *name;
  unsigned int cap_conns;
  unsigned int num_conns;
  Room *connections[7];
};

bool AddConnection(Room * room1, Room * room2, Room room_list[MAX_CONNECTED_ROOMS]);
bool CanAddConnection(Room * room1, Room * room2);
Room * buildrooms();
int adventure(Room rooms[MAX_CONNECTED_ROOMS]); 

#define ONID "mouldsm"
#define MIN_CONNECTIONS 3
#define MAX_CONNECTIONS 6

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

#endif
