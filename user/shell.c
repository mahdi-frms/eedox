#include <stdlib.h>

#define BUFFER_SIZE 256

int main()
{
    char buffer[BUFFER_SIZE];
    while (1)
    {
        printf("sh->");
        int len = read(STDIN, buffer, BUFFER_SIZE);
        if (len > 1)
        {
            write(STDOUT, buffer, len);
        }
    }
    exit(0);
    return 0;
}