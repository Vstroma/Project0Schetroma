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

   				// parse through command line arguments
    for (int i = 1; i < argc - 1; i++) {

        struct stat srcStat;
			
        if (stat(argv[i], &srcStat) == -1) {

            perror("Error getting source file/directory information");		// error with file or directory
            continue;
        }

        if (S_ISREG(srcStat.st_mode))    {
            						// copy file to destination 
            copyFile(argv[i], dest);
        }
        else if (S_ISDIR(srcStat.st_mode))      {	// copy contents of source to directory
						
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
    			//checks if destination is a directory
    struct stat destStat;
    if (stat(destination, &destStat) == 0 && S_ISDIR(destStat.st_mode))		{

        								// if destination is a directory create a path

        char *srcBaseName = strrchr(source, '/');
        if (srcBaseName == NULL)    {

            srcBaseName = (char *)source;
        }
        else	{
            srcBaseName++; // Skip the slash
        }

        char destFilePath[strlen(destination) + strlen(srcBaseName) + 2];		//allocates length for destfilepath
        snprintf(destFilePath, sizeof(destFilePath), "%s/%s", destination, srcBaseName);	// redirects into buffer

        								
        FILE *destFile = fopen(destFilePath, "wb");			// open destination file for writing 
        if (destFile == NULL)	{

            perror("Error opening/creating destination file");
            return;
        }

        								
        FILE *srcFile = fopen(source, "rb");				// open source to read
        if (srcFile == NULL)	{

            perror("Error opening source file");
            fclose(destFile);
            return;
        }
	
        char buffer[1024];			// initialize buffer
        size_t bytesRead;

       											
        while ((bytesRead = fread(buffer, 1, sizeof(buffer), srcFile)) > 0)	{		// copy from source to destnationfile

            fwrite(buffer, 1, bytesRead, destFile);
        }

        fclose(srcFile);
        fclose(destFile);
    }

    else 	{				// if destination is a file 
     
        FILE *srcFile = fopen(source, "rb");		//open source file
        if (srcFile == NULL)	{

            perror("Error opening source file");
            exit(1);
        }

        						
        FILE *destFile = fopen(destination, "wb");				// open or make destination file for writing
        if (destFile == NULL)	{

           							
            destFile = fopen(destination, "wb");			// create destination file if DNE
            if (destFile == NULL)	{
                perror("Error opening/creating destination file");
                fclose(srcFile);
                exit(1);
            }
        }
        char buffer[1024];				// initialize buffer
        size_t bytesRead;

 											
        while ((bytesRead = fread(buffer, 1, sizeof(buffer), srcFile)) > 0)	{		// copy from source to destination

            fwrite(buffer, 1, bytesRead, destFile);
        }

        fclose(srcFile);
        fclose(destFile);
    }
}

void copyToDir(const char *sourceName, const char *destinationDir)
{
    							
    DIR *srcDir = opendir(sourceName);				// source directory
    if (srcDir == NULL)		{
        perror("Error opening source directory");
        return;
    }

    struct dirent *entry;

   						
    while ((entry = readdir(srcDir)))	{			// read source directory

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)	{	//skip dot entries

            continue;
        }

        char srcPath[strlen(sourceName) + strlen(entry->d_name) + 2];		// path lengths
        char destPath[strlen(destinationDir) + strlen(entry->d_name) + 2];

        snprintf(srcPath, sizeof(srcPath), "%s/%s", sourceName, entry->d_name);			//redirect to buffer
        snprintf(destPath, sizeof(destPath), "%s/%s", destinationDir, entry->d_name);

        struct stat statbuf;
        if (stat(srcPath, &statbuf) == -1)	{

            perror("Could not get file information");			// throw error
            continue;
        }

        if (S_ISDIR(statbuf.st_mode))	{	// recursively copy 
						
          	
            copyToDir(srcPath, destPath);
        }
        else if (S_ISREG(statbuf.st_mode))	{	// copy files to destination 

            						
            copyFile(srcPath, destPath);
        }
    }

    closedir(srcDir);
}
