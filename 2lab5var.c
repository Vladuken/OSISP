#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>

//#define NUM 600
#define NAMESIZE 600
#define ERR_LOG_PATH "/tmp/err.log"

typedef struct {
    int countfiles;
    char *name;
    long int sumsize;
    long int maxsize;
    char *max_file_name;
} direction_info;

void print_error_log(FILE *err_log);

void print_error_log(FILE *err_log);

void show_dir_content(char *path);

long get_file_info(char *filepath, FILE *err_log, char *program_name, char *path,direction_info *buff);

void get_dir_info(char *path, FILE *err_log,FILE *output, char *program_name);
void print_direction(direction_info *buff, FILE *output);

    int main(int argc, char *argv[]) {

    char *program_name = basename(argv[0]);

    FILE *err_log = NULL;
    if ((err_log = fopen(ERR_LOG_PATH, "w+")) == NULL) {
        fprintf(stderr, "%s: Unable create error log (%s)\n", program_name, ERR_LOG_PATH);
        return 1;
    }

    if (argc != 3) {
        save_error_to_log(err_log, program_name, "Wrong number of parameters. Usage", "./lab2.exe [pathname]");
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
    if (closedir(dir_pointer) == -1) {
        save_error_to_log(err_log, program_name, argv[1], strerror(errno));
        print_error_log(err_log);
        return 2;
    }

    //direction_info *direction_info_array = (direction_info *) malloc(sizeof(direction_info_array));

    FILE *output = NULL;
    if ((output = fopen(argv[2], "w+")) == NULL) {
        fprintf(stderr, "%s: Unable create output file (%s)\n", program_name, argv[2]);
        return 3;
    }
    //show_dir_content(argv[1]);
    get_dir_info(argv[1], err_log,output, program_name);

    if (fclose(output) == EOF) {
        save_error_to_log(err_log, program_name, argv[2], strerror(errno));
    }

    print_error_log(err_log);
    return 0;
}

void get_dir_info(char *path, FILE *err_log,FILE *output, char *program_name) {
    struct dirent *direction;

    direction_info *buff = (direction_info *) malloc(sizeof(direction_info));
    buff->countfiles = 0;
    buff->sumsize = 0;
    buff->maxsize = 0;
    buff->max_file_name = NULL;
    buff->name = (char*) malloc(sizeof(char) * (strlen(path) + 1));
    sprintf(buff->name, "%s", path);

    DIR *dir_pointer = NULL;

    if ((dir_pointer = opendir(path)) == NULL) {
        save_error_to_log(err_log, program_name, path, strerror(errno));
        return;
    }

    while (direction = readdir(dir_pointer))
    {

        if ((strcmp(direction->d_name,".") == 0) || (strcmp(direction->d_name,"..") == 0))
        {
            continue;
        }

        char *dirpath = (char*) malloc(sizeof(char)*(strlen(path)+strlen(direction->d_name) + 2));
        sprintf(dirpath, "%s/%s", path, direction->d_name);

        //if its directory
        if (direction->d_type == DT_DIR)
        {
            get_dir_info(dirpath,err_log,output,program_name);
        }
        else if(direction->d_type == DT_REG)
        {
            int buffsize = get_file_info(dirpath,err_log,program_name,path,buff);
            if (buffsize >= buff->maxsize) {
                buff->maxsize = buffsize;
                //add max name
                if (buff->max_file_name != NULL)
                {
                    free(buff->max_file_name);
                }
                buff->max_file_name = (char*) malloc(sizeof(char)*(strlen(dirpath) + 1));
                sprintf(buff->max_file_name, "%s", dirpath);

            }
        }
        else
        {
            continue;
        }

        free(dirpath);


    }

    if (closedir(dir_pointer) == -1) {
        save_error_to_log(err_log, program_name, path, strerror(errno));
        return;
    }

    //if null give it empty string to print
    if (buff->max_file_name == NULL)
    {
        buff->max_file_name = (char*) malloc(sizeof(char));
        sprintf(buff->max_file_name, "");
    }

    print_direction(buff,output);
    free(buff->name);
    if (buff->max_file_name != NULL)
    {
        free(buff->max_file_name);
    }
    free(buff);
}


void print_direction(direction_info *buff, FILE *output)
{

    printf("%s %d %ld %s\n",
           buff->name, buff->countfiles, buff->sumsize,
           buff->max_file_name);

    fprintf(output, "%s %d %ld %s\n",
            buff->name, buff->countfiles, buff->sumsize,
            buff->max_file_name);

}


ino_t *visited_inodes = NULL;
int visited_inode_len = 0;

//returns size of file in bytes
long get_file_info(char *filepath, FILE *err_log, char *program_name, char *path,direction_info *buff) {
    struct stat *filestat = malloc(sizeof(struct stat));

    if (stat(filepath, filestat) == -1) {
        save_error_to_log(err_log, program_name, path, strerror(errno));
    }

    int is_in = 0;

    //added check for inodes
    if (filestat->st_nlink > 1)
    {
        // Array contains visited inodes numbers.
        for (int i = 0; i < visited_inode_len; i++)
        {
            if (visited_inodes[i] == filestat->st_ino)
            {
                is_in = 1;
            }
        }


        if (is_in)
        {
            return 0;
        }
        else
        {
            visited_inodes = (ino_t*) realloc(visited_inodes,sizeof(ino_t) * (visited_inode_len + 1));
            visited_inodes[visited_inode_len] = filestat->st_ino;
            visited_inode_len++;

            (buff->sumsize) += filestat->st_size;
            (buff->countfiles)++;

            return 0;
        }

    }

    (buff->sumsize) += filestat->st_size;
    (buff->countfiles)++;

    return filestat->st_size;
}

// Print error message to temporary file err_log.
void save_error_to_log(FILE *err_log, const char *program_name, const char *directory, const char *error_message) {
    fprintf(err_log, "%s: %s: %s\n", program_name, directory, error_message);
}

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

