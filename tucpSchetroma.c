#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

void copyFile(const char *source, const char *destination);
void copyToDir(const char *sourceName, const char *destinationDir);

int main(int argc, char **argv)
{
    
    if (argc < 3) {				// if < 3 arguments throw an error 
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        return 1;
    }

    const char *dest = argv[argc - 1];		// destination file pointer

   				// command line arguments
    for (int i = 1; i < argc - 1; i++) {

        struct stat srcStat;
			
        if (stat(argv[i], &srcStat) == -1) {

            perror("Error getting source file/directory information");
            continue;
        }

        if (S_ISREG(srcStat.st_mode))    {
            						// copy file to destination 
            copyFile(argv[i], dest);
        }
        else if (S_ISDIR(srcStat.st_mode))      {	// copy contents of source directory to destination
						
            copyToDir(argv[i], dest);
        }
        else 	{
            fprintf(stderr, "Unsupported file type for source: %s\n", argv[i]);		// else throw error
        }
    }

    return 0;
}

void copyFile(const char *source, const char *destination)
{
    // Check if destFileName is a directory
    struct stat destStat;
    if (stat(destination, &destStat) == 0 && S_ISDIR(destStat.st_mode))		{

        // if destination is a directory, create the path within that directory

        char *srcBaseName = strrchr(source, '/');
        if (srcBaseName == NULL)    {

            srcBaseName = (char *)source;
        }
        else	{
            srcBaseName++; // Skip the slash
        }

        char destFilePath[strlen(destination) + strlen(srcBaseName) + 2];		
        snprintf(destFilePath, sizeof(destFilePath), "%s/%s", destination, srcBaseName);

        // Open the destination file for writing
        FILE *destFile = fopen(destFilePath, "wb");
        if (destFile == NULL)	{

            perror("Error opening/creating destination file");
            return;
        }

        // Open the source file for reading
        FILE *srcFile = fopen(source, "rb");
        if (srcFile == NULL)	{

            perror("Error opening source file");
            fclose(destFile);
            return;
        }

        char buffer[1024];
        size_t bytesRead;

        // Copy data from source to destination
        while ((bytesRead = fread(buffer, 1, sizeof(buffer), srcFile)) > 0)	{

            fwrite(buffer, 1, bytesRead, destFile);
        }

        fclose(srcFile);
        fclose(destFile);
    }

    else 	{
     
        FILE *srcFile = fopen(source, "rb");		//open source file
        if (srcFile == NULL)	{

            perror("Error opening source file");
            exit(1);
        }

        // Open or create the destination file for writing
        FILE *destFile = fopen(destination, "wb");
        if (destFile == NULL)	{

            // Attempt to create the destination file if it doesn't exist
            destFile = fopen(destination, "wb");
            if (destFile == NULL)	{
                perror("Error opening/creating destination file");
                fclose(srcFile);
                exit(1);
            }
        }
        char buffer[1024];
        size_t bytesRead;

        // Copy data from source to destination
        while ((bytesRead = fread(buffer, 1, sizeof(buffer), srcFile)) > 0)	{

            fwrite(buffer, 1, bytesRead, destFile);
        }

        fclose(srcFile);
        fclose(destFile);
    }
}

void copyToDir(const char *sourceName, const char *destinationDir)
{
    // Open the source directory
    DIR *srcDir = opendir(sourceName);
    if (srcDir == NULL)		{
        perror("Error opening source directory");
        return;
    }

    struct dirent *entry;

    // Traverse the source directory
    while ((entry = readdir(srcDir)))	{

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)	{	//skip dot entries

            continue;
        }

        char srcPath[strlen(sourceName) + strlen(entry->d_name) + 2];		// path lengths
        char destPath[strlen(destinationDir) + strlen(entry->d_name) + 2];

        snprintf(srcPath, sizeof(srcPath), "%s/%s", sourceName, entry->d_name);
        snprintf(destPath, sizeof(destPath), "%s/%s", destinationDir, entry->d_name);

        struct stat statbuf;
        if (stat(srcPath, &statbuf) == -1)	{

            perror("Error getting file/directory information");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode))	{

            // Recursively copy directories
            copyToDir(srcPath, destPath);
        }
        else if (S_ISREG(statbuf.st_mode))	{

            // Copy regular files
            copyFile(srcPath, destPath);
        }
    }

    closedir(srcDir);
}