#ifndef CHEAT_HELPERS_H
#define CHEAT_HELPERS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int cheat_stream_contains(FILE *stream, char const *contents);

#define CHEAT_WRAP_STREAM(stream, stream_fd, name, body) \
        char filename_pattern[] = "cheat_captured_stream_" #name "_XXXXXX";\
        int original_stream;\
        int fake_stream;\
        fake_stream = mkstemp(filename_pattern);\
        fflush(stream);\
        original_stream = dup(stream_fd); \
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
    buffer[len] = '\0';

    result = strstr(buffer, contents) != NULL;

    free(buffer);

    return result;
}

#ifdef unix

#include <unistd.h>

#elif defined _WIN32

#include <windows.h>
#include <fcntl.h>

#define dup _dup
#define dup2 _dup2

int mkstemp(char * pattern) {
    /*
     * TODO: Generate a uUnique to avoid using CREATE_ALWAYS.
     * Even better: get rid of this, and try to open in a do..while loop
     * to match mkstemp.
     */
    char tempFileName[MAX_PATH];
    GetTempFileName(
        ".",
        pattern,
        0,
        tempFileName);

    HANDLE tempFile = CreateFile(
        tempFileName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS, /* Bad. Should be _NEW, to avoid races. */
        0,
        NULL);

    return _open_osfhandle((intptr_t)tempFile, _O_APPEND);
}

#else

#warning "Capturing output not supported in this platform. Capturing tests will be ignored."

#undef TEST_WITH_CAPTURED_STDOUT
#undef TEST_WITH_CAPTURED_STDERR
#undef TEST_WITH_CAPTURED_OUTPUT
#define TEST_WITH_CAPTURED_STDOUT(name, body) TEST_IGNORE(name, {})
#define TEST_WITH_CAPTURED_STDERR(name, body) TEST_IGNORE(name, {})
#define TEST_WITH_CAPTURED_OUTPUT(name, body) TEST_IGNORE(name, {})

#endif

#endif
