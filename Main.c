#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <Windows.h>

#include "ConfigPaser.h"
extern int _skipAnnotationAndSpaces(FILE **fpp);
int main(){
	char *keys[3] = { "cccc", "aaa", "a" };
	char * vals[3];
	vals[0] = malloc(sizeof(char) * 10);
	vals[1] = malloc(sizeof(char) * 10);
	vals[2] = malloc(sizeof(char) * 10);
	if (NULL == vals[0] || NULL == vals[1] || NULL == vals[2]){
		printf("can't alloc memorys\n");
		free(vals[0]); vals[0] = NULL;
		free(vals[1]); vals[1] = NULL;
		free(vals[2]); vals[2] = NULL;
		system("pause");
		return 1;
	}
	int bufSize[] = { 10, 10, 10 };
	readConfigFile("D:\\aaa.ini", keys, vals, bufSize, 3);
	printf("result:\n");
	printf("%d\n", strcmp(vals[1], "a123"));
	printf("%s\n%s\n%s\n", vals[0], vals[1], vals[2]);
	free(vals[0]); vals[0] = NULL;
	free(vals[1]); vals[1] = NULL;
	free(vals[2]); vals[2] = NULL;
	system("pause");
}