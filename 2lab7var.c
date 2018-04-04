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

void get_file_info(char *filepath, FILE *err_log, char *program_name, char *path);

void recurcive_dir_pass(char *path, FILE *err_log,FILE *output, char *program_name);
int N1,N2;
int main(int argc, char *argv[]) {

    char *program_name = basename(argv[0]);

    FILE *err_log = NULL;
    if ((err_log = fopen(ERR_LOG_PATH, "w+")) == NULL) {
        fprintf(stderr, "%s: Unable create error log (%s)\n", program_name, ERR_LOG_PATH);
        return 1;
    }

    if (argc != 4) {
        save_error_to_log(err_log, program_name, "Wrong number of parameters. Usage", "./lab2.exe [pathname] [N1] [N2]");
        print_error_log(err_log);
        return 1;
    }

    N1 = atoi(argv[2]);
    N2 = atoi(argv[3]);

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
    recurcive_dir_pass(argv[1], err_log,output, program_name);
    pass_files();
    if (fclose(output) == EOF) {
        save_error_to_log(err_log, program_name, argv[2], strerror(errno));
    }

    print_error_log(err_log);
    return 0;
}


void recurcive_dir_pass(char *path, FILE *err_log,FILE *output, char *program_name) {
    struct dirent *direction;
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
            recurcive_dir_pass(dirpath,err_log,output,program_name);
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
            return;
        }
        else
        {
            visited_inodes = (ino_t*) realloc(visited_inodes,sizeof(ino_t) * (visited_inode_len + 1));
            visited_inodes[visited_inode_len] = filestat->st_ino;
            visited_inode_len++;

            //there is functon
            //open_file_and_wc(filepath,err_log,program_name);
            //work with files
            ///////////////////////////////////////////////////////////////////
            addfile(filestat,filepath);
            return;
        }

    } else if(filestat->st_nlink == 1)
    {
        addfile(filestat,filepath);
        //open_file_and_wc(filepath,err_log,program_name);
    }
}



char ** Files = NULL;
int Files_Len = 0;
void addfile(struct stat *filestat, char *path)
{

    if (filestat->st_size >= N1 && filestat->st_size <= N2)
    {
        Files = (char **) realloc(Files,sizeof(char **) * (Files_Len + 1));
        Files[Files_Len] = (char *) malloc(sizeof(char) * (strlen(path) + 1));
        sprintf(Files[Files_Len], "%s", path);
        Files_Len++;
    }

}

void pass_files()
{
    for(int i=0; i<Files_Len; i++)
    {
        for(int j = i+1; j < Files_Len; j++)
        {
            compare_two_files(Files[i],Files[j]);
        }
    }
}

void compare_two_files(char * path1, char * path2)
{
    struct stat *filestat1 = malloc(sizeof(struct stat));
    struct stat *filestat2 = malloc(sizeof(struct stat));

//    if (stat(path1, filestat1) == -1) {
//        save_error_to_log(err_log, program_name, path, strerror(errno));
//    }

    stat(path1,filestat1);
    stat(path2,filestat2);

    if (filestat1->st_size == filestat2->st_size)
    {
        //тут сравнение файлов
        FILE * fp1 = fopen(path1, "r");
        FILE * fp2 = fopen(path2, "r");

        if (fp1 == NULL) {
            printf("Cannot open %s for reading \n", path1);
            exit(1);
        } else if (fp2 == NULL) {
            printf("Cannot open %s for reading \n", path2);
            exit(1);
        } else {
            char ch1 = getc(fp1);
            char ch2 = getc(fp2);

            while ((ch1 != EOF) && (ch2 != EOF) && (ch1 == ch2)) {
                ch1 = getc(fp1);
                ch2 = getc(fp2);
            }

            if (ch1 == ch2)
                printf("%s = %s\n", path1, path2);

            fclose(fp1);
            fclose(fp2);
        }

    }
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
