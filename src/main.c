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

#define mesh_buffer_size 1024
char *mesh_readline(void){
    int bufsize = mesh_buffer_size;
    int pos = 0;
    char* buffer = malloc(sizeof(char) * bufsize);
    int c;
    if (!buffer){
        fprintf(stderr, "mesh: allocation error\n");
        exit(EXIT_FAILURE);

    }

    while(1){
        c = getchar();
        if (c == EOF || c == '\n') {
            buffer[pos] = '\0';
            return buffer;
        }else {
            buffer[pos] = c;
        }
        pos++;

        if(pos>=bufsize){
            bufsize+=mesh_buffer_size;
            buffer = realloc(buffer,bufsize);
            if (!buffer){
                fprintf(stderr, "mesh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

    }

}

#define mesh_tok_bufsize 64
#define mesh_tok_delim " \t\r\n\a"

char **mesh_line_split(char *line){
    int bufsize = mesh_tok_bufsize,pos=0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "mesh: allocation error\n");
        exit(EXIT_FAILURE);
    }
    token = strtok(line,mesh_tok_delim);
    while(token != NULL){
        tokens[pos] = token;
        pos++;

        if(pos>= bufsize){
            bufsize+= mesh_buffer_size;
            tokens = realoc(tokens,bufsize * sizeof(char*));
            if(!tokens){
                fprintf(stderr, "mesh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL,mesh_tok_delim);
    }
    tokens[pos] = NULL;
    return tokens;


}
