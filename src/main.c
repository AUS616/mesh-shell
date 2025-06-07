#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
// commands 
int mesh_cd(char **args);
int mesh_help(char **args);
int mesh_exit(char **args);

// buildin commands
char *buildin_str[] = {
    "cd",
    "help",
    "exit"
};
// array of builtin commands
int(*buildin_func[])(char **) = {
    &mesh_cd,
    &mesh_help,
    &mesh_exit
};
//number of builtin functions
int mesh_num_builtin(){
    return sizeof(buildin_str)/sizeof(char *);
}

int mesh_cd(char **args){
    if (args[1] == NULL){
        print("Unexpected error");
    }else{
        if (chdir(args[1]) != 0){
            perror("mesh");
        }
    }
    return 1;
}

int mesh_help(char **args){
    printf("Austrin's mesh \n");
    printf("A simple implementation of a shell using c \n");
    printf("The buildin functions are: \n");

}

int mesh_exit(char **args){
    return 0;
}

int mesh_launch(char **args){
    pid_t pid;
    int status;
    pid = fork();
    if (pid == 0){
        // child process
        if (execvp(args[0],args) == -1){
            perror("mesh");
        }
        exit(EXIT_FAILURE);
    }else if(pid>0 ){
        // error creating the child process
        perror("mesh");


    }else{
        // parent process
        do{
            waitpid(pid,&status,WUNTRACED);

        }while(!WIFEXITED(status) && !WIFSIGNALED(status));


    }
    return 1;

}

int mesh_execute(char **args){
    if (args[0] == NULL){
        return 1;
    }
    for(int i = 0;i<mesh_num_builtin;i++){
        if (strcmp(args[0],buildin_str[i]) == 0){
            return (*buildin_func[i])(args);
        }
    }
    return mesh_launch(args);
}
