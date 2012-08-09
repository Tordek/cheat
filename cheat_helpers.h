#ifndef CHEAT_HELPERS_H
#define CHEAT_HELPERS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef unix

#include <unistd.h>

#define TEST_WITH_CAPTURED_STDOUT(name, body) TEST(name, {\
        fflush(stdout);\
        char filename_pattern[] = "cheat_captured_stdout_" #name "_XXXXXX";\
        int original_stdout;\
        int fake_stdout;\
        original_stdout = dup(STDOUT_FILENO); \
        fake_stdout = mkstemp(filename_pattern);\
        dup2(fake_stdout, STDOUT_FILENO);\
        { body }\
        fflush(stdout);\
        dup2(original_stdout, STDOUT_FILENO); \
        })

int cheat_stdout_contains(char *contents)
{
    char *buffer;
    int result;
    int len;

    fflush(stdout);
    len  = lseek(STDOUT_FILENO, 0, SEEK_CUR);
    lseek(STDOUT_FILENO, 0, SEEK_SET);
    buffer = malloc(len + 1);

    read(STDOUT_FILENO, buffer, len);
    result = strstr(buffer, contents) != NULL;

    free(buffer);

    return result;
}

#else
#error "Unsupported platform. Sorry!"
#endif

#endif
