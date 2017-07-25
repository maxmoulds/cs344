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
  unsigned int connection_count;
  Room *connections[7];
};

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
pthread_t tid;
pthread_mutex_t lock;
char wd[256];

Room * room_read() {
  Room * rooms =  malloc(sizeof(Room)*MAX_CONNECTED_ROOMS);
  /* Read the game_state.txt file in */
  ssize_t read;
  FILE * file;
  char * line = NULL;
  size_t len;
  char * in = 0;
  int linecount = 0;
  char dir[256];
  file = fopen("game_state.txt", "r");
  trace("reading from %s", getcwd(NULL, 0));
  while (linecount < 9 && (read = getline(&line, &len, file)) != -1) {
    trace("Retrieved line of length %zu :\n", read);
    trace("Line is :: %s", line);
    if (linecount == 0) {
      strcpy(dir, line);
      linecount += 1;
      /*wd = (char *) malloc(strlen(dir)*sizeof(char)+2*sizeof(char));*/
      strcpy(wd, dir);/* why does this not return right */
      trace("WD is %s", wd);
      /*chdir(wd);*/
    }
    else {  
      in = strtok(line, " ");
      trace("in is :: %s and line count is :: %d", in, linecount);
      char * temp = (char *) malloc(strlen(in)*sizeof(char));
      strcpy(temp, in);
      rooms[linecount-2].name = temp;
      in = strtok(NULL, " ");
      trace("in is :: %s", in);
      rooms[linecount-2].type = atoi(in);
      in = strtok(NULL, " ");
      trace("in is :: %s", in);
      rooms[linecount-2].connection_count = atoi(in);
      in = strtok(NULL, " ");
      trace("in is :: %s", in);
      rooms[linecount-2].cap_conns = atoi(in);
      trace("all done line is :: %s", line);
      /* connections in round 2? */
    }
    //printf("%s", line);
    linecount += 1;
    trace("linecount is %d", linecount);
  }
  trace("what do we have?");
  int m = 0;
  for (m = 0; m < MAX_CONNECTED_ROOMS; m++) {
    trace("Room %d is %s", m, rooms[m].name);
  }
  /* now add connections */
  fclose(file);
  file = fopen("game_state.txt", "r");
  line = NULL;
  linecount = 0;
  len = 0;
  while (linecount < 9 && (read = getline(&line, &len, file)) != -1) {
    trace("Retrieved line of length %zu :\n", read);
    trace("Line is :: %s", line);
    if (linecount == 0) {
      //strcpy(dir, line);
      linecount += 1;
    }   
    else {   
      in = strtok(line, " ");
      trace("in is :: %s and line count is :: %d", in, linecount);
      //char * temp = (char *) malloc(strlen(in)*sizeof(char));
      //strcpy(temp, in);
      //rooms[linecount-2].name = temp;
      in = strtok(NULL, " ");
      trace("in is :: %s", in);
      //rooms[linecount-2].type = atoi(in);
      in = strtok(NULL, " ");
      trace("in is :: %s", in);
      //rooms[linecount-2].connection_count = atoi(in);
      in = strtok(NULL, " ");
      trace("in is :: %s", in);
      //rooms[linecount-2].cap_conns = atoi(in);
      trace("all done line is :: %s", line);
      /* connections in round 2? */
      int n = 0;
      for (n = 0; n < rooms[linecount-2].connection_count; n++) {
        in = strtok(NULL, " ");
        trace("in is :: %s", in);
        /* iterate through rooms to find matching string */
        int p = 0;
        for (p = 0; p < MAX_CONNECTED_ROOMS; p++) {
          if (strcmp(in, rooms[p].name) == 0) {
            /* we need to link to that room */
            rooms[linecount-2].connections[n] = &rooms[p];
          }
        }
      }
    }   
    //printf("%s", line);
    linecount += 1;
    trace("linecount is %d", linecount);
  }
  /* lets print this all out */
  int i = 0;
  for (i = 0; i < MAX_CONNECTED_ROOMS; i++) {
    trace("ROOM NAME: %s", rooms[i].name);
    int j = 0;
    for (j = 0; j < rooms[i].connection_count; j++) {
      trace("CONNECTION %d: %s", j+1, rooms[i].connections[j]->name);
    }
    trace("ROOM TYPE: %s", room_type_string[rooms[i].type]);
  }
  fclose(file);
  remove("game_state.txt");
  return rooms;
}



void* time_print()
{
  pthread_mutex_lock(&lock);
  trace("started a thread, will this work?");
  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  /*lets check some stuffs */
  chdir(wd);
  trace("chdir may be needed? %s, but wd is %s", getcwd(NULL, 0), wd);
  /* prep new file */
  //char * filename = (char *) malloc(sizeof("currentTime.txt")+1);
  char * time_string = (char *) malloc(256);
  strftime(time_string, 256, "%l:%M%P, %A, %B %d, %Y", timeinfo);
  trace("time string test :: %s ", time_string);
  /* open file */
  char * checkpath = getcwd(NULL, 0); /* remember to free checkpath, also how does this handle errors? idk */
  char * file_path_to_create = (char *) malloc((strlen(checkpath) + strlen("currentTime.txt") + 2*strlen("/") + strlen("\n") + strlen(wd))*sizeof(char));
  char * end = file_path_to_create;
  trace("making file path?");
  end += sprintf(end, "%s", checkpath);
  end += sprintf(end, "%s", "/");
  end += sprintf(end, "%s", wd);
  end -= 1; /* remove the new line?*/
  end += sprintf(end, "%s", "/");
  end += sprintf(end, "%s", "currentTime.txt");
  trace("full path is :: %s", file_path_to_create);
  FILE *file = fopen(file_path_to_create, "ab+"); /* so this is absolute and could cause errors? */
  int _err_fwrite = fwrite(time_string, 1, strlen(time_string)+1, file);
  int _err_fclose = fclose(file);
  trace("Building file path for %s, in dir :: %s", file_path_to_create, checkpath);
  printf("%s\n", time_string);
  /* open, and write please */
  pthread_mutex_unlock(&lock);
  free(time_string);
  free(file_path_to_create);
  free(checkpath);
  return NULL;
}

void printRoom(Room * room) {
  printf("CURRENT LOCATION: %s\n", room->name);
  printf("POSSIBLE CONNECTIONS: ");
  int i = 0;
  for (i = 0; i < room->connection_count; i++) {
    printf("%s", (room->connections[i])->name);
    if (i < room->connection_count-1) {
      printf(", ");
    }
  }
  printf(".");
  printf("\nWHERE TO?: >");
}

int room_match(char * search, Room room, Room rooms[MAX_CONNECTED_ROOMS]) {
  /* first, grab any time commands */
  if (strcmp(search, "time") == 0) {
    /* we need to write the time to a file using mutexes and lpthread */
    trace("TIME COMMAND ISSUED");
    int err = pthread_create(&(tid), NULL, &time_print, NULL);
    if (err != 0) {
      trace("can't create thread :[%s]", strerror(err)); 
    }
    pthread_join(tid, NULL);
    pthread_mutex_destroy(&lock);

    /* set the state to invalid input, without reprint, actually just continue*/
    printf("WHERE TO?: >");
    return -1; //for now
  }
  trace("matching %s to a room name", search);
  int i = 0;
  for ( i = 0; i < room.connection_count; i++)
  {
    if (strcmp(search, room.connections[i]->name) == 0) {
      /* Its a connected room, so now return the right index in the the room_list */
      int j = 0;
      for (j = 0; j < MAX_CONNECTED_ROOMS; j++)
      {
        trace("testing %s against %s", room.connections[i]->name, rooms[j].name);
        if (strcmp(room.connections[i]->name, rooms[j].name) == 0) {
          return j;
        }
      }
      /* so something super weird happened err out */
      trace("YOU SHOULD NEVER SEE THIS MESSAGE - no matching room in room_list");
      return -1;

    }
  }
  /* no match, what to do? */
  trace("No match for input %s", search);
  printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
  printRoom(&room);
  return -2;
}

int input_room(char * line, int size_of_line, Room current_room, Room rooms[MAX_CONNECTED_ROOMS-1]) {
  trace("in INPUT...");
  int i;
  if (fgets(line, size_of_line, stdin)) {
    if (1 == sscanf(line, "%d", &i)) {
      /* i can be safely used */
    }   
  }
  strtok(line, "\n");
  trace("I scanned in %s", line);
  int to_move_to_room = room_match(line, current_room, rooms);
  while (to_move_to_room == -2) {
    /* wrong stuff entered */
    if (fgets(line, sizeof(line), stdin)) {
      if (1 == sscanf(line, "%d", &i)) {
        /* i can be safely used */
      }   
    }   
    strtok(line, "\n");
    trace("I scanned in %s", line);

    to_move_to_room = room_match(line, current_room, rooms);
  }
  if (to_move_to_room == -1) {
    /* EASY CHEAT HERE, */
    trace("BACK IN THE... US-");
    to_move_to_room == -2;
    /* self call */
    return input_room(line, size_of_line, current_room, rooms);
    /* this wont work */
  }
  if (to_move_to_room == -1) {
    trace("This should never happen, for realz");
    exit -1;
  }
  return to_move_to_room;
}

int adventure(Room rooms[MAX_CONNECTED_ROOMS]) {
  /* What to do, what to do, */
  /* lets test the functions */ 
  trace("Hey were in the adventure now....");
  int road[256]; /* here is where you will store where the user went */ 
  int steps = 0; /* this is the number of "turns" and index to road */
  /* So find the starting room? */
  trace("I hope this is the starting room - %s and type %s", rooms[0].name, room_type_string[rooms[0].type]);
  /* so spec says to re-read the files, okay, and find the one with the newest
   * mtime */
  trace("end room is %s", rooms[MAX_CONNECTED_ROOMS-1].name);
  char * checkpath = getcwd(NULL, 0);
  trace("checking path :: %s ", checkpath);
  printRoom(&rooms[0]);
  Room current_room = rooms[0];
  road[steps] = 0; /* starting room */
  char line[256];
  int to_move_to_room = input_room(line, sizeof(line), current_room, rooms);
  trace("we found a room %d :: %s which matched input of %s", to_move_to_room, rooms[to_move_to_room].name, line);
  /* now move to that room lets loop this. */
  /* we know one move is needed. */
  current_room = rooms[to_move_to_room];
  steps++;
  road[steps] = to_move_to_room;
  /* Loop until end room is reached */
  while (to_move_to_room != (MAX_CONNECTED_ROOMS-1)) {
    trace("Not an end room yet, %d, %d", to_move_to_room, MAX_CONNECTED_ROOMS-1);
    trace("end room is %s", rooms[MAX_CONNECTED_ROOMS-1].name);
    printRoom(&current_room);
    to_move_to_room = input_room(line, sizeof(line), current_room, rooms);
    trace("we found a room %d :: %s which matched input of %s", to_move_to_room, rooms[to_move_to_room].name, line);
    current_room = rooms[to_move_to_room];
    steps++;
    road[steps] = to_move_to_room;
  }
  //free(line);//for some reason this barfs
  trace("freeing checkpath");
  free(checkpath);
  /* Well to get here you must have been a winner */
  trace("So END_ROOM is %s, and you are in the %s", rooms[MAX_CONNECTED_ROOMS-1].name, current_room.name);
  trace("you took %d steps on your path to the end", steps);
  printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
  printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps+1);
  int i = 0;
  for (i = 0; i <= steps; i++) {
    printf("%s\n",rooms[road[i]].name);
  }
  return 0;
}
int main() {
  Room * room_list = room_read();
  return adventure(room_list);
}
