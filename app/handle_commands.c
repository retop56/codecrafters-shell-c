#include "cc_shell.h"
#include "handle_commands.h"
#include "arg_obj_def.h"
#include "argparser.h"
extern char * curr_tok;

void handle_exit_command() { exit(0); }

void handle_echo_command(struct arg_obj *ao) {
  if (ao->args[1] != NULL) {
    printf("%s", ao->args[1]);
  }
  for (size_t i = 2; i < ao->size; i++) {
    printf(" %s", ao->args[i]);
  }
  printf("\n");
}

void handle_type_command(struct arg_obj *ao) {
  if (strcmp(ao->args[1], "echo") == 0 || strcmp(ao->args[1], "exit") == 0 ||
      strcmp(ao->args[1], "type") == 0 || strcmp(ao->args[1], "pwd") == 0) {
    printf("%s is a shell builtin\n", ao->args[1]);
  } else {
    char *exec_name = (char *)malloc(256 * sizeof(char));
    strcpy(exec_name, ao->args[1]);
    char *full_path = search_for_exec(exec_name);
    if (full_path == NULL) {
      printf("%s: not found\n", exec_name);
      free(full_path);
      free(exec_name);
      return;
    }
    printf("%s is %s\n", exec_name, full_path);
    free(full_path);
    free(exec_name);
  }
}

void handle_program_execution(struct arg_obj * ao) {
  char * full_path = search_for_exec(ao->args[0]);
  if (full_path == NULL) {
    printf("%s: command not found\n", ao->args[0]);
    return;
  }
  /*printf("full_path: %s\n", full_path);*/
  /*ao->args[0] = full_path;*/
  /*// Calculate length of full_path w/ args (and null at the end)*/
  /*size_t len_of_full_path = strlen(full_path);*/
  /*for (size_t i = 1; i < ao->size; i++) {*/
  /*  len_of_full_path += 1; // Add one for space between args*/
  /*  len_of_full_path += strlen(ao->args[i]);*/
  /*}*/
  /*len_of_full_path += 1; // Add one at end for null*/
  /*char * real_full_path = (char *)malloc(len_of_full_path * sizeof(char));*/
  /*real_full_path[0] = '\0';*/
  /*strcat(real_full_path, full_path);*/
  /*for(size_t i = 1; i < ao->size; i++) {*/
  /*  strcat(real_full_path, " ");*/
  /*  strcat(real_full_path, ao->args[i]);*/
  /*}*/
  pid_t p = fork();
  switch (p) {
    case -1:
      fprintf(stderr, "Fork failed!\n");
      exit(EXIT_FAILURE); 
    case 0:
      // Write code for executing file
      // Make sure last argument in list is NULL
      ao->args[ao->size] = (char *)NULL;
      execvp(full_path, ao->args);
      break;
    default:
      free(full_path);
      /*free(real_full_path);*/
      wait(&p);
  }
}

/*void handle_program_execution(char *input) {*/
/*  char *prog_args[100];*/
/*  size_t num_of_args = 0;*/
/*  char *curr_arg = strtok(input, " ");*/
/*  while (curr_arg != NULL) {*/
/*    prog_args[num_of_args++] = strdup(curr_arg);*/
/*    curr_arg = strtok(NULL, " ");*/
/*  }*/
/*  char *full_path = search_for_exec(prog_args[0]);*/
/*  if (full_path == NULL) {*/
/*    printf("%s: command not found\n", input);*/
/*    return;*/
/*  }*/
/*  // Calculate length of full_path w/ args (and null at the end)*/
/*  size_t len_of_full_path = strlen(full_path);*/
/*  for (size_t i = 1; i < num_of_args; i++) {*/
/*    len_of_full_path += 1; // Add one for space in between args*/
/*    len_of_full_path += strlen(prog_args[i]);*/
/*  }*/
/*  len_of_full_path += 1; // Add one at end for null;*/
/*  char *real_full_path = (char *)malloc(len_of_full_path * sizeof(char));*/
/*  real_full_path[0] = '\0';*/
/*  strcat(real_full_path, full_path);*/
/*  for (size_t i = 1; i < num_of_args; i++) {*/
/*    strcat(real_full_path, " ");*/
/*    strcat(real_full_path, prog_args[i]);*/
/*  }*/
/*  /*prog_args[0] = full_path;*/
/*  prog_args[num_of_args] = NULL;*/
/*  pid_t p;*/
/*  /*fflush(stdout);*/
/*  p = fork();*/
/*  switch (p) {*/
/*  case -1:*/
/*    printf("Fork failed!\n");*/
/*    exit(EXIT_FAILURE);*/
/*  case 0:*/
/*    execvp(prog_args[0], prog_args);*/
/*    printf("Unreachable");*/
/*    break;*/
/*  default:*/
/*    free(full_path);*/
/*    free(real_full_path);*/
/*    wait(&p);*/
/*  }*/
/*}*/

char *search_for_exec(char *exec_name) {
  char *full_path = (char *)malloc(PATH_MAX * sizeof(char));
  char *paths = strdup(getenv("PATH"));
  char *start_of_paths = paths;
  if (paths == NULL) {
    printf("Unable to get 'PATH' environment variable!\n");
    exit(EXIT_FAILURE);
  }
  /*printf("paths received : %s\n", paths); //Debug line*/
  curr_tok = strtok(paths, ":");
  // Loop through every path in the 'PATH' env_var and
  // search for 'exec_name' in the path. If found, print it
  DIR *dirp;
  while (curr_tok != NULL) {
    /*printf("Currently looking at '%s'\n", curr_tok);*/ // Debug line
    dirp = opendir(curr_tok);
    if (dirp == NULL) {
      curr_tok = strtok(NULL, ":");
      continue;
    }
    struct dirent *pDirent;
    // Loop through every entry in directory
    while ((pDirent = readdir(dirp)) != NULL) {
      // Found matching file, now need to check if it's executable
      if (strcmp(exec_name, pDirent->d_name) == 0) {
        sprintf(full_path, "%s/%s", curr_tok,
                exec_name); // Construct full path name
        if ((access(full_path, X_OK)) == 0) {
          /*printf("%s is executable!\n", full_path);*/ // Debug line
          closedir(dirp);
          free(start_of_paths);
          return full_path;
        }
      }
    }
    curr_tok = strtok(NULL, ":");
  }
  free(full_path);
  free(start_of_paths);
  closedir(dirp);
  return NULL;
}

void handle_pwd_command() {
  const char *pwd_str = getenv("PWD");
  if (pwd_str == NULL) {
    fprintf(stderr, "Unable to get pwd!\n");
    return;
  }
  printf("%s\n", pwd_str);
}

void handle_cd_command(char *command) {
  if (*command == '\0') {
    return;
  } else if (strncmp(command, "./", 2) == 0) {
    char *fixed_arg = strdup(&command[1]);
    char *new_dir = (char *)calloc(1000, sizeof(char));
    strcat(new_dir, getenv("PWD"));
    strcat(new_dir, fixed_arg);
    if (chdir(new_dir) != 0) {
      printf("cd: %s: No such file or directory\n", command);
      return;
    }
    setenv("PWD", new_dir, 1);
    return;
  } else if (strncmp(command, "../", 3) == 0) {
    char *new_command = strdup(command + 3);
    char *curr_pwd = getenv("PWD");
    int i = strlen(curr_pwd);
    for (; i > 0; i--) {
      if (curr_pwd[i] == '/') {
        break;
      }
    }
    char *new_pwd = strndup(curr_pwd, i);
    setenv("PWD", new_pwd, 1);
    handle_cd_command(new_command);
    return;
  } else if (strncmp(command, "~", 1) == 0) {
    char *home_dir = getenv("HOME");
    setenv("PWD", home_dir, 1);
    return;
  } else if (chdir(command) != 0) {
    printf("cd: %s: No such file or directory\n", command);
    return;
  }
  setenv("PWD", command, 1);
}

void handle_cat_command() {
  char temp[BUFF_LENGTH] = {0};
  strcat(temp, "cat ");
  while ((curr_tok = strtok(NULL, "")) != NULL) {
    strcat(temp, curr_tok);
  }
  system(temp);
  return;
}
