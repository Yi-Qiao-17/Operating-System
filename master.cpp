#include<iostream>
#include<sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<string>
#include <wait.h>
#include<fstream>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <cstdlib>
using namespace std;

int main(int argc, char* argv[]) {
	pid_t pid = fork();
	if(pid<0){
		cout<<"error"<<endl;
	}
	else if (pid == 0) {
		cout<<"Successful (#"<<getpid()<<")!"<<endl;
		char* buffer[4];
		buffer[0]=argv[0];
		buffer[1]=(char*)"blake.txt";
		buffer[2]=(char*)"happy.tmp";
		buffer[3]=NULL;
		if(argc==3){
			buffer[2]=(char*)"happy2.tmp";
		}
		int a;
		a=execv("mmv.out",buffer);
	}
	else{
		wait(NULL);
	}
	return 0;
}