/*
 * sys_uselib in C
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
/* __NR_uselib */
#include <asm/unistd.h>
/* syscall() */
#include <unistd.h>

/* Architecture defined */
#ifndef __NR_uselib
#define __NR_uselib	86
#endif

static void usage(const char *program_name)
{
	printf("BiscuitOS: sys_uselib helper\n");
	printf("Usage:\n");
	printf("      %s <-p pathname>\n", program_name);
	printf("\n");
	printf("\t-p\t--path\tThe path for share library.\n");
	printf("\ne.g:\n");
	printf("%s -p /lib/ld-2.25.so\n\n", program_name);
}

int main(int argc, char *argv[])
{
	char *path = NULL;
	int c, hflags = 0;
	opterr = 0;

	/* options */
	const char *short_opts = "hp:";
	const struct option long_opts[] = {
		{ "help", no_argument, NULL, 'h'},
		{ "path", required_argument, NULL, 'p'},
		{ 0, 0, 0, 0 }
	};

	while ((c = getopt_long(argc, argv, short_opts, 
						long_opts, NULL)) != -1) {
		switch (c) {
		case 'h':
			hflags = 1;
			break;
		case 'p': /* Path */
			path = optarg;
			break;
		default:
			abort();
		}
	}

	if (hflags || !path) {
		usage(argv[0]);
		return 0;
	}

	/*
	 * sys_uselib
	 *
	 *    SYSCALL_DEFINE1(uselib,
	 *                    const char __user *, library)
	 */
	syscall(__NR_uselib, path);
	return 0;
}
