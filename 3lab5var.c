#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#include <wait.h>

#define NUM 1000
#define NAMESIZE 1000
#define ERR_LOG_PATH "/tmp/err.log"



void print_error_log(FILE *err_log);

void print_error_log(FILE *err_log);

void show_dir_content(char *path);

void dir_iteration(char *path, FILE *err_log, char *program_name);

int main(int argc, char *argv[]) {

    char *program_name = basename(argv[0]);

    FILE *err_log = NULL;
    if ((err_log = fopen(ERR_LOG_PATH, "w+")) == NULL) {
        fprintf(stderr, "%s: Unable create error log (%s)\n", program_name, ERR_LOG_PATH);
        return 1;
    }

    if (argc != 2) {
        save_error_to_log(err_log, program_name, "Wrong number of parameters. Usage", "./lab3.exe [pathname]");
        print_error_log(err_log);
        return 1;
    }

    //try to open directory
    DIR *dir_pointer;
    if ((dir_pointer = opendir(argv[1])) == NULL) {
        save_error_to_log(err_log, program_name, argv[1], strerror(errno));
        print_error_log(err_log);
        return 1;
    }

    //printf("\n\n\n");
    if (closedir(dir_pointer) == -1) {
        save_error_to_log(err_log, program_name, argv[1], strerror(errno));
        print_error_log(err_log);
        return 2;
    }

    ///////////////////////////////////////////////////////////////////////

    dir_iteration(argv[1],err_log,program_name);

    ///////////////////////////////////////////////////////////////////////


    print_error_log(err_log);
    return 0;
}

// Print error message to temporary file err_log.
void save_error_to_log(FILE *err_log, const char *program_name, const char *directory, const char *error_message) {
    fprintf(err_log, "%s: %s: %s\n", program_name, directory, error_message);
};

// Print all error messages to stream stderr from temporary file err_log and remove the file.
void print_error_log(FILE *err_log) {

    fseek(err_log, 0, SEEK_SET);

    int ch = fgetc(err_log);
    while (ch != EOF) {
        fputc(ch, stderr);
        ch = fgetc(err_log);
    }

    fclose(err_log);

    remove(ERR_LOG_PATH);
}

//int num_pr = 0;

void dir_iteration(char *path, FILE *err_log, char *program_name) {

    DIR *dir_pointer = NULL;
    if ((dir_pointer = opendir(path)) == NULL) {
        save_error_to_log(err_log, program_name, path, strerror(errno));
        return;
    }

    struct dirent *direction;

    while (direction = readdir(dir_pointer)) {



        if ((direction->d_type == DT_DIR) &&
            ((direction->d_name[0] != '.') || ((direction->d_name[0] != '.') && (direction->d_name[1] != '.'))))
        {
            //make new path to dir
            char dir_path[NAMESIZE];
            sprintf(dir_path, "%s/%s", path, direction->d_name);


            //startprocess
            dir_iteration(dir_path,err_log,program_name);


        } else if (direction->d_type == DT_REG) {

            char file_path[NAMESIZE];
            sprintf(file_path, "%s/%s", path, direction->d_name);


            pid_t parent = getpid();
            pid_t pid = fork();

            if (pid == -1)
            {
                // error, failed to fork()
            }
            else if (pid > 0)
            {
                int status;
                waitpid(pid, &status, 0);
            }
            else
            {

                ////////////////////////////////////////////////////////////////////////////////////////////////
                //be carefull
                // we are the child
                if (execl("/home/vlad/Desktop/aaaa.sh","./aaaa.sh",file_path,ERR_LOG_PATH,program_name) == -1)
                {
                    save_error_to_log(err_log, program_name, path, strerror(errno));
                    _exit(NULL);
                }
                _exit(EXIT_FAILURE);
            }


        }
        else
        {
            continue;
        }

    }

    if (closedir(dir_pointer) == -1) {
        save_error_to_log(err_log, program_name, path, strerror(errno));
        return;
    }
}




















