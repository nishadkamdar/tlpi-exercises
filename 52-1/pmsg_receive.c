#include <mqueue.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

struct timespec times;

static void usage_error(const char *prog_name)
{
	fprintf(stderr, "Usage: %s [-n] name [-t] timeout\n", prog_name);
	fprintf(stderr, "	-n	Use O_NONBLOCK flag\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	int flags, opt;
	mqd_t mqd;
	unsigned int prio;
	void *buffer;
	struct mq_attr attr;
	ssize_t num_read;
	time_t timeout;

	if (clock_gettime(CLOCK_REALTIME, &times) == -1)
	{
		printf("failed to get get time\n");
		exit(EXIT_FAILURE);
	}	

	flags = O_RDONLY;
	while ((opt = getopt(argc, argv, "nt:")) != -1)
	{
		switch(opt)
		{
			case 'n':
				flags |= O_NONBLOCK;
				break;

			case 't':
				timeout = atoi(optarg);
				times.tv_sec += timeout;
				break;

			default:
				usage_error(argv[0]);
		}
	}

	if (optind >= argc)
		usage_error(argv[0]);

	mqd = mq_open(argv[optind], flags);
	if (mqd == (mqd_t) - 1)
	{
		printf("mq_open error\n");
		exit(EXIT_FAILURE);
	}

	if (mq_getattr(mqd, &attr) == -1)
	{
		printf("mq_getattr error\n");
		exit(EXIT_FAILURE);
	}

	buffer = malloc(attr.mq_msgsize);
	if (buffer == NULL)
	{
		printf("malloc error\n");
		exit(EXIT_FAILURE);
	}
	
	num_read = mq_timedreceive(mqd, buffer, attr.mq_msgsize, &prio, &times);
	if (num_read == -1)
	{
		printf("mq_receive error\n");
		exit(EXIT_FAILURE);
	}

	printf("Read %ld bytes; priority = %u\n", (long)num_read, prio);
	if (write(STDOUT_FILENO, buffer, num_read) == -1)
	{
		printf("write error\n");
		exit(EXIT_FAILURE);
	}
	
	exit(EXIT_SUCCESS);
}
