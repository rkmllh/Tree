#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define memzero(base, size)\
		memset(base, 0, size)

#define APPEND		"\\*"

//Find directories and files
//@ [target] starting directory
void tree(const char* target);

//Print and die
//@ [process] path to this
void usage(char* process);

//malloc wrapper
//@ [mem_size] size in bytes
void* smart_zmalloc(size_t mem_size);

//Print and die
//@ [what] cause
void fatal(const char* what);

int main(
	int argc,
	char** argv
)
{
	char* target = NULL;

	if (argc != 2)
		usage(argv[0]);

	target = (char*)smart_zmalloc(((strlen(argv[1])) * sizeof(char) + strlen(APPEND) * sizeof(char)) + 1 * sizeof(char));
	memcpy(target, argv[1], strlen(argv[1]) * sizeof(char));
	printf("[+]Searching %s\n\n", target);

	strcat(target, APPEND);

	tree(target);

	free(target);
	exit(0);
}

void tree(const char* target)
{
	HANDLE hFile = NULL;
	WIN32_FIND_DATAA w32fd;
	SYSTEMTIME st;

	char* new_directory = NULL;

	memzero(&w32fd, sizeof(WIN32_FIND_DATAA));
	memzero(&st, sizeof(SYSTEMTIME));

	hFile = FindFirstFileA(target, &w32fd);

	if (INVALID_HANDLE_VALUE != hFile)
	{
		do
		{
			if (strcmp(".", w32fd.cFileName) != 0 && strcmp("..", w32fd.cFileName) != 0)
			{
				FileTimeToSystemTime(&w32fd.ftCreationTime, &st);
				printf("%d\\%.2d\\%.2d  %.2d:%.2d    ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);

				if (w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					new_directory = (char*)smart_zmalloc((strlen(target) * sizeof(char) + strlen("\\") * sizeof(char) + strlen(w32fd.cFileName) * sizeof(char)) + sizeof(char));
					memcpy(new_directory, target, strlen(target) - strlen(APPEND));
					strcat(new_directory, "\\");
					strcat(new_directory, w32fd.cFileName);
					strcat(new_directory, APPEND);
					printf("<DIR>%s%s\n","     ", w32fd.cFileName);
					tree(new_directory);
					free(new_directory);
				}
				else
				{
					printf("%s%s\n", "          ", w32fd.cFileName);
				}
			}
		}while (FindNextFileA(hFile, &w32fd) != 0);
	}

	FindClose(hFile);
}

void* smart_zmalloc(size_t mem_size)
{
	void* mem_block = malloc(mem_size);

	if (mem_block == NULL)
		fatal("malloc");

	memzero(mem_block, mem_size);
	return mem_block;
}

void usage(char* process)
{
	printf("<usage> <%s> <directory>\n", process);
	exit(1);
}

void fatal(const char* what)
{
	printf("[-]fatal in %s\n", what);
	exit(1);
}
