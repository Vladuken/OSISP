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
#include <locale.h>
#include <wchar.h>

#define ERR_LOG_PATH "/tmp/err.log"

void print_error_log(FILE *err_log);

void print_error_log(FILE *err_log);

void get_file_info(char *filepath, FILE *err_log, char *program_name, char *path);

void recurcive_dir_pass(char *path, FILE *err_log,FILE *output, char *program_name,const int num_of_process);
void word_count(FILE *file, FILE *err_log, char *program_name, char* path);
int main(int argc, char *argv[]) {

    char *program_name = basename(argv[0]);

    FILE *err_log = NULL;
    if ((err_log = fopen(ERR_LOG_PATH, "w+")) == NULL) {
        fprintf(stderr, "%s: Unable create error log (%s)\n", program_name, ERR_LOG_PATH);
        return 1;
    }

    if (argc != 4) {
        save_error_to_log(err_log, program_name, "Wrong number of parameters. Usage", "./lab2.exe [pathname] [outfile] [N of files]");
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

    int max_process_num = atoi(argv[3]);
    if (max_process_num <= 0)
    {
        fprintf(stderr, "%s: Bad N parameter.\n", program_name);
        return 4;
    }


    recurcive_dir_pass(argv[1], err_log,output, program_name, atoi(argv[3]));

    if (fclose(output) == EOF) {
        save_error_to_log(err_log, program_name, argv[2], strerror(errno));
    }
    print_error_log(err_log);
    return 0;
}




int N = 0;
//////////////////////////////////////////////////////////////////
void recurcive_dir_pass(char *path, FILE *err_log,FILE *output, char *program_name, const int num_of_process) {
    struct dirent *direction;
    DIR *dir_pointer = NULL;

    if ((dir_pointer = opendir(path)) == NULL)
    {
        //fflush(stdout);
        //save_error_to_log(err_log, program_name, path, strerror(errno));
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
            recurcive_dir_pass(dirpath,err_log,output,program_name, num_of_process);
        }
        else if(direction->d_type == DT_REG)
        {
            get_file_info(dirpath,err_log,program_name,path);
            //function to work with files
        }
        else
        {
            continue;
        }


        free(dirpath);

    }


    if (errno == EBADF)
    {
        save_error_to_log(err_log, program_name, path, strerror(errno));
        //errno = 0;
        return;
    }

    if (closedir(dir_pointer) == -1) {
        save_error_to_log(err_log, program_name, path, strerror(errno));
        return;
    }
}



ino_t *visited_inodes = NULL;
int visited_inode_len = 0;
//get info file pattern
void get_file_info(char *filepath, FILE *err_log, char *program_name, char *path)
{
    struct stat *filestat = malloc(sizeof(struct stat));

    if (stat(filepath, filestat) == -1) {
        free(filestat);
        //save_error_to_log(err_log, program_name, path, strerror(errno));
        return;
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
            return;
        }
        else
        {
            visited_inodes = (ino_t*) realloc(visited_inodes,sizeof(ino_t) * (visited_inode_len + 1));
            visited_inodes[visited_inode_len] = filestat->st_ino;
            visited_inode_len++;

            //there is functon
            open_file_and_wc(filepath,err_log,program_name);
            //work with files

            return;
        }

    } else if(filestat->st_nlink == 1)
    {
        open_file_and_wc(filepath,err_log,program_name);
        return;
    }
}


void open_file_and_wc(char * path, FILE *err_log, char *program_name)
{
    FILE *file = NULL;
    if ((file = fopen(path, "r")) == NULL) {
        save_error_to_log(err_log, program_name, path, strerror(errno));
        return;
    }
    word_count(file,err_log,program_name, path);
    return;
}

void word_count(FILE *file, FILE *err_log, char *program_name,char * path)
{
    setlocale(LC_ALL, "");
    wint_t c;
    unsigned long long countchars = 0;
    unsigned long long countwords = 0;
    unsigned long long bytes = 0;


    short inword = 0;
    //count chars in file
    do
    {
        c = fgetwc(file);
        if ((c == WEOF) && (errno == EILSEQ))
        {
            errno = 0;
            c = getc(file);

        }
        else if (c == WEOF)
        {
            break;
        }
        countchars++;
        switch (c)
        {
            case '\n':
            case '\r':
            case '\f':
            case '\t':
            case ' ':
            case '\v':
                if (inword)
                {
                    inword = 0;
                    countwords++;
                }
                break;
            default:
                inword = 1;
                break;
        }

    } while (c != WEOF);

//////////////////////////////////////////////////
    if (errno == EILSEQ)
    {
        save_error_to_log(err_log, program_name, path, strerror(errno));
        errno = 0;
    }
//
//    if (errno == EILSEQ)
//    {
//        printf("AAAAAAAAAAAAAAAA");
//        errno = 0;
//    }
    //printf(, N);
    //printf("%d %s ",getpid(), path);
    printf("%d %d %s %lld %lld\n",N, getpid(),path,countchars,countwords);
}

// Print error message to temporary file err_log.
void save_error_to_log(FILE *err_log, const char *program_name, const char *directory, const char *error_message) {

//    printf("%d o\n",getpid());

    fprintf(err_log, "%d N:%d  %s: %s: %s\n", getpid(),N,program_name, directory, error_message);
    return;
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







































































///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void recurcive_dir_pass(char *path, FILE *err_log,FILE *output, char *program_name) {
//    struct dirent *direction;
//    DIR *dir_pointer = NULL;
//
//    if ((dir_pointer = opendir(path)) == NULL) {
//        save_error_to_log(err_log, program_name, path, strerror(errno));
//        return;
//    }
//
//    while (direction = readdir(dir_pointer))
//    {
//        if ((strcmp(direction->d_name,".") == 0) || (strcmp(direction->d_name,"..") == 0))
//        {
//            continue;
//        }
//
//        char *dirpath = (char*) malloc(sizeof(char)*(strlen(path)+strlen(direction->d_name) + 2));
//        sprintf(dirpath, "%s/%s", path, direction->d_name);
//
//        //if its directory
//        if (direction->d_type == DT_DIR)
//        {
//            recurcive_dir_pass(dirpath,err_log,output,program_name);
//        }
//        else if(direction->d_type == DT_REG)
//        {
//            //function to work with files
//        }
//        else
//        {
//            continue;
//        }
//        free(dirpath);
//    }
//    if (errno == EBADF)
//    {
//        save_error_to_log(err_log, program_name, path, strerror(errno));
//    }
//
//    if (closedir(dir_pointer) == -1) {
//        save_error_to_log(err_log, program_name, path, strerror(errno));
//        return;
//    }
//}
//
//



//ino_t *visited_inodes = NULL;
//int visited_inode_len = 0;
////get info file pattern
//void get_file_info_pattern(char *filepath, FILE *err_log, char *program_name, char *path) {
//    struct stat *filestat = malloc(sizeof(struct stat));
//
//    if (stat(filepath, filestat) == -1) {
//        save_error_to_log(err_log, program_name, path, strerror(errno));
//    }
//
//    int is_in = 0;
//
//
//
//    //added check for inodes
//    if (filestat->st_nlink > 1)
//    {
//        // Array contains visited inodes numbers.
//        for (int i = 0; i < visited_inode_len; i++)
//        {
//            if (visited_inodes[i] == filestat->st_ino)
//            {
//                is_in = 1;
//            }
//        }
//
//
//        if (is_in)
//        {
//            return;
//        }
//        else
//        {
//            visited_inodes = (ino_t*) realloc(visited_inodes,sizeof(ino_t) * (visited_inode_len + 1));
//            visited_inodes[visited_inode_len] = filestat->st_ino;
//            visited_inode_len++;
//
//            //work with files
//
//            return;
//        }
//
//    }
//}
//
//




