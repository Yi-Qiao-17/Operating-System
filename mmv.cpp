#include<iostream>
#include<sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include<string>
#include<fstream>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;

int main(int argc, char* argv[]) {
    int fileread=open(argv[1],O_RDONLY);
    int filewrite=open(argv[2],O_WRONLY|O_CREAT,0777);
    if (fileread ==-1)
    {
        cout << argv[1]<<" can not open." << endl;
    }
    if (filewrite ==-1)
    {
        cout <<argv[2] <<" can not open." << endl;
    }
    write(filewrite,"\\\\----Say Hello to s1081420!----\\\\\n",sizeof("\\\\----Say Hello to s1081420!----\\\\\n"));
    struct stat buf;
    stat(argv[1], &buf);
    char* tmp[10000];
    read(fileread,tmp,buf.st_size);
    write(filewrite,tmp,buf.st_size);
    close(filewrite);
    close(fileread);
    return 0;
	
}