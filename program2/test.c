#include "header.h"

char* concat(const char *s1, const char *s2)
{
  const size_t len1 = strlen(s1);
  const size_t len2 = strlen(s2);
  char *result = malloc(len1+len2+1);//+1 for the zero-terminator
  //in real code you would check for errors in malloc here
  memcpy(result, s1, len1);
  memcpy(result+len1, s2, len2+1);//+1 to copy the null-terminator
  return result;
}


int main(int argc, char **args) {
  //_test_debug(argc, args);

  trace("Debugging is enabled. Level is %d", (int) DEBUG);

  /* lets test some dir making */
  struct stat s;
  char * path = "./temp";
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

}
