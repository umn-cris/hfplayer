// ===================================================================== //
// Copyright (C) 2011 Benjamin Segovia                                   //
//                                                                       //
// Licensed under the Apache License, Version 2.0 (the "License");       //
// you may not use this file except in compliance with the License.      //
// You may obtain a copy of the License at                               //
//                                                                       //
//     http://www.apache.org/licenses/LICENSE-2.0                        //
//                                                                       //
// Unless required by applicable law or agreed to in writing, software   //
// distributed under the License is distributed on an "AS IS" BASIS,     //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express          //
// or implied.                                                           //
// See the License for the specific language governing permissions and   //
// limitations under the License.                                        //
// ===================================================================== //

/* Use "g++ -std=c++0x -laio -Wall -O2 -g aio.cpp -o aio" to build it */

#include <unistd.h>
#include <fcntl.h>
#include <libaio.h>
#include <errno.h>
#include <cassert>
#include <cstdlib>
#include <cstdio>

#define FATAL(...)\
	do {\
		fprintf(stderr, __VA_ARGS__);\
		fprintf(stderr, "\n");\
		assert(0);\
		exit(-1);\
	} while (0)

static const void handle_error(int err)
{
#define DECL_ERR(X) case -X: FATAL("Error "#X"\n"); break;

	switch(err) {
			DECL_ERR(EFAULT);
			DECL_ERR(EINVAL);
			DECL_ERR(ENOSYS);
			DECL_ERR(EAGAIN);
	};

	if(err < 0) FATAL("Unknown error");

#undef DECL_ERR
}

#define IO_RUN(F, ...)\
	do {\
		int err = F(__VA_ARGS__);\
		handle_error(err);\
	} while (0)

#define MB(X) (X * 1024 * 1024)
//#define SZ MB(1)
#define SZ 1

static const int maxEvents = 10;
char *dst = NULL;   // data we are reading
char *src = NULL;   // data we are writing
int fd = -1;        // file to open

void check(io_context_t ctx, struct iocb *iocb, long res, long res2)
{
	if(res2 || res != SZ) FATAL("Error in async IO");

	for(size_t i = 0; i < SZ; ++i)
		if(dst[i] != src[i]) FATAL("Error in async copy");

	printf("DONE\n");
	fflush(stdout);
}

int
main(int argc, char *argv[])
{
	/* Create a file and fill it with random crap */
	FILE *file = fopen("crap.dat", "wb");

	if(file == NULL) FATAL("Unable to create crap.dat");

	src = new char[SZ];

	for(size_t i = 0; i < SZ; ++i) src[i] = rand();

	size_t nr = fwrite(src, SZ, 1, file);

	if(nr != 1) FATAL("Unable to fill crap.dat");

	fclose(file);
	/* Prepare the file to read */
 	int fd = open("/dev/sdi", O_DIRECT , 0);
//	int fd = open("crap.dat", O_NONBLOCK, 0);

	if(fd < 0) FATAL("Error opening file");

	dst = new char[SZ];
	/* Now use *real* asynchronous IO to read back the file */
	io_context_t ctx;
	memset(&ctx, 0, sizeof(ctx));
	IO_RUN(io_queue_init, maxEvents, &ctx);
	/* This is the read job we asynchronously run */
	iocb *job = (iocb*) new iocb[1];
	io_prep_pread(job, fd, dst, SZ, 0);
	io_set_callback(job, check);
	/* Issue it now */
	IO_RUN(io_submit, ctx, 1, &job);
	IO_RUN(io_submit, ctx, 1, &job);
	/* Wait for it */
	io_event evt;
	IO_RUN(io_getevents, ctx, 1, 1, &evt, NULL);
	check(ctx, evt.obj, evt.res, evt.res2);
	close(fd);
	delete [] src;
	delete [] dst;
	io_destroy(ctx);
	return 0;
}


