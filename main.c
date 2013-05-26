//
//  main.c
//  myenv
//
//  Created by Moshe Berman on 5/25/13.
//  Copyright (c) 2013 Moshe Berman. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>	//	Used for getting OS Name
#include <limits.h>			//	Used for max line and max args
#include <unistd.h>
#include <grp.h>			//	Used for group ID



extern char **environ;
extern char *etext;
extern char *edata;
extern char *end;


char* myenv(char *name);
size_t locationOfSubtringInString(char *haystack, char *needle);
void printAllEnvironmentVariables();

int main(int argc, const char * argv[])
{
	
	/*	Get the system anem
	 */
	
	struct utsname unameData;
	uname(&unameData);
	
	
	/*	Retrieve and print out environment variables
	 */
	char *homeDirectory = myenv("HOME");
	char *workingDirectory = myenv("PWD");
	char *shell = myenv("SHELL");
	char *systemVersion = unameData.version;
	char *username = myenv("USER");
	
	/*	Print the environment variables
	 */
	
	printf("\n---- Environment Variables ---- \n\n");
	printf("HOME: %s\n", homeDirectory);
	printf("Working Directory: %s\n", workingDirectory);
	printf("SHELL: %s\n", shell);
	printf("OS: %s\n", systemVersion);
	printf("USER: %s\n", username);
	
	/*	Print out the Process data
	 */
	printf("---- Process Data ---- \n\n");
//	printf("etext: %s\n", &etext);
//	printf("edata: %s\n", &edata);
//	printf("end: %s\n", &end);
	
	/*	Execute system calls
	 */
	pid_t processID = getpid();
	pid_t parentProcessID = getppid();
	pid_t userID = getuid();
	struct group groupID = *getgrgid(userID);
	long maxInput = sysconf(_SC_LINE_MAX);
	long maxArgs = sysconf(_SC_ARG_MAX);

	/*	Print System Calls
	 */
	printf("\n---- System Calls ---- \n\n");
	printf("Process ID: %i\n", processID);
	printf("Parent process ID: %i\n", parentProcessID);
	printf("User ID: %i\n", userID);
	printf("Group ID: %i\n", groupID.gr_gid);
	printf("Max length of input: %li\n", maxInput);
	printf("Max size of argv[] and envp[]: %li\n\n", maxArgs);

    return 0;
}

/*
 *	char* myenv(char *name)
 *
 *	Returns the value of a given
 *
 */
char* myenv(char *name)
{
	
	/*	Set up a return value
	 */
	
	char * result = NULL;
	
	/*	As long as there's another evironment
	 *	value, keep iterating.
	 */
	int i;
	for (i = 0; environ[i] != NULL; i++)
	{
		

		/*	Get the next value...
		 */
		
		char *currentEnvironmentVariable = environ[i];
		
		/*	Check if our current value begins with "name".
		 *
		 *	First, find the first equals sign. Next,
		 *	extract the substring before it. Finally,
		 *	compare our extracted substring against our
		 *	key. If it matches, return everything after 
		 *	the equals sign.
		 *
		 */
		
		size_t location = locationOfSubtringInString(currentEnvironmentVariable, "=");
		
		/*	If we haven't got an equals sign, continue 
		 *	to the next environment variable.
		 */
		
		if (location == -1)
		{
			continue;
		}
		
		/*	If we've reached here, there's a key/value pair,
		 *	so get the length of the key and the value, respectively.
		 *
		 *	Then, copy the values over to the appropriate arrays.
		 */
		
		size_t lengthOfKeyWithValue = strlen(currentEnvironmentVariable);
		size_t lengthOfValue = lengthOfKeyWithValue - location;
		size_t lengthOfKey = lengthOfKeyWithValue - lengthOfValue;

		/*	We're leaking memory here, but I'm not sure
		 *	what the right way to release this memory is.
		 *
		 *	According to the man pages, getenv leaks too,
		 *	but that leak is unrelated to this. 
		 */
		char *key = malloc(lengthOfKey * sizeof(char));
		char *value = malloc((lengthOfValue+1) * sizeof(char));

		strncpy(key, currentEnvironmentVariable, location);
		strncpy(value, currentEnvironmentVariable+location+1, lengthOfValue);

		/*	Assign value to result, if we've found the matching key.
		 */
		
		if(strcmp(name, key) == 0)
		{
			result = value;
		}
	}
	
	return result;
}

/*	
 *	size_t locationOfSubtringInString(char *haystack, char *needle)
 *
 *	Takes a substring (the needle) and
 *	searches for it in another string 
 *	(the haystack), then returns the
 *	index of the needle.
 *
 *	Returns -1 if the substring 
 *	is not found.
 */

size_t locationOfSubtringInString(char *haystack, char *needle)
{
	char *temp = strstr(haystack,  needle);
	
	/*	By default, return -1
	 */
	
	size_t loc = -1;
	
	/*	Else, return a valid value. 
	 *	We use pointer arithmetic to 
	 *	turn the substring into an index.
	 */
	
	if (temp != NULL)
	{
		loc = temp - haystack;
	}
	
	return loc;
}

/*	void printAllEnvironmentVariables()
 *
 *	Does what it says on the tin.
 */

void printAllEnvironmentVariables()
{
	int i;
	for (i = 0; environ[i] != NULL; i++) {
		printf("Environ[%i] %s\n", i, environ[i]);
	}
}

