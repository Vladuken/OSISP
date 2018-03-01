#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
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
}dir_struct;

void show_dir_content(char *path);
long get_file_info (char *filepath);
dir_struct get_dir_info(char *path, int index, dir_struct *darr);


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("%s %s",argv[0],"[dir] [output file]\n");
        fflush(stdout);
    }

    //try to open directory
    //printf("%d\n",get_file_info(argv[1]));

    DIR *d;
    if ((d = opendir(argv[1])) == NULL)
    {
        printf("Usage: %s [directory]\n",argv[0]);

        return 1;
    }
    printf("\n\n\n");
    closedir(d);

    int index = 0;
    dir_struct darr[NUM];
    darr[index] = get_dir_info(argv[1],index,darr);

    int i=0;
    while(darr[i].name[0])
    {
        printf("Name: %s\n\n Num of files :%d\n Dir size: %ld\n Max: %s\n Maxsize: %ld\n\n\n\n",darr[i].name,darr[i].countfiles,darr[i].sumsize,darr[i].max_file_name,darr[i].maxsize);
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
    DIR *d = opendir(path);
    if (d == NULL) return;



    struct dirent *dir;
    while (dir = readdir(d))
    {
        if ((dir->d_type == DT_DIR) && (strcmp(dir->d_name,".")!=0) && (strcmp(dir->d_name,"..")!=0))
        {


            char dpath[NAMESIZE];
            sprintf(dpath,"%s/%s",path,dir->d_name);
            printf("%s%s/\n",GREEN,dpath);
            show_dir_content(dpath);


        }
        else if (dir->d_type ==DT_REG)
        {
            printf("%s%s/%s\n",BLUE,path,dir->d_name);
        }
    }
    closedir(d);
}

//
dir_struct get_dir_info(char *path, int index, dir_struct *darr)
{

    DIR *d = opendir(path);
    if (d==NULL)
    {
        printf("ERROR\n");
    }
    //if (d == NULL) return 1;

    //dir_struct *dir_info;
    struct dirent *dir;

    dir_struct *buff = (dir_struct *)malloc(sizeof(dir_struct));
    buff->sumsize = 0;
    buff->countfiles = 0;
    buff->maxsize = 0;


    while (dir = readdir(d))
    {

        //if its directory
        if ((dir->d_type == DT_DIR) && ((dir->d_name[0]!='.') || ((dir->d_name[0]!='.') && (dir->d_name[1]!='.'))))
        {
              //printf("%s\n",dir->d_name);
              char dpath[NAMESIZE];
              sprintf(dpath,"%s/%s",path,dir->d_name);
              strcpy(buff->name,dpath);
//            for(int i = 0; i < 256; i++)
//            {
//                (buff->name)[i]=dpath[i];
//            }
            index++;
            darr[index] = get_dir_info(dpath,index,darr);
        }
        else if (dir->d_type == DT_REG) //if it is file
        {
            char fpath[NAMESIZE];
            sprintf(fpath,"%s/%s",path,dir->d_name);
            long buffsize = get_file_info(fpath);



            if (buffsize >= buff->maxsize)
            {
                buff->maxsize = buffsize;
                strcpy(buff->max_file_name,fpath);
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
    closedir(d);
    return *buff;
}


//returns size of file in bytes
long get_file_info (char * filepath)
{
    struct stat *filestat = malloc(sizeof(struct stat));

    stat(filepath,filestat);
    //printf("%d\n",filestat->st_size);
    return filestat->st_size;
}



