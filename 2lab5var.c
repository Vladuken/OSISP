#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <dirent.h>
#include <string.h>

void show_dir_content(char *path);

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("%s %s",argv[0],"[dir] [output file]\n");
	}


	//try to open directory
	DIR *d;
	if ((d = opendir(argv[1])) == NULL)
	{
		printf("Usage: %s [directory]\n",argv[0]);
		return 1;
	}
	closedir(d);

	show_dir_content(argv[1]);


	// struct dirent *dir;
	// while (dir = readdir(d))
	// {
	// 	if ((dir = readdir(d)) -> d_type == DT_DIR)
	// 	{
	// 		printf("%s\n",dir->d_name);
	// 	}
	// 	else
	// 	{
	// 		printf("%s\n",dir->d_name);
	// 	}





	//show_dir_content(argv[1]);
	return 0;


}

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
			printf("%s%s\n",GREEN,dir->d_name);

			char dpath[255];
			sprintf(dpath,"%s/%s",path,dir->d_name);
			show_dir_content(dpath);
		}
		else if (dir->d_type ==DT_REG)
		{
			printf("%s%s/%s\n",BLUE,path,dir->d_name);
		}
	}
	closedir(d);
}
