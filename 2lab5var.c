#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>

#define NUM 1000
#define NAMESIZE 1000
#define ERR_LOG_PATH "/tmp/err.log"

typedef struct {
    int countfiles;
    char name[NAMESIZE];
    long int sumsize;
    long int maxsize;
    char max_file_name[NAMESIZE];
} direction_info;

void print_error_log(FILE *err_log);

void print_error_log(FILE *err_log);

void show_dir_content(char *path);

long get_file_info(char *filepath, FILE *err_log, char *program_name, char *path);

direction_info
get_dir_info(char *path, int index, direction_info *direction_info_array, FILE *err_log, char *program_name);


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
    printf("\n\n\n");

    if (closedir(dir_pointer) == -1) {
        save_error_to_log(err_log, program_name, argv[1], strerror(errno));
        print_error_log(err_log);
        return 2;
    }


    int index = 0;
    direction_info direction_info_array[NUM];
    direction_info_array[index] = get_dir_info(argv[1], index, direction_info_array, err_log, program_name);


    FILE *output = NULL;
    if ((output = fopen(argv[2], "w+")) == NULL) {
        fprintf(stderr, "%s: Unable create output file (%s)\n", program_name, argv[2]);
        return 3;
    }

    int i = 0;
    for (int i = 0; i < NUM; i++) {

        if (direction_info_array[i].name[0]) {
            printf("Name: %s\n Num of files :%d\n Dir size: %ld\n Max: %s\n Maxsize: %ld\n\n",
                   direction_info_array[i].name, direction_info_array[i].countfiles, direction_info_array[i].sumsize,
                   direction_info_array[i].max_file_name, direction_info_array[i].maxsize);

            fprintf(output, "Name: %s\n Num of files :%d\n Dir size: %ld\n Max: %s\n Maxsize: %ld\n\n",
                    direction_info_array[i].name, direction_info_array[i].countfiles, direction_info_array[i].sumsize,
                    direction_info_array[i].max_file_name, direction_info_array[i].maxsize);

        }

    }


    if (fclose(output) == EOF) {
        save_error_to_log(err_log, program_name, argv[2], strerror(errno));
    }

    print_error_log(err_log);
    return 0;
}

direction_info
get_dir_info(char *path, int index, direction_info *direction_info_array, FILE *err_log, char *program_name) {

    struct dirent *direction;


    direction_info *buff = (direction_info *) malloc(sizeof(direction_info));
    buff->sumsize = 0;
    buff->countfiles = 0;
    buff->maxsize = 0;


    DIR *dir_pointer = NULL;

    if ((dir_pointer = opendir(path)) == NULL) {
        save_error_to_log(err_log, program_name, path, strerror(errno));
        return;
    }


    while (direction = readdir(dir_pointer)) {

        //if its directory
        if ((direction->d_type == DT_DIR) &&
            ((direction->d_name[0] != '.') || ((direction->d_name[0] != '.') && (direction->d_name[1] != '.')))) {
            char dir_path[NAMESIZE];
            sprintf(dir_path, "%s/%s", path, direction->d_name);
            strcpy(buff->name, dir_path);
            index++;
            direction_info_array[index] = get_dir_info(dir_path, index, direction_info_array, err_log, program_name);
        } else if (direction->d_type == DT_REG) //if it is file
        {
            char file_path[NAMESIZE];
            sprintf(file_path, "%s/%s", path, direction->d_name);
            long buffsize = get_file_info(file_path, err_log, program_name, file_path);


            if (buffsize >= buff->maxsize) {
                buff->maxsize = buffsize;
                strcpy(buff->max_file_name, file_path);
            }

            (buff->countfiles)++;
            (buff->sumsize) += buffsize;

        } else {
            continue;
        }


    }


    strcpy(buff->name, path);
    fflush(stdout);

    if (closedir(dir_pointer) == -1) {
        save_error_to_log(err_log, program_name, path, strerror(errno));
        return;
    }

    return *buff;
}


//returns size of file in bytes
long get_file_info(char *filepath, FILE *err_log, char *program_name, char *path) {
    struct stat *filestat = malloc(sizeof(struct stat));

    if (stat(filepath, filestat) == -1) {
        save_error_to_log(err_log, program_name, path, strerror(errno));
    }
    return filestat->st_size;
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

//функция для красивого вывода директорий и файлов и их дифференциации
//#define NORMAL_COLOR "\x1B[0m"
//#define BLUE "\x1B[34m"
//#define GREEN "\x1B[32m"
//
//
//void show_dir_content(char *path) {
//    DIR *dir_pointer = opendir(path);
//    if (dir_pointer == NULL) return;
//
//
//    struct dirent *direction;
//    while (direction = readdir(dir_pointer)) {
//
//
//        if ((direction->d_type == DT_DIR) && (strcmp(direction->d_name, ".") != 0) &&
//            (strcmp(direction->d_name, "..") != 0)) {
//
//
//            char dir_path[NAMESIZE];
//            sprintf(dir_path, "%s/%s", path, direction->d_name);
//            printf("%s%s/\n", GREEN, dir_path);
//            show_dir_content(dir_path);
//
//
//        } else if (direction->d_type == DT_REG) {
//            printf("%s%s/%s\n", BLUE, path, direction->d_name);
//        }
//    }
//    closedir(dir_pointer);
//}


