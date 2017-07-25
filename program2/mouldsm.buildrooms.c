//#define DEBUG 3

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
  unsigned int connection_count;
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

#if defined(DEBUG) && DEBUG > 1
#define trace(fmt, args...) fprintf(stderr, ANSI_COLOR_YELLOW "DEBUG %s:%d:%s(): " ANSI_COLOR_RESET fmt "\n", __FILE__, __LINE__, __func__, ##args)
#else
#define trace(fmt, args...) /* Don't do anything in release builds */
#endif

/* One day ill get rid of the header file and this global... */
Room room_list[7];
/* lpthread stuffs */

Room * buildrooms() {
  /* So here is the plan, 
   * 1) Create main directory....
   * 2) generate 7 random rooms...
   * 3) connect the 7 random rooms...
   * 4) write the connection info to the files...
   * 5) pass either the files or the struct in memory to the game logic...
   * 6) drink.
   * 7) return the room_list array to whatever called this to-be buildroom
   * function!!! */
  srand(time(NULL));
  /* lets test some dir making */
  struct stat s;
  /* make dir name */
  char * dirname = (char *) malloc(strlen(ONID) + strlen(".rooms.") + (sizeof(char) * sizeof(long)));
  char * end = dirname;
  end += sprintf(end, "%s", ONID);
  trace("sprintfing chars to :: %s", dirname);
  end += sprintf(end, "%s", ".rooms.");
  trace("sprintfing chars to :: %s", dirname);
  end += sprintf(end, "%ld", getpid());
  trace("sprintfing chars to :: %s", dirname);
  trace("Should have made a string of length %d to hold the string of actual length %d ", (strlen(ONID) + strlen(".rooms.") + (sizeof(char) * sizeof(long))), strlen(dirname));
  trace("You wasted %d bytes of memory, congrats", ((strlen(ONID) + strlen(".rooms.") + (sizeof(char) * sizeof(long))) - strlen(dirname)));
  int err = lstat(dirname, &s);
  if(-1 == err) {
    if(ENOENT == errno) {
      /* does not exist */
      trace("There is no directory at : %s  --- creating a directory called : %s", getcwd(NULL, 0), dirname); /* I LEAKED */
      mkdir(dirname, S_IRUSR | S_IWUSR | S_IXUSR);/* 777 is also S_IRWXU or even ACCESSPERMS */
      /* 1) IS DONE, Now lets make random numbers for room_list...*/
      chdir(dirname);
      bool noap[MAX_CONNECTED_ROOMS];/* I tried 10 different ways, bool arrays seem cheeszy bbbuut */
      memset(&noap, 0, MAX_CONNECTED_ROOMS * sizeof(bool)); /* Clearing with memset, oo what a cool tool */
      for (int i = 0; i < MAX_CONNECTED_ROOMS; i++) {
        room_list[i].connection_count = 0; /* For now nothing is connected, not max or min here brosif */
        int cap_conns = (rand() % (MAX_CONNECTIONS-(MIN_CONNECTIONS-2))) + MIN_CONNECTIONS-2 ; /* Okay I lied, we are going to set connection numbers */
        room_list[i].cap_conns = cap_conns;
        /* Could this count forever? */
        while (true) {
          int room_index = rand() % MAX_CONNECTED_ROOMS;
          if (!noap[room_index]) {
            noap[room_index] = true;
            room_list[i].name = room_names[room_index];
            break;
          }
        }
        room_list[i].type = MID_ROOM;/* Every room is a mid_room for now... */
      }
      trace("Initialized the rooms...connecting");
      int rando = -1;
      for (int i = 0; i < MAX_CONNECTED_ROOMS; i++) {
        for (int j = 0; j < room_list[i].cap_conns; j++) {
          rando = rand() % MAX_CONNECTED_ROOMS;
          trace("the random number should be between 0 and 6 and is %d", rando);
          while (!AddConnection(&(room_list[i]), &(room_list[rando]), room_list)) {
            rando = rand() % MAX_CONNECTED_ROOMS;
          }
        }
        trace("Thank you Richard Stallman or whoever is to blame.");
      }
      room_list[0].type = START_ROOM;
      room_list[MAX_CONNECTED_ROOMS-1].type = END_ROOM;
      trace("WE JUST SET THE START AND END ROOM AND THEY ARE %s and %s respectively", room_list[0].name, room_list[MAX_CONNECTED_ROOMS-1].name);
      /* So lets recap the state at this point */
      /* room_list is a array of Room structs, each room is one of the 7 for a given run */
      /* NOW IO ME */
      for (int i = 0; i < MAX_CONNECTED_ROOMS; i++) {
        char * checkpath = getcwd(NULL, 0); /* remember to free checkpath, also how does this handle errors? idk */
        trace("Should be in the new directory = %s so pwd is %s", dirname, checkpath);
        char * file_path_to_create = (char *) malloc(strlen(checkpath) + strlen(room_list[i].name) + (sizeof("/")));
        trace("Building file path for %s", room_list[i].name);
        end = file_path_to_create;
        end += sprintf(end, "%s", checkpath);
        end += sprintf(end, "%s", "/");
        end += sprintf(end, "%s", room_list[i].name);
        trace("This is the new file being created :: %s ", file_path_to_create);
        FILE *file = fopen(room_list[i].name, "ab+");
        /* So we have the room open, lets add the connections now too */
        char * room_info = (char *) malloc(strlen("ROOM NAME: ") + strlen(room_list[i].name) + 3);
        /* Building each room info */
        end = room_info;
        end += sprintf(end, "%s", "ROOM NAME: ");
        end += sprintf(end, "%s", room_list[i].name);
        end += sprintf(end, "%s", "\n");
        trace("This is what is being written to the file (%d) :: %s ", strlen(room_info), room_info);
        int _err_fwrite = fwrite(room_info, 1, strlen(room_info)+1, file);
        trace("fwrite returned :: %d", _err_fwrite);
        /* Build connection strings. */
        char * room_connection_info;
        int room_connection_info_length = 0;
        for (int j = 0; j < room_list[i].connection_count; j++) {
          room_connection_info_length += strlen("CONNECTION ");
          room_connection_info_length += snprintf(NULL, 0,"%d",j); /* deerty */
          room_connection_info_length += strlen((room_list[i].connections[j])->name);
          room_connection_info_length += strlen(": \n");
          /* we couild cheat and use fprintf(file, "CONNECTION %d: %s\n", j + 1, rooms[j].name); */
          /* or realloc...*/
        }
        trace("type is %d, so type strint is %s", room_list[i].type, room_type_string[room_list[i].type]);
        room_connection_info_length += strlen(room_type_string[room_list[i].type]) + 4;
        room_connection_info_length += strlen("ROOM TYPE: ");

        /* k malloc then loop again haha */
        trace("we are going to malloc %d bytes to write room connection info", room_connection_info_length);
        room_connection_info = (char *) malloc(room_connection_info_length * sizeof(char));
        char * end = room_connection_info;
        for (int j = 0; j < room_list[i].connection_count; j++) {
          end += sprintf(end, "%s", "CONNECTION ");
          end += sprintf(end, "%d", j+1);
          end += sprintf(end, "%s", ": ");
          end += sprintf(end, "%s", (room_list[i].connections[j])->name);
          end += sprintf(end, "%s", "\n");
        }
        /* Annnnd add the room type string */
        end += sprintf(end, "%s", "ROOM TYPE: "); 
        end += sprintf(end, "%s", room_type_string[room_list[i].type]);
        end += sprintf(end, "%s", "\n");
        trace("Going to write : --------------\n%s\n---------- to %s", room_connection_info, room_list[i].name);
        _err_fwrite = fwrite(room_connection_info, 1, strlen(room_connection_info)+1, file); 

        int _err_fclose = fclose(file);
        if (_err_fclose != 0) {
          trace("A file did not close, ignoring for now :: %s ", file_path_to_create);
        }
        trace("freeing..");
        free(room_info);
        free(checkpath);
        free(file_path_to_create);
        free(room_connection_info);
      }
      free(dirname);

    } else {
      trace("Something really nasty happened with/at the : %s -- path", dirname);
      free(dirname);/* DANGER WILL ROBINSON */
      /* do i exit right? */
      exit(1);
    }
  } else {
    if(S_ISDIR(s.st_mode)) {
      /* it's a dir */
      trace("That directory already exists as : %s", dirname);
    } else {
      /* exists but is no dir */
      trace("There is something else at : %s", dirname);
    }
  }
  /* since adventure has no knowledge of this variable room_list, lets writeout*/
  chdir("..");
  char * dir = (char *) malloc(strlen(ONID) + strlen(".rooms.") + (sizeof(char) * sizeof(long)) + 2*sizeof(char));
  char * nd = dir;
  nd += sprintf(nd, "%s", ONID);
  trace("sprintfing chars to :: %s", dir);
  nd += sprintf(nd, "%s", ".rooms.");
  trace("sprintfing chars to :: %s", dir);
  nd += sprintf(nd, "%ld", getpid());
  trace("sprintfing chars to :: %s", dir);
  nd += sprintf(nd, "%s", "\n");
  FILE *file = fopen("game_state.txt", "w+"); /* so this is absolute and could cause errors? */
  int _err_fwrite = fwrite(dir, 1, strlen(dir), file);

  for (int i = 0; i < MAX_CONNECTED_ROOMS; i++) {
    /*so we need to write all of the stuct to a temp file */
    /*typedef struct Room Room;
     * struct Room {
     * enum room_type type;
     * const char *name;
     * unsigned int cap_conns;
     * unsigned int connection_count;
     * Room *connections[7];
     * }; */
    char * record = (char *) malloc(1024*sizeof(char));
    char * end = record;
    end += sprintf(end, "%s", room_list[i].name);
    end += sprintf(end, "%s", " ");
    end += sprintf(end, "%d", room_list[i].type);
    end += sprintf(end, "%s", " ");
    end += sprintf(end, "%d", room_list[i].connection_count);
    end += sprintf(end, "%s", " ");
    end += sprintf(end, "%d", room_list[i].cap_conns);
    end += sprintf(end, "%s", " ");
    for (int j = 0; j < room_list[i].connection_count; j++){
      end += sprintf(end, "%s", room_list[i].connections[j]->name);
      end += sprintf(end, "%s", " ");
    }
    end += sprintf(end, "%s", "\n");
    //end += sprintf(end, "%s", "\0");
    trace("The big one------------\n %s\n------------",record);
    int _err_fwrite = fwrite(record, 1, strlen(record), file);
    free(record);
  }
  int _err_fclose = fclose(file);
  return room_list;
}

bool AddConnection(Room * room1, Room * room2, Room room_list[MAX_CONNECTED_ROOMS]) {
  Room *r1 = room1;
  Room *r2 = room2;
  if (r1->connection_count == MAX_CONNECTIONS) {
    return true;
  }
  if (CanAddConnection(room1, room2)) {
    return false;
  }
  if (r1->connection_count >= MAX_CONNECTIONS || r2->connection_count >= MAX_CONNECTIONS) {
    return false;
  }
  r1->connections[r1->connection_count] = r2;
  r2->connections[r2->connection_count] = r1;
  r1->connection_count++;
  r2->connection_count++;
  return true;
}
bool CanAddConnection(Room * room1, Room * room2) {
  /* DO YOU SELF-IDENTIFY AS BEING CONNECTED TO YOURSELF ? */
  if (strcmp(room1->name,room2->name) == 0) {
    return true;
  }
  for (int i = 0; i < room1->connection_count; i++) {
    if ((strcmp((room1->connections[i])->name,room2->name) == 0) &&  room1->connections[i] != NULL) {
      return true;
    }
  }
  return false;
}

int main() {
  Room * built_rooms = buildrooms();
  return 0;
}

