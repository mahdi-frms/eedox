#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>
#include <../src/util.h>

#define STDOUT 1
#define STDIN 0

int write(int fd, const void *buffer, int length);
int read(int fd, const void *buffer, int length);
int open(const char *path, int flags);
int close(int fd);
int opendir(const char *path);
int readdir(int fd, char* buffer);
int getcwd(char *buffer);
int setcwd(const char *buffer);
int exit(short int statuscode);
int exec(const char *path,char* argv []);
int wait(short int *statuscode);
int wait_pid(int pid, short int *statuscode);
int getpid();
int fork();

int _printf(const char *message, va_list args);
int printf(const char *message, ...);

#endif