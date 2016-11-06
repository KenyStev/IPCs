
sig_atomic_t child_exit_status;
struct sigaction sigchld_action;
void clean_up_child_process (int signal_number)
{
  /* Clean up the child process.  */
  int status;
  wait (&status);
  /* Store its exit status in a global variable.  */
  child_exit_status = status;
}

void waitAsync(){
  /* Handle SIGCHLD by calling clean_up_child_process.  */
  memset (&sigchld_action, 0, sizeof (sigchld_action));
  sigchld_action.sa_handler = &clean_up_child_process;
  sigaction (SIGCHLD, &sigchld_action, NULL);
}

void createNode(char * val,pid_t *rt){
  pid_t pid;
  pid = fork();
  if(pid == 0){
    printf("Yo soy hijo\n");
    char* char_val[3];
    char_val[0]="./single_server";
    char_val[1]=val;
    char_val[2]=0;
    execvp("./single_server", char_val);
  }else{
    printf("Yo soy el padre\n");
    *rt = pid;
  }
}
