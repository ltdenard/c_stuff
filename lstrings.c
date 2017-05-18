#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

#define tofind "(http|https):\\/\\/[A-Za-z\\.]*\\.[a-zA-Z]{2,6}[^ )]+"

#define MAX_MATCHES 1


int main(int argc, char *argv[])
{
    FILE *fp;
    char * line = NULL;
    int retval = 0;
    size_t len = 0;
    ssize_t read;
    regex_t re;

    if (regcomp(&re, tofind, REG_EXTENDED) != 0)
    {
        fprintf(stderr, "Failed to compile regex '%s'\n", tofind);
        return EXIT_FAILURE;
    }

    fp = fopen(argv[1],  "r");
    while ((read = getline(&line, &len, fp)) != -1) {
        regmatch_t matches[MAX_MATCHES];
        line[strlen(line)-1] = '\0';
        if ((retval = regexec(&re, line, MAX_MATCHES, matches, 0)) == 0)
        {
            printf("%s | %.*s\n", argv[1], (int)(matches[0].rm_eo - matches[0].rm_so), line + matches[0].rm_so);
        }
    }
    fclose(fp);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
}