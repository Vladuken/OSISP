#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define NUM 1000
#define NAMESIZE 1000

typedef struct {
    int countfiles;
    char name[NAMESIZE];
    long int sumsize;
    long int maxsize;
    char max_file_name[NAMESIZE];
}direction_info;


void show_dir_content(char *path);
long get_file_info (char *filepath);
direction_info get_dir_info(char *path, int index, direction_info *direction_info_array);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("%s %s",argv[0],"[direction] [output file]\n");
        fflush(stdout);
    }

    //try to open directory
    //printf("%dir_pointer\n",get_file_info(argv[1]));

    
    DIR *dir_pointer;
    if ((dir_pointer = opendir(argv[1])) == NULL)
    {
        printf("Usage: %s [directory]\n",argv[0]);

        return 1;
    }
    printf("\n\n\n");
    closedir(dir_pointer);

    int index = 0;
    direction_info direction_info_array[NUM];
    direction_info_array[index] = get_dir_info(argv[1],index,direction_info_array);

    int i=0;
    while(direction_info_array[i].name[0])
    {
        printf("Name: %s\n\n Num of files :%dir_pointer\n Dir size: %ld\n Max: %s\n Maxsize: %ld\n\n\n\n",direction_info_array[i].name,direction_info_array[i].countfiles,direction_info_array[i].sumsize,direction_info_array[i].max_file_name,direction_info_array[i].maxsize);
        i++;
    }




    return 0;


}




//функция для красивого вывода директорий и файлоф и их дифференциации
#define NORMAL_COLOR "\x1B[0m"
#define BLUE "\x1B[34m"
#define GREEN "\x1B[32m"


void show_dir_content(char *path)
{
    DIR *dir_pointer = opendir(path);
    if (dir_pointer == NULL) return;



    struct dirent *direction;
    while (direction = readdir(dir_pointer))
    {

        
        if ((direction->d_type == DT_DIR) && (strcmp(direction->d_name,".")!=0) && (strcmp(direction->d_name,"..")!=0))
        {


            char dir_path[NAMESIZE];
            sprintf(dir_path,"%s/%s",path,direction->d_name);
            printf("%s%s/\n",GREEN,dir_path);
            show_dir_content(dir_path);


        }
        else if (direction->d_type ==DT_REG)
        {
            printf("%s%s/%s\n",BLUE,path,direction->d_name);
        }
    }
    closedir(dir_pointer);
}

//
direction_info get_dir_info(char *path, int index, direction_info *direction_info_array)
{

    DIR *dir_pointer = opendir(path);

    if (dir_pointer==NULL)
    {
        printf("ERROR\n");
    }

    //if (dir_pointer == NULL) return 1;

    //direction_info *dir_info;
    struct dirent *direction;


    direction_info *buff = (direction_info *)malloc(sizeof(direction_info));
    buff->sumsize = 0;
    buff->countfiles = 0;
    buff->maxsize = 0;

    while (direction = readdir(dir_pointer))
    {

        //if its directory
        if ((direction->d_type == DT_DIR) && ((direction->d_name[0]!='.') || ((direction->d_name[0]!='.') && (direction->d_name[1]!='.'))))
        {
              //printf("%s\n",direction->d_name);
              char dir_path[NAMESIZE];
              sprintf(dir_path,"%s/%s",path,direction->d_name);
              strcpy(buff->name,dir_path);
//            for(int i = 0; i < 256; i++)
//            {
//                (buff->name)[i]=dir_path[i];
//            }
            index++;
            direction_info_array[index] = get_dir_info(dir_path,index,direction_info_array);
        }
        else if (direction->d_type == DT_REG) //if it is file
        {
            char file_path[NAMESIZE];
            sprintf(file_path,"%s/%s",path,direction->d_name);
            long buffsize = get_file_info(file_path);



            if (buffsize >= buff->maxsize)
            {
                buff->maxsize = buffsize;
                strcpy(buff->max_file_name,file_path);
            }

            (buff->countfiles)++;
            (buff->sumsize)+=buffsize;
            
        }
        else
        {
            continue;
        }




    }


    strcpy(buff->name,path);
    fflush(stdout);
    closedir(dir_pointer);
    return *buff;
}


//returns size of file in bytes
long get_file_info (char * filepath)
{
    struct stat *filestat = malloc(sizeof(struct stat));

    stat(filepath,filestat);
    //printf("%dir_pointer\n",filestat->st_size);
    return filestat->st_size;
}



