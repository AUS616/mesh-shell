#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
// commands 
int mesh_cd(char **args);
int mesh_help(char **args);
int mesh_exit(char **args);
int mesh_ls(char **args);
int mesh_mk(char **args);
int mesh_rm(char **args);
int mesh_clear(char **args);

//color
#define RED "\033[31m"
#define RESET "\033[0m"
#define BLUE "\033[34m"
#define GREEN "\033[32m"

// buildin commands
char *buildin_str[] = {
    "cd",
    "help",
    "exit",
    "ls",
    "mkdir",
    "rmdir",
    "clear"
};
// array of builtin commands
int(*buildin_func[])(char **) = {
    &mesh_cd,
    &mesh_help,
    &mesh_exit,
    &mesh_ls,
    &mesh_mk,
    &mesh_rm,
    &mesh_clear
};
//number of builtin functions
int mesh_num_builtin(){
    return sizeof(buildin_str)/sizeof(char *);
}

int mesh_cd(char **args){
    if (args[1] == NULL){
        printf("Unexpected error");
    }else{
        if (chdir(args[1]) != 0){
            perror("mesh");
        }
    }
    return 1;
}

int mesh_help(char **args){
    printf(BLUE "Austrin's mesh \n");
    printf("A simple implementation of a shell using c \n");
    printf("The buildin functions are: \n");
    printf("Available Commands:\n");
    printf("  cd     - Change the current directory.\n");
    printf("  ls     - List the contents of the current directory.\n");
    printf("  exit   - Exit the shell or program.\n");
    printf("  help   - Display this help information.\n");
    printf("  mkdir  - Create a new directory.\n");
    printf("  rmdir  - Remove an empty directory.\n");
    printf("  clear  - Clear the terminal screen.\n" RESET);

}

int mesh_exit(char **args){
    return 0;
}

int mesh_ls(char **args){
    char cwd[1024]; // Buffer to store current directory path

    // Get the current working directory
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd error");
        return -1;
        
    }

    DIR *dir = opendir(cwd);
    if (dir == NULL){
        perror("opendir error");
        return -1;
        
    }

    struct dirent *entry;
    while ((entry = readdir(dir))!=NULL){
        if ((strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)){
            printf("%s \n",entry->d_name);
        }
        
    }
    closedir(dir);
    return 1;

    
}

int mesh_mk(char **args){
    const char *dir_name = args[1];
    mode_t mode = 0755;
    if(mkdir(dir_name,mode)!=0){
        perror("error creating the directory");
        return 1;

    }
    return 1;

}

int mesh_rm(char **args){
    const char *dir_name = args[1];
    if(rmdir(dir_name)!=0){
        perror("error in removing the directory");
        return 1;
    }
    return 1;
}

int mesh_clear(char **args){
    system("clear");
    return 1;

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
            tokens = realloc(tokens,bufsize * sizeof(char*));
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

void mesh_loop(void){
    char *line;
    char **args;
    int status;
    char cwd[1024];
    do {

        if (getcwd(cwd,sizeof(cwd))==NULL){
            perror("mesh: getcwd error");
            exit(EXIT_FAILURE);
        }
        printf(RED "[%s]> " RESET,cwd);
        line = mesh_readline();
        args = mesh_line_split(line);
        status = mesh_execute(args);
    
        free(line);
        free(args);
      } while (status);
}
int main(int argc, char **argv)
{
    printf(GREEN "Welcome to the Shell!\n");
    printf("Type 'help' to see available commands.\n" RESET);
  mesh_loop();

  

  return EXIT_SUCCESS;
}
