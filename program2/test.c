#include "header.h"

int rand_range(int lower, int upper, int seed) {
  //srand(time(NULL) + seed);
  info("Between %d and %d", lower, upper);
  int temp = ((rand() % ((lower+1-lower)))+lower);
  return temp;
}

int main(int argc, char *argv[]) {
  //_test_debug(argc, argv);

  trace("Debugging is enabled. Level is %d", (int) DEBUG);

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
      /* Now lets make some files...*/
      chdir(dirname);
      for (int i = 0; i < (sizeof(ROOM_STRING)/sizeof(ROOM_STRING[i])); i++) {
        char * checkpath = getcwd(NULL, 0); /* remember to free checkpath, also how does this handle errors? idk */
        trace("Should be in the new directory = %s so pwd is %s", dirname, checkpath);
        char * file_path_to_create = (char *) malloc(strlen(checkpath) + strlen(ROOM_STRING[i]) + (sizeof("/")));
        end = file_path_to_create;
        end += sprintf(end, "%s", checkpath);
        end += sprintf(end, "%s", "/");
        end += sprintf(end, "%s", ROOM_STRING[i]);
        trace("This is the new file being created :: %s ", file_path_to_create);
        info("The number of rooms is %d ", (sizeof(ROOM_STRING)/sizeof(ROOM_STRING[i])));
        FILE *file = fopen(ROOM_STRING[i], "ab+");
        //int file = open(ROOM_STRING[i], O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
        char * room_info = (char *) malloc(strlen("ROOM NAME: ") + strlen(ROOM_STRING[i]) + 3);
        end = room_info;
        end += sprintf(end, "%s", "ROOM NAME: ");
        end += sprintf(end, "%s", ROOM_STRING[i]);
        end += sprintf(end, "%s", "\n");
        //*strncat(char *dest, const char *src, size_t n)
        trace("This is what is being written to the file (%d) :: %s ", strlen(room_info), room_info);
        int _err_fwrite = fwrite(room_info, 1, strlen(room_info)+1, file);
        trace("fwrite returned :: %d", _err_fwrite);
        /* Done writing for now */
        int _err_fclose = fclose(file);
        if (_err_fclose != 0) {
          err("A file did not close, ignoring for now :: %s ", file_path_to_create);
        }
        free(room_info);
        free(checkpath);
        free(file_path_to_create);
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
  //So at this point, basic files have one line, now build connections?
  //I need to pick a starting room. 
  //
  srand(time(NULL));
  int starting_room = (rand() % ((sizeof(ROOM_STRING)/sizeof(ROOM_STRING[0]))));
  info("Should have set a starting room :: %s  :: %d :: with 0 being the min and max being :: %d ", ROOM_STRING[starting_room], starting_room, (sizeof(ROOM_STRING)/sizeof(ROOM_STRING[0])));
  //lets open it again.
  FILE *file = fopen(ROOM_STRING[starting_room], "ab+");
  // So lets make some connections?
  int num_connections = ((rand() % ((MAX_CONNECTIONS + 1 - MIN_CONNECTIONS)) + MIN_CONNECTIONS));
  info("We have %d connections for the %s room!!!", num_connections, ROOM_STRING[starting_room]);
  
  //make a new variable to hold connections
  
  int new_rand = 0;
  char rooms[num_connections+1][2];
  memset(rooms, -1, sizeof(rooms[0][0]) * num_connections * 2);
  rooms[0][0] = starting_room;
  rooms[0][1] = ROOM_STRING[starting_room];
  for (int i = 1; i <= num_connections; i++) {
    
    //we need to call rand again to get the next room, im going to cheat, 
    
    new_rand = rand() % ((sizeof(ROOM_STRING)/sizeof(ROOM_STRING[0])));  
    
    for (int j = 0; j < num_connections; j++) {
      
      if (new_rand == rooms[j][0]) {
        //we have that room already, self identity
        //just move up one. sure
        new_rand = (new_rand + 1) % ((sizeof(ROOM_STRING)/sizeof(ROOM_STRING[0])));
        trace("already connected, new rand is %d", new_rand);
        j = 0;
      }
      else {
        //rooms[j][0] = new_rand;
        //rooms[j][1] = ROOM_STRING[new_rand];
        //trace("Adding the room, rand matched, added %s", ROOM_STRING[new_rand]);
      }
    }
    err("Random number was %d so room would be %s ", new_rand, ROOM_STRING[new_rand]);
    rooms[i][0] = new_rand;
    rooms[i][1] = ROOM_STRING[new_rand]; //DOESNT WORK LIKE I THOUGHT...hmm
    // can i just write to file? here... lets try
    char * room_to_write = (char *) malloc(strlen("CONNECTION ") + snprintf(NULL, 0,"%d",i) + strlen(": ") + strlen(ROOM_STRING[new_rand]) + strlen("\n"));
    char * end = room_to_write;
    end += sprintf(end, "%s", "CONNECTION ");
    end += sprintf(end, "%d", i);
    end += sprintf(end, "%s", ": ");
    end += sprintf(end, "%s", ROOM_STRING[new_rand]);
    end += sprintf(end, "%s", "\n");
    int _err_fwrite = fwrite(room_to_write, 1, strlen(room_to_write)+1, file);
    trace("IT WAS WRITTEN %d", i);
    free(room_to_write);
  }
  //so now rooms is ready to be written? 
  

  //write starting room jsut to test
  char * to_write = "ROOM TYPE: STARTING ROOM\n";
  int _err_fwrite = fwrite(to_write, 1, strlen(to_write)+1, file);
  //close
  int _err_fclose = fclose(file);
  trace("all done writing starting room info");


  //what now. 
  for (int i = 0; i < (sizeof(rooms)/sizeof(rooms[0])-1); i++) {
    //add connections somehow. 
    //open
    info("In a loop");
    trace("opening %s", ROOM_STRING[rooms[i+1][0]]);
    FILE * room_file = fopen(ROOM_STRING[rooms[i+1][0]], "ab+");
    //write
    char * room_to_write = (char *) malloc(strlen("CONNECTION ") + snprintf(NULL, 0,"%d",i) + strlen(": ") + strlen(ROOM_STRING[rooms[i+1][0]]) + strlen("\n"));
    char * end = room_to_write;
    end += sprintf(end, "%s", "CONNECTION ");
    end += sprintf(end, "%d", i); 
    end += sprintf(end, "%s", ": ");
    end += sprintf(end, "%s", ROOM_STRING[rooms[i+1][0]]);
    end += sprintf(end, "%s", "\n");
    //int _err_fwrite = fwrite(room_to_write, 1, strlen(room_to_write)+1, room_file);
  }
  trace("size of %d", sizeof(rooms)/sizeof(rooms[0]));
  trace("size of other %d", sizeof(rooms[0])/sizeof(rooms[0][0]));


}
