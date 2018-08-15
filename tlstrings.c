#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <pcre.h>
#include <dirent.h>

//gcc -o tlstrings tlstrings.c -I/usr/include -lpcre -lpthread

pthread_t tid[99999];


void *getlinks(void *filename)
{
    pthread_t id = pthread_self();
    //printf("running: %s\n", (char *)filename);
    FILE *fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    const char *error;
    int erroffset;
    int ovector[186];
    char re[8192]="";
    char re1[]="((?:http|https)(?::\\/{2}[\\w]+)(?:[\\/|\\.]?)(?:[^\\s\"\\><\')]*))";
    strcat(re,re1);

    pcre *r =  pcre_compile(re, PCRE_CASELESS|PCRE_DOTALL, &error, &erroffset, NULL);

    fp = fopen((char *)filename,  "r");

    while ((read = getline(&line, &len, fp)) != -1)
    {
        int rc = pcre_exec(r, NULL, line, strlen(line), 0, 0, ovector, 186);
        if (rc>0)
        {
          char httpurl1[1024];
          pcre_copy_substring(line, ovector, rc,1,httpurl1, 1024);
          printf("%s | %s\n", (char *)filename, httpurl1);
        }
    }
    fclose(fp);
    if (line)
        free(line);
    pthread_exit(0);
}

int main(int argc, char *argv[])
{
    int i = 0;
    int j;
    int err;
    DIR *dp;
    struct dirent *ep;
    dp = opendir(argv[1]);
    if (dp != NULL)
    {
        while ((ep = readdir(dp)) != NULL)
        {
            char *filepath = argv[1];
            char *filename = (ep->d_name);
            if (strcmp(filename, ".") != 0 && strcmp(filename, "..") != 0)
            {
                char *fpath = malloc( strlen( argv[1] ) + strlen( filename ) + 1 );
                strcpy(fpath, argv[1]);
                strcat(fpath, filename);
                //printf("%s\n", fpath);
                char *test = fpath;
                err = pthread_create(&tid[i], NULL, &getlinks, (void *)test);
                if (err != 0)
                {
                    printf("\ncan't create thread :[%s]", strerror(err));
                } else {
                    i++;
                }
            }
        }
        for(j=0; j < i; j++)
        {
            pthread_join(tid[i], NULL);
        }
        closedir (dp);
    }
    else
    {
        perror ("Couldn't open the directory");
    }
    sleep(1);
    return 0;
}