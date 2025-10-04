/*
* Programming Assignment 02: ls v1.1.0
* Base: v1.0.0 + Feature 2 (-l long listing)
* Usage:
*   $ ./bin/ls
*   $ ./bin/ls -l
*   $ ./bin/ls -l /home /etc
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <pwd.h>      // getpwuid()
#include <grp.h>      // getgrgid()
#include <time.h>     // strftime()
#include <limits.h>   // PATH_MAX

extern int errno;

void do_ls(const char *dir, int long_flag);   // ✅ added semicolon here!

void mode_to_str(mode_t mode, char *str);

int main(int argc, char const *argv[])
{
    int long_flag = 0;
    int opt;

    // detect -l option
    while ((opt = getopt(argc, (char * const *)argv, "l")) != -1)
    {
        switch (opt)
        {
        case 'l':
            long_flag = 1;
            break;
        default:
            fprintf(stderr, "Usage: %s [-l] [dir...]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // remaining args after options
    if (optind == argc)
    {
        do_ls(".", long_flag);
    }
    else
    {
        for (int i = optind; i < argc; i++)
        {
            printf("Directory listing of %s:\n", argv[i]);
            do_ls(argv[i], long_flag);
            puts("");
        }
    }
    return 0;
}

void mode_to_str(mode_t mode, char *str)
{
    str[0] = S_ISDIR(mode) ? 'd' :
             S_ISLNK(mode) ? 'l' :
             S_ISCHR(mode) ? 'c' :
             S_ISBLK(mode) ? 'b' :
             S_ISFIFO(mode)? 'p' :
             S_ISSOCK(mode)? 's' : '-';

    str[1] = (mode & S_IRUSR) ? 'r' : '-';
    str[2] = (mode & S_IWUSR) ? 'w' : '-';
    str[3] = (mode & S_IXUSR) ? 'x' : '-';
    str[4] = (mode & S_IRGRP) ? 'r' : '-';
    str[5] = (mode & S_IWGRP) ? 'w' : '-';
    str[6] = (mode & S_IXGRP) ? 'x' : '-';
    str[7] = (mode & S_IROTH) ? 'r' : '-';
    str[8] = (mode & S_IWOTH) ? 'w' : '-';
    str[9] = (mode & S_IXOTH) ? 'x' : '-';
    str[10] = '\0';
}

void do_ls(const char *dir, int long_flag)
{
    struct dirent *entry;
    DIR *dp = opendir(dir);

    if (dp == NULL)
    {
        fprintf(stderr, "Cannot open directory: %s\n", dir);
        return;
    }

    errno = 0;

    // --- Long listing mode (existing from v1.1.0) ---
    if (long_flag)
    {
        while ((entry = readdir(dp)) != NULL)
        {
            if (entry->d_name[0] == '.')
                continue;

            char path[PATH_MAX];
            snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);

            struct stat st;
            if (lstat(path, &st) == -1)
            {
                perror("lstat");
                continue;
            }

            char perms[11];
            mode_to_str(st.st_mode, perms);

            struct passwd *pw = getpwuid(st.st_uid);
            struct group  *gr = getgrgid(st.st_gid);

            char timebuf[64];
            struct tm *tm = localtime(&st.st_mtime);
            strftime(timebuf, sizeof(timebuf), "%b %e %H:%M", tm);

            printf("%s %2ld %s %s %6ld %s %s\n",
                   perms,
                   (long)st.st_nlink,
                   pw ? pw->pw_name : "?",
                   gr ? gr->gr_name : "?",
                   (long)st.st_size,
                   timebuf,
                   entry->d_name);
        }
    }
    else
    {
        // --- Column display mode (new for Feature 3) ---
        char *names[1024];
        int count = 0, maxlen = 0;

        while ((entry = readdir(dp)) != NULL)
        {
            if (entry->d_name[0] == '.')
                continue;
            names[count] = strdup(entry->d_name);
            if ((int)strlen(entry->d_name) > maxlen)
                maxlen = strlen(entry->d_name);
            count++;
        }

        if (errno != 0)
        {
            perror("readdir failed");
            closedir(dp);
            return;
        }

        // Determine terminal width
        int term_width = 80;
        char *cols_env = getenv("COLUMNS");
        if (cols_env)
            term_width = atoi(cols_env);

        int col_width = maxlen + 2;
        int cols = term_width / col_width;
        if (cols == 0) cols = 1;
        int rows = (count + cols - 1) / cols;

        for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < cols; c++)
            {
                int idx = c * rows + r;
                if (idx < count)
                    printf("%-*s", col_width, names[idx]);
            }
            printf("\n");
        }

        // free allocated memory
        for (int i = 0; i < count; i++)
            free(names[i]);
    }

    closedir(dp);
}

