/*
 * sys_getresuid16 in C
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
/* __NR_getresuid16 */
#include <asm/unistd.h>
/* syscall() */
#include <unistd.h>

/* Architecture defined */
#ifndef __NR_getresuid16
#define __NR_getresuid16	165
#endif

typedef unsigned int old_uid_t;

static void usage(const char *program_name)
{
	printf("BiscuitOS: sys_getresuid16 helper\n");
	printf("Usage:\n");
	printf("      %s\n", program_name);
	printf("\ne.g:\n");
	printf("%s\n\n", program_name);
}

int main(int argc, char *argv[])
{
	old_uid_t ruid, euid, suid;
	int c, hflags = 0;
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
	 * sys_getresuid16
	 *
	 *    SYSCALL_DEFINE3(getresuid16,
	 *                    old_uid_t __user *, ruidp,
	 *                    old_uid_t __user *, euidp,
	 *                    old_uid_t __user *, suidp)
	 */
	syscall(__NR_getresuid16, &ruid, &euid, &suid);
	printf("RUID: %d\nEUID: %d\nSUID: %d\n", ruid, euid, suid);

	return 0;
}
