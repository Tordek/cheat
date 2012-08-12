#ifndef CHEAT_HELPERS_H
#define CHEAT_HELPERS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int cheat_stream_contains(FILE *stream, char const *contents);

#ifdef unix

#include <unistd.h>

#define CHEAT_WRAP_STREAM(stream, stream_fd, name, body) \
        char filename_pattern[] = "cheat_captured_stream_" #name "_XXXXXX";\
        int original_stream;\
        int fake_stream;\
        fflush(stream);\
        original_stream = dup(stream_fd); \
        fake_stream = mkstemp(filename_pattern);\
        dup2(fake_stream, stream_fd);\
        setbuf(stream, NULL);\
        body\
        fflush(stream);\
        dup2(original_stream, stream_fd);

#define TEST_WITH_CAPTURED_STDOUT(name, body) TEST(name, { CHEAT_WRAP_STREAM(stdout, STDOUT_FILENO, name, body) } )
#define TEST_WITH_CAPTURED_STDERR(name, body) TEST(name, { CHEAT_WRAP_STREAM(stderr, STDERR_FILENO, name, body) } )
#define TEST_WITH_CAPTURED_OUTPUT(name, body) TEST(name, {\
        CHEAT_WRAP_STREAM(stderr, STDERR_FILENO, name,\
            CHEAT_WRAP_STREAM(stdout, STDOUT_FILENO, name, body)\
        ))

int cheat_stream_contains(FILE *stream, char const *contents)
{
    char *buffer;
    int result;
    int len;

    len = ftell(stream);
    fseek(stream, 0, SEEK_SET);

    buffer = malloc(len + 1);
    fread(buffer, 1, len, stream);

    result = strstr(buffer, contents) != NULL;

    free(buffer);

    return result;
}

#else
#error "Unsupported platform. Sorry!"
#endif

#endif
