#include "cc_shell.h"
#include "handle_commands.h"
#include "arg_obj_def.h"
#include "argparser.h"
#include <stdlib.h>

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
  pid_t p = fork();
  switch (p) {
    case -1:
      fprintf(stderr, "Fork failed!\n");
      exit(EXIT_FAILURE); 
    case 0:
      /* 
       * Make sure last argument in list is NULL to satisfy requirements
       * of execvp
      */
      ao->args[ao->size] = (char *)NULL;
      execvp(full_path, ao->args);
      break;
    default:
      free(full_path);
      wait(&p);
  }
}


char *search_for_exec(char *exec_name) {
  char *full_path = (char *)malloc(PATH_MAX * sizeof(char));
  char *paths = strdup(getenv("PATH"));
  char *start_of_paths = paths;
  if (paths == NULL) {
    printf("Unable to get 'PATH' environment variable!\n");
    exit(EXIT_FAILURE);
  }
  //printf("paths received : %s\n", paths); //Debug line
  char * curr_path = strtok(paths, ":");
  /*
    Loop through every path in the 'PATH' env_var and
    search for 'exec_name' in the path. If found, print it
  */
  DIR *dirp;
  while (curr_path != NULL) {
    //printf("Currently looking at '%s'\n", curr_tok); // Debug line
    dirp = opendir(curr_path);
    if (dirp == NULL) {
      curr_path = strtok(NULL, ":");
      continue;
    }
    struct dirent *pDirent;
    // Loop through every entry in directory
    while ((pDirent = readdir(dirp)) != NULL) {
      // Found matching file, now need to check if it's executable
      if (strcmp(exec_name, pDirent->d_name) == 0) {
        sprintf(full_path, "%s/%s", curr_path,
                exec_name); // Construct full path name
        if ((access(full_path, X_OK)) == 0) {
          //printf("%s is executable!\n", full_path); // Debug line
          closedir(dirp);
          free(start_of_paths);
          return full_path;
        }
      }
    }
    curr_path = strtok(NULL, ":");
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

/*void handle_cd_command(struct arg_obj *ao) {*/
/*  if (strncmp(ao->args[1], "./", 2) == 0) {*/
/*    char *curr_pwd = getenv("PWD");*/
/*    size_t new_path_size = strlen(curr_pwd) + strlen(ao->args[1]);*/
/*    char *new_path = (char *) malloc(new_path_size);*/
/*    new_path[0] = '\0';*/
/*    strcat(new_path, curr_pwd);*/
/*    strcat(new_path, ao->args[1] + 1);*/
/*    if (chdir(new_path) != 0) {*/
/*      printf("cd: %s: No such file or directory\n", new_path);*/
/*    }*/
/*    setenv("PWD", new_path, 1);*/
/*  } else if(strncmp(ao->args[1], "../", 3) == 0) {*/
/*    char *arg_ptr = ao->args[1] + 3;*/
/*    size_t num_of_levels_up = 1;*/
/*    while (*arg_ptr != '\0' && strncmp(arg_ptr, "../", 3) == 0) {*/
/*      arg_ptr += 3;*/
/*      num_of_levels_up++;*/
/*    }*/
/*    char *curr_pwd = getenv("PWD");*/
/*    char *pwd_ptr = curr_pwd + (strlen(curr_pwd) - 1);*/
/*    for (;num_of_levels_up > 0; num_of_levels_up--) {*/
/*      while (*pwd_ptr != '/') {*/
/*        pwd_ptr--;*/
/*      }*/
/*    }*/
/*    printf("curr arg_ptr: %s\n", arg_ptr); */
/*    exit(EXIT_FAILURE);*/
/*    /*char *curr_pwd = getenv("PWD");*/
/**/
/*  } else if (chdir(ao->args[1]) != 0) {*/
/*    printf("cd: %s: No such file or directory\n", ao->args[1]);*/
/*  } else {*/
/*    setenv("PWD", ao->args[1], 1);*/
/*  }*/
/*}*/

void handle_cd_command(struct arg_obj *ao, char *command) {
  if (*ao->args[1] == '\0') {
    return;
  } else if (strncmp(ao->args[1], "./", 2) == 0) {
    char *fixed_arg = strdup(ao->args[1] + 1);
    char *new_dir = (char *)calloc(1000, sizeof(char));
    strcat(new_dir, getenv("PWD"));
    strcat(new_dir, fixed_arg);
    if (chdir(new_dir) != 0) {
      printf("cd: %s: No such file or directory\n", ao->args[1]);
      return;
    }
    setenv("PWD", new_dir, 1);
    return;
  } else if (strncmp(ao->args[1], "../", 3) == 0) {
    size_t num_of_levels = 1;
    char *arg_ptr = ao->args[1] + 3;
    while (*arg_ptr != '\0' && strncmp(arg_ptr, "../", 3) == 0) {
      arg_ptr += 3;
      num_of_levels++;
    }
    char *curr_pwd = strdup(getenv("PWD"));
    char *pwd_ptr = curr_pwd + (strlen(curr_pwd) - 1);
    /*printf("num_of_levels: %zu\n", num_of_levels);*/
    for (; num_of_levels > 0; num_of_levels--) {
      pwd_ptr--;
      while (*pwd_ptr != '/') {
        pwd_ptr--;
      }
    }
    size_t length_of_new_pwd = pwd_ptr - curr_pwd;
    /*printf("length_of_new_pwd: %zu\n", length_of_new_pwd);*/
    /*printf("curr_pwd: %s\n", curr_pwd);*/
   
    curr_pwd[length_of_new_pwd] = '\0';
    /*printf("modified curr_pwd: %s\n", curr_pwd);*/
    setenv("PWD", curr_pwd, 1);
    /*while (curr_pwd != pwd_ptr) {*/
      /*printf("%c", *curr_pwd++);*/
    /*}*/
    /*printf("\n");*/
    /*char *new_pwd = strndup(curr_pwd, i);*/
    /*setenv("PWD", new_pwd, 1);*/
    /*handle_cd_command(ao, new_command);*/
    return;
  } else if (strncmp(ao->args[1], "~", 1) == 0) {
    char *home_dir = getenv("HOME");
    setenv("PWD", home_dir, 1);
    return;
  } else if (chdir(ao->args[1]) != 0) {
    printf("cd: %s: No such file or directory\n", ao->args[1]);
    return;
  }
  setenv("PWD", ao->args[1], 1);
}

/*void handle_cat_command() {*/
/*  char temp[BUFF_LENGTH] = {0};*/
/*  strcat(temp, "cat ");*/
/*  while ((curr_tok = strtok(NULL, "")) != NULL) {*/
/*    strcat(temp, curr_tok);*/
/*  }*/
/*  system(temp);*/
/*  return;*/
/*}*/
