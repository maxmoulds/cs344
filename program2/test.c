#include "header.h"
Room room_list[7];

int main(int argc, char *argv[]) {
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
  char * path = "./temp";
  char * filename = "test";
  /* make dir name */
  char * dirname = (char *) malloc(strlen(ONID) + strlen(".rooms.") + (sizeof(char) * sizeof(long)));
  char * end = dirname;
  end += sprintf(end, "%s", ONID);
  trace("sprintfing chars to :: %s", dirname);
  end += sprintf(end, "%s", ".rooms.");
  trace("sprintfing chars to :: %s", dirname);
  end += sprintf(end, "%ld", getpid());
  trace("sprintfing chars to :: %s", dirname);
  info("Should have made a string of length %d to hold the string of actual length %d ", (strlen(ONID) + strlen(".rooms.") + (sizeof(char) * sizeof(long))), strlen(dirname));
  trace("You wasted %d bytes of memory, congrats", ((strlen(ONID) + strlen(".rooms.") + (sizeof(char) * sizeof(long))) - strlen(dirname)));
  int err = lstat(path, &s);
  if(-1 == err) {
    if(ENOENT == errno) {
      /* does not exist */
      info("There is no directory at : %s  --- creating a directory called : %s", path, dirname);
      mkdir(dirname, S_IRUSR | S_IWUSR | S_IXUSR);/* 777 is also S_IRWXU or even ACCESSPERMS */
      /* 1) IS DONE, Now lets make random numbers for room_list...*/
      chdir(dirname);
      bool noap[MAX_CONNECTED_ROOMS];/* I tried 10 different ways, bool arrays seem cheeszy bbbuut */
      memset(&noap, 0, MAX_CONNECTED_ROOMS * sizeof(bool)); /* Clearing with memset, oo what a cool tool */
      for (int i = 0; i < MAX_CONNECTED_ROOMS; i++) {
        room_list[i].num_conns = 0; /* For now nothing is connected, not max or min here brosif */
        int cap_conns = rand() % (MAX_CONNECTIONS - MIN_CONNECTIONS); /* Okay I lied, we are going to set connection numbers */
        cap_conns += MIN_CONNECTIONS-2;
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
      //DELETE MEEEEEEEEEE
      for (int i = 0; i < 7; i++) {
        printf("%s, and type is %d\n", room_list[i].name, room_list[i].type);
      }
      //CUZ I HAVE NO WAY
      trace("Initialized the rooms...connecting");
      int rando = -1;
      for (int i = 0; i < MAX_CONNECTED_ROOMS; i++) {
        for (int j = 0; j < room_list[i].cap_conns; j++) {
          rando = rand() % MAX_CONNECTED_ROOMS;
          trace("the random number should be between 0 and 7 and is %d", rando);
          while (!AddConnection(&(room_list[i]), &(room_list[rando]), room_list)) {
            rando = rand() % MAX_CONNECTED_ROOMS;
          }
        }
        info("Thank you Richard Stallman or whoever is to blame.");
      }
      room_list[0].type = START_ROOM;
      room_list[MAX_CONNECTED_ROOMS-1].type = END_ROOM;
      info("WE JUST SET THE START AND END ROOM AND THEY ARE %s and %s respectively", room_list[0].name, room_list[MAX_CONNECTED_ROOMS-1].name);
      /* So lets recap the state at this point */
      /* room_list is a array of Room structs, each room is one of the 7 for a given run */

      for (int i = 0; i < MAX_CONNECTED_ROOMS; i++) {
        char * checkpath = getcwd(NULL, 0); /* remember to free checkpath, also how does this handle errors? idk */
        trace("Should be in the new directory = %s so pwd is %s", dirname, checkpath);
        char * file_path_to_create = (char *) malloc(strlen(checkpath) + strlen(room_list[i].name) + (sizeof("/")));
        info("Building file path for %s", room_list[i].name);
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
        for (int j = 0; j < room_list[i].num_conns; j++) {
          room_connection_info_length += strlen("CONNECTION ");
          room_connection_info_length += snprintf(NULL, 0,"%d",j); /* deerty */
          room_connection_info_length += strlen((room_list[i].connections[j])->name);
          room_connection_info_length += strlen(": \n");
          /* we couild cheat and use fprintf(file, "CONNECTION %d: %s\n", j + 1, rooms[j].name); */
          /* or realloc...*/
        }
        info("type is %d, so type strint is %s", room_list[i].type, room_type_string[room_list[i].type]);
        room_connection_info_length += strlen(room_type_string[room_list[i].type]) + 4;
        room_connection_info_length += strlen("ROOM TYPE: ");

        /* k malloc then loop again haha */
        info("we are going to malloc %d bytes to write room connection info", room_connection_info_length);
        room_connection_info = (char *) malloc(room_connection_info_length * sizeof(char));
        char * end = room_connection_info;
        for (int j = 0; j < room_list[i].num_conns; j++) {
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
        info("Going to write : --------------\n%s\n---------- to %s", room_connection_info, room_list[i].name);
        _err_fwrite = fwrite(room_connection_info, 1, strlen(room_connection_info)+1, file); 

        int _err_fclose = fclose(file);
        if (_err_fclose != 0) {
          err("A file did not close, ignoring for now :: %s ", file_path_to_create);
        }
        info("freeing..");
        free(room_info);
        free(checkpath);
        free(file_path_to_create);
        free(room_connection_info);
      }
      free(dirname);

    } else {
      err("Something really nasty happened with/at the : %s -- path", path);
      /* do i exit right? */
      exit(1);
    }
  } else {
    if(S_ISDIR(s.st_mode)) {
      /* it's a dir */
      info("That directory already exists as : %s", path);
    } else {
      /* exists but is no dir */
      info("There is something else at : %s", path);
    }
  }
  /* A Recap, */
  /* We have 7 files in a directory each with one connections? */
}

bool AddConnection(Room * room1, Room * room2, Room room_list[MAX_CONNECTED_ROOMS]) {
  Room *r1 = room1;
  Room *r2 = room2;
  if (r1->num_conns == MAX_CONNECTIONS) {
    return true;
  }
  if (CanAddConnection(room1, room2)) {
    return false;
  }
  if (r1->num_conns >= MAX_CONNECTIONS || r2->num_conns >= MAX_CONNECTIONS) {
    return false;
  }
  r1->connections[r1->num_conns] = r2;
  r2->connections[r2->num_conns] = r1;
  r1->num_conns++;
  r2->num_conns++;
  return true;
}
bool CanAddConnection(Room * room1, Room * room2) {
  /* DO YOU SELF-IDENTIFY AS BEING CONNECTED TO YOURSELF ? */
  if (strcmp(room1->name,room2->name) == 0) {
    return true;
  }
  for (int i = 0; i < room1->num_conns; i++) {
    if ((strcmp((room1->connections[i])->name,room2->name) == 0) &&  room1->connections[i] != NULL) {
      return true;
    }
  }
  return false;
}

