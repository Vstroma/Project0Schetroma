#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

void listFiles(const char *directoryName);

int main(int argc, char **argv)
{
    // Prints out the number of arguements passed to the program
    printf("Number of arguments: %d\n", argc);

    // Parse command line arguments
    DIR *directory;
    struct dirent *entry;

    // Tests whether or not directory exists/can be opened
    

    /*If there are more than than more arguement on the command line
    return an error code. But if no arguement is written return every
    file/directory in the current directory*/
    if (argc == 2)
    {
	directory = opendir(".");
	if (directory == NULL) {
        perror("tuls: cannot open directory\n");
        exit(0);
    } else {
        listFiles(argv[1]);
        while ((entry = readdir(directory)) != NULL)
        {
            printf("File: %s\n", entry->d_name);
        }
    }
}
     if (argc == 1)
    {
	directory = opendir(".");
	if (directory == NULL) {
        perror("tuls: cannot open directory\n");
        exit(0);
    } else {
	while ((entry = readdir(directory)) != NULL) {
            printf("File: %s\n", entry->d_name);
        }
	closedir(directory);
    }
}
    return 0;

}
void listFiles(const char *directoryName)
{
    // Parse command line arguments
    DIR *directory = opendir(directoryName);

    // Tests whether or not directory exists/can be opened
    if (directory == NULL)
    {
        perror("tuls: cannot open directory\n");
        exit(0);
    }

    struct dirent *entry;

    // Read through argc and argv
    while ((entry = readdir(directory)) != NULL)
    {
        /*If its a regular file return the file name,
        or if its a directory read through recursively */
        if (entry->d_type == DT_REG)
        {
            printf("File: %s\n", entry->d_name);
        }
        else if (entry->d_type == DT_DIR && (strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0))
        {
            // Specifiy the path to the directory
            char path[100] = {0};
            strcat(path, directoryName);
            strcat(path, "/");
            strcat(path, entry->d_name);

            listFiles(path);
            printf("Dir: %s/%s\n", directoryName, entry->d_name);
        }
    }
    // When argc is NULL, close the directory
    if (closedir(directory) == -1)
    {
        // If it failed to close print and error message
        perror("Error closing directory\n");
        exit(0);
    }
}
