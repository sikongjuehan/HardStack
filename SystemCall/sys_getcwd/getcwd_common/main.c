/*
 * sys_getcwd in C
 *
 * (C) 2020.03.11 BuddyZhang1 <buddy.zhang@aliyun.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <getopt.h>
/* __NR_getcwd */
#include <asm/unistd.h>
/* syscall() */
#include <unistd.h>

/* Architecture defined */
#ifndef __NR_getcwd
#define __NR_getcwd	183
#endif

static void usage(const char *program_name)
{
	printf("BiscuitOS: sys_getcwd helper\n");
	printf("Usage:\n");
	printf("      %s\n", program_name);
	printf("\n");
	printf("\ne.g:\n");
	printf("%s\n\n", program_name);
}

int main(int argc, char *argv[])
{
	int c, hflags = 0;
	char pwd[128];
	opterr = 0;

	/* options */
	const char *short_opts = "h";
	const struct option long_opts[] = {
		{ "help", no_argument, NULL, 'h'},
		{ 0, 0, 0, 0 }
	};

	while ((c = getopt_long(argc, argv, short_opts, 
						long_opts, NULL)) != -1) {
		switch (c) {
		case 'h':
			hflags = 1;
			break;
		default:
			abort();
		}
	}

	if (hflags) {
		usage(argv[0]);
		return 0;
	}

	/*
	 * sys_getcwd
	 *
	 *    SYSCALL_DEFINE2(getcwd,
	 *                    char __user *, buf,
	 *                    unsigned long, size)
	 */
	syscall(__NR_getcwd, pwd, 80);
	printf("Current dirent: %s\n", pwd);
	return 0;
}
