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
  /* testing the enum stuffs */
  //trace("enum orange as a string: %s\n",ROOM_STRING[orange]);
  return 0;
}

