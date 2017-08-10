#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdarg.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<fcntl.h>
#include<signal.h>
#include<sys/stat.h>
#include<errno.h>

typedef int bool;
#define true 1
#define false 0
//comment this out, to remove trace... set to anything to get them back
//#define DEBUG 1

#if defined(DEBUG) && DEBUG > 0
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define trace(fmt, args...) fprintf(stderr, ANSI_COLOR_RED "DEBUG %s:%d:%s():PID %d: "  ANSI_COLOR_RESET fmt "\n", __FILE__, __LINE__, __func__, getpid(), ##args), fflush(stdout)
#else
#define trace(fmt, args...) /* Don't do anything in release builds */
#endif


#define ERROR 1
#define MAX_ARGS 512
#define ARG_DELIM " \n"
#define MARG_DELIM "  \n"
#define FILE_IN_CHAR "<"
#define FILE_OUT_CHAR ">"
#define RUN_BG "&"
#define COMMENT "#"
#define EXIT "exit"
#define CD "cd"
#define STATUS "status"
#define DBL_DOLLAR "$$"
#define NEWLINE '\n'
#define CHAR_NULL '\0'

int bg_possible = 1;
pid_t parent;

//forward declarations
void err_handle(int err);
int mprintf(const char *format, ...);
void showstatus(int status);

//helper functions
void err_handle(int err) {
  pid_t pid = 0;
  int pid_flag = 1;
  int status;
  if (err == 2)
  {
    trace("Caught a live SIGINT, mounting and showing off to elderly friends, :: %s ::", strerror(errno));
    //wait for any children. 
    while((pid = waitpid(-1, NULL, 0)))
    {
      if (pid == -1) 
      {
        trace("Best guess is you are a parent (%i), and not allowed in the playhouse :: %d ::", pid, err);
        mprintf("PARENT TERMINATED BY SIGNAL %d\n", err);
        exit(err);
      }
      trace("At the airport, with SIGINT, waiting on the children...");
      if (errno == ECHILD) 
      {
        trace("you are a childless wench (%i). exit now :: %s ::", pid, strerror(errno));
        signal(SIGINT, SIG_DFL);
        break;
      }
    }
    if (pid != parent)
    {
      mprintf("CHILD (%d) TERMINATED BY SIGNAL %i\n", pid, err);
      trace("You arent my parent (%i) you are a child (%i)", parent, pid);
      return;
    }
    trace("LIVE STRONG, silently");
    return;
  }
  else if (err == 17) 
  {
    //ima term child...awww
    trace("CHILD (%i parent: %i, getpid: %i) TERMINATED :: %s ::", pid, parent, getpid(), strerror(errno));
    while (pid_flag != 0 && (((pid = waitpid(-1, &status, WNOHANG))) != -1)) //& (pid != 0))//ahh the error here is grrrr
    {
      if (pid == 0)
      {
        pid_flag = 0;
      }
      else
      {
      mprintf("Child process %i has finished!\n", pid);
      //dertywork
      }
    }
    fflush(stdout);
    return;
  }
  else if (err == 20) 
  {
    //ima sigstp. 
    trace("LANDED A Z STOP... flip the mode please, :: %s ::", strerror(errno));
    if (bg_possible) 
    {
      mprintf("\nEntering foreground-only mode\n");
      bg_possible = 0;
      return;
    }
    else if (!bg_possible)
    {
      mprintf("\nExiting foreground-only mode\n");
      bg_possible = 1;
      return;
    }
    else 
    {
      trace(" Returning to civilian life, battered, bruised, but BADASS... o and super confused... errno maybe :: %s ::", strerror(errno));
      return;
    }
  }
  return;
}

int mprintf(const char *format, ...) 
{
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  //so like i still dont trust this. 
  fflush(stdout);
  return 0;
}

void showstatus(int status) 
{
  trace("BORING, saving my fingers.");
  //if you are a parent, just wait for your children to die...
  if (WIFEXITED(status)) 
  {
    mprintf("exiting %i\n", WEXITSTATUS(status));
  } 
  else 
  {
    mprintf("sig caught %i\n", status);
  }
}

int main(int argc, char*argv[]) 
{
  char * input = calloc((MAX_ARGS+1), sizeof(char));
  char * token;
  char * com_tok;
  char * pos;
  char * input_filename = NULL;
  char * output_filename = NULL;
  char * margv[MAX_ARGS+1];
  int bg; 
  int arg_count = 0;
  int status = 0;
  int pid = -1; 
  struct sigaction act; //fg
  FILE* file;
  int exit_flag = 0;
  int argcount;
  int j = 0; //because i is overrated.
  
  trace("Set up signal handler act");
  act.sa_handler = err_handle; //err_handle;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;

  parent = getpid();

  sigaction(SIGTSTP, &act, NULL);
  sigaction(SIGINT, &act, NULL);
  sigaction(SIGCHLD, &act, NULL);
  
  trace("Starting main loop... input first");
  trace("HELLO, THIS COULD BE A NEWBIE PID: %d", getpid());
  while(!exit_flag) 
  {
    trace("Entering another input loop...");
    bg = 0;
    arg_count = 0; //counting arguments
    trace("outputting two vertically aligned dots");
    PROMPT: mprintf(": ");//ready for next line, if there is 
       if (fgets(input, MAX_ARGS+1, stdin) == NULL) 
         //if (!(getline(&input, &size, stdin)))
       {
         trace("READ:: fgets has some stoopid error, like NOT too long of a line...");
         if (!feof(stdin)) {
           goto PROMPT; //this is to deal with some derty bits of my program, fgets leaves a \n behind? idk for sure.
         }
         return 0; //too long of a line
       }
       arg_count = 0;
       //first lets comment toke. and some trailing newline eating
       if (strncmp((input==NULL ? "":input), COMMENT, 1) == 0)
       {
         if ((pos = strchr(input, NEWLINE)) != NULL)
         {
           *pos = CHAR_NULL;
         }
        trace("whole line is a comment. :: %s ::", input); 
        com_tok = strdup(input);
        goto PROMPT;
        //memset(input, 0, (strlen(input)-1)*sizeof(char));
       }
       else
       {
         trace("doesnt start with a comment");
         //search for the first comment using strtok
         token = strtok(input, COMMENT);
         com_tok = strtok(NULL, "");
       //token = strtok((input==NULL ? "":input), COMMENT);
       //strip newling
       if ((pos=strchr(token, NEWLINE)) != NULL)
       {
         *pos = CHAR_NULL;
       }
       trace("init token, :: %s ::", token);
       //com_tok = strtok(NULL, "");
       //}
       //now all that is left is 
       if (com_tok != NULL && (pos = strchr(com_tok, NEWLINE)) != NULL)
       {
         *pos = CHAR_NULL;
       }
       trace("init com_tok, :: %s ::", com_tok);
       //trace("input is now :: %s ::", (input == NULL ? "NULL":input));
       //token is already to go. 
       //if (token != NULL) 
       //{
       //input = strdup(token);
       //token = strtok(input, MARG_DELIM);
       //}
       //else 
       //{
       //  input = NULL;
       input = strdup(token);
       
       
       token = strtok(input, MARG_DELIM);
       trace("READ:: %s  ---", token);
       }
       while (token != NULL) 
       {
         if (strcmp(token, FILE_IN_CHAR) == 0) 
         {
           //the next argument is the file to read in?
           token = strtok(NULL, ARG_DELIM);
           trace("READ:: %s  ---", token);
           input_filename = strdup(token);
           token = strtok(NULL, ARG_DELIM);
           trace("READ:: %s  ---", token);
         }
         else if (strcmp(token, FILE_OUT_CHAR) == 0) 
         {
           //the next argument is the file to output to?
           token = strtok(NULL, ARG_DELIM);
           trace("READ:: %s  ---", token);
           output_filename = strdup(token);
           token = strtok(NULL, ARG_DELIM);
           trace("READ:: %s  ---", token);
         }
         else if (strcmp(token, RUN_BG) == 0) //&& bg_possible == true) 
         {
           trace("Issued Background...");
           if (bg_possible != 0)
           {
             //run in background
             trace("Run in the background is set, exec next in bg...");
             bg = 1;
             //continue or return or break or goto. 
             break;
           }
           mprintf("background not possible\n");
           //fflush(stdout);
           break;
         }
         else 
         {
           trace("READ:: background is possible..."); 
           margv[arg_count] = strdup(token);
           token = strtok(NULL, ARG_DELIM);
           trace("READ:: %s  ---", token);
           arg_count++;
           trace("arg_count = %d", arg_count);
         }
       }//end special args
       //set last argument in the list to null
       margv[arg_count] = NULL;
       //builtins command checking?
       if (margv[0] == NULL)
       {
         trace("EMPTY line... easy peasy");
       }
       //find me a comment char...
       //for (j = 0; j < arg_count; j++)
       //{
       //for (k = 0; k < strlen(margv[j]); k++)
       //{
       //if (strcmp((margv[j][k]), COMMENT) == 0)
       //{
       //boom comment, OKAY SO NOW, lets read in the whole line?????
       //trace("COMMENT DETECTED: rest of line is :: %s ::", input);
       // so like do i need to eat the line? or make it all one argument.
       // lets do both. 

       //}
       // }
       //}
       else if (strcmp(margv[0], EXIT) == 0)  
       {   
         trace("EXIT requested, PID = %d, GETPID = %d ::", pid, getpid()); 
         exit(0);
         trace("After an exit, i guess i am a parent. clean the loop up");
         exit_flag = 1;
       }
       else if (strcmp(margv[0], DBL_DOLLAR) == 0)
       {
         trace("READ run in background... this is not working with echo");
         mprintf("%d\n", (int)getpid());
       }
       else if (strcmp(margv[0], STATUS) == 0) 
       {
         //you asked for my PID? or my childs. grr. 
         //shouldnt matter tho. 
         trace("Entered status, PID is %d\n, and getpid is %d", pid, getpid());
         showstatus(status);
       }
       else if (strcmp(margv[0], CD) == 0) 
       {
         trace("READ:: cd (%s) command...", margv[0]);
         if (margv[1] == NULL) 
         {
           trace("...cd had no margvuments, going HOME ::  %s  ::", getenv("HOME"));
           chdir(getenv("HOME"));
         } 
         else 
         {
           trace("...cd had MARGVUMENTS :: %s  ::", margv[1]);
           chdir(margv[1]);
         }
       }
       else 
       {
         trace("IM FORKING... watchout");
         fflush(stdout);
         pid = fork();//give me a pid for a child. 
         fflush(stdout);
         if (pid == 0)
         { 
           trace("I should be a child");
           if (!bg) 
           {
             //foreground child...
             trace("not bg, should i remove sigstp handler");
             //act.sa_handler = SIG_DFL;
             //sigemptyset(&act.sa_mask);
             //act.sa_flags = 0;
             //am i replacing CTRL-C... nah.... 
             //sigaction(SIGINT, &act, NULL);
             //signal(SIGTSTP, SIG_DFL);
           }
           //to read
           if (input_filename != NULL) 
           {
             trace("input_filename OPEN :: %s ::", input_filename);
             file = fopen(input_filename, "r");
             if (!file);
             {
               trace("ERR, input_filename ::  %s  :: from fd :: %d ::", input_filename, file);
               mprintf("smallsh:: cannot open %d for input\n", input_filename);
               _Exit(ERROR);//now hear this, ima child. 
             }
             if (dup2(fileno(file), 0) == -1)
             {
               trace("Dup2 failed... what to do what to do... fileno(%d)", file);
               perror("file descriptor dup error");
               _Exit(ERROR);
             }
             fclose(file);
             trace("File is now closed...");
           }
           else if (bg) 
           {
             trace("This is the background now...");
             file = fopen("/dev/null", "r");
             if (!file)
             {
               trace("ERR, /dev/null did not fopen...");
               perror("open failed");
               _Exit(ERROR);
             }
             if (dup2(fileno(file), 0) == -1)
             {
               trace("Dup2 failed... fileno(%d)", file);
               perror("file descriptor dup error");
               _Exit(ERROR);
             }
             fclose(file);
             trace("File is now closed TWO...");
           }
           else if (output_filename != NULL) 
           {
             trace("Output_filename OPEN ::  %s  ::", output_filename);
             file = fopen(output_filename, "w");
             if (!file)
             {
               trace("ERR, output_filename ::  %d  :: outpute filename is :: %s ::", file, output_filename);
               mprintf("smallsh:: cannot open %s for output\n", output_filename);
               _Exit(ERROR);
             }
             if (dup2(fileno(file), 1) == -1)
             {
               trace("Dup2 failed... fileno(%d)", file);
               perror("file descriptor dup error");
               _Exit(ERROR);
             }
             fclose(file);
             trace("File is now closed THREE...");
           }
           trace("quick test on margv[0] for future double dollars");
           //tryin to deal with $$ here of all places is not good...but
           if (margv[0] != NULL)
           {
             trace("its not null, testing margv[1] now, well we loop first.. argcount is :: %d ::", arg_count);
             for (j = 1; j < arg_count; j++)
             {
               trace("we have to loop to search the argv, iter is :: %d ::", j);
               if (margv[j] != NULL)
               {
                 trace("AND its not null");
                 if (strcmp(margv[j], DBL_DOLLAR) == 0)
                 {
                   if (pid == 0)
                   {
                     trace("QUICK we are a parent and need to replace the margv[%d] = :: %s :: with ::  %d  :: from getpid...", j,  margv[j], getpid());
                     //can we assume that the pid will be shorted than MAX_ARGS...
                     //maybe
                     sprintf(margv[j], "%ld", (long) getpid());
                   }
                   else 
                   {
                     trace("Ehh just a child slow rolling by, mucking everything up");
                     sprintf(margv[j], "%d", pid);
                   }
                   trace("...oo i hope that worked margv[1] is now ::  %s  ::", margv[j]);
                 }
               }
               else
               {
                 trace("A null was found whilst searching for DBL_DOLLAR a 2nd time...");
                 break;//i wonder what works best
                 j = arg_count;
               }
             }
           }
           trace("Exec start for ::  %s  ::", margv[0]);
           fflush(stdout);
           if (execvp(margv[0], margv)) //&& (mprintf("THIS IS WEIRD") || 1)) 
           { //ya... so..., like anything executes.
             trace("EXEC, failed on ::  %s  ::", margv[0]);
             mprintf("smallsh:: command %s not found\n", margv[0]);
             _Exit(ERROR);
           }
           trace("done with exec... dealing with self identity issues");
         }
         else if (pid < 0) 
         {
           trace("failure to fork means you will die of starvation, fyi");
           perror("failed forking");
           status = 1;
           break;
           trace("if you are reading this, jim is not dead");
         }
         else {
           trace("...first personal question...");
           if (!bg) 
           {
             trace("are you running in bg mode, YES :: %d ::", bg);
             do 
             {
               //wait for fg
               #ifdef DEBUG
               trace("...always waiting for something...");
               sleep(1);
               #endif
               waitpid(pid, &status, 0); //NO-WNOHANG, purrent
               fflush(stdout);
             }
             while (!WIFEXITED(status)); //&& !WIFSIGNALED(status));
             trace("Done waiting...");
           }
           else 
           {
             //block sigtstp
             //signal(SIGTSTP, SIG_IGN);
             trace("printing background process pid and moving on...");
             mprintf("background pid is %i\n", pid);
             //continue;
           }
         }
       }
       //arg_count--;//this did not work. 
       //int argcount = 0;
       trace("FINALLY - cleanup...");
       for (argcount = 0; argcount < arg_count; argcount++) 
       {
         trace("could have freed me, trapped margv at argcount of (%d) and is :: %s ::", argcount, margv[argcount]);
         margv[argcount] = NULL;
         //free(margv[args]);
       }
       trace("Setting input and output filenames back to NULL...");
       input_filename = NULL;
       output_filename = NULL;
       //check for finished background processes
       trace("So pid is :: %d :: and will soon be reset, cuz we waitin again", pid);
       pid = waitpid(-1, &status, WNOHANG);
       trace("Your self identity has changed to PID :: %d ::", pid);
       while (pid > 0) 
       {
         trace("...bout to start waiting again...");
         mprintf("background pid %i is done\n", pid);
         //showstatus(status);
         if (!bg_possible)
         {
           pid = waitpid(-1, &status, 0);
           showstatus(status);
         }
         else 
         {
           pid = waitpid(-1, &status, WNOHANG);
           showstatus(status);
         }

         trace("DONE WAITING, new pid is :: %d ::", pid);
       }
       trace("Done waiting for stuff I may be responsible for...");
       //signal(SIGINT, SIG_IGN);
  }
  //end args parse while(true)...
  trace("the main while is now over, either you are not the parent, or the big sleep is near!");
  return 0;
}
