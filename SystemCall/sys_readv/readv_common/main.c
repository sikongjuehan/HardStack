/*
 * sys_readv in C
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
/* __NR_open/__NR_readv */
#include <asm/unistd.h>
/* syscall() */
#include <unistd.h>
#include <sys/uio.h>

/* Architecture defined */
#ifndef __NR_readv
#define __NR_readv	145
#endif

static void usage(const char *program_name)
{
	printf("BiscuitOS: sys_readv helper\n");
	printf("Usage:\n");
	printf("      %s\n", program_name);
	printf("\ne.g:\n");
	printf("%s\n\n", program_name);
}

int main(int argc, char *argv[])
{
	int c, hflags = 0;
	struct iovec iov;
	char buffer[128];
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
	iov.iov_base = buffer;
	iov.iov_len  = 128;

	/*
	 * sys_readv
	 *
	 *    SYSCALL_DEFINE3(readv,
	 *                    unsigned long, fd,
	 *                    const struct iovec __user *, vec,
	 *                    unsigned long, vlen) 
	 */
	syscall(__NR_readv, 0, &iov, 1);
	printf("Read from stdin: %s\n", buffer);

	return 0;
}
