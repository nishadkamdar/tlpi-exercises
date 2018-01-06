#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

void print_usage (void);
void print_version (void);
int dup_new (int oldfd);
void *xcalloc (int nr_elements, int size_per_element);

int main (int argc, char *argv[])
{
	int fd;
	char *f_buf;
	int oldfd, newfd;
	int ret;
	int f_len;
	int open_flag;
	off_t off_old, off_new;
		
	static struct option opts[] = 
	{
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{"open", required_argument, 0, 'o'},
		{0, 0, 0, 0}
	};

	while ((ret = getopt_long(argc, argv, "hvo:", opts, NULL)) != -1)
	{
		switch ((char) ret)
		{
			case 'h':
				print_usage ();
				exit (EXIT_SUCCESS);
				break;
			case 'v':	
				print_version ();
				exit (EXIT_SUCCESS);
				break;
			case 'o':
				open_flag = TRUE;
				f_len = strlen (optarg);
				f_buf = (char *) xcalloc (1, f_len+1);
				strncpy (f_buf, optarg, f_len);
				break;
			default:
				print_usage ();
				break;	
		}
	}

	if (argc == 1)
	{
		print_usage ();
		exit (EXIT_FAILURE);
	}

	if (!(open_flag == TRUE))
	{
		print_usage ();
		exit (EXIT_FAILURE);			
	}
	
	oldfd = open (f_buf, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if (oldfd < 0)
	{
		fprintf (stderr, "error opening file %s:%s\n", f_buf, strerror (errno));
		exit (EXIT_FAILURE);
	}

	newfd = dup(oldfd);
	if (newfd < 0)
	{
		fprintf (stderr, "error duplicating file %s:%s\n", f_buf, strerror (errno));
		exit (EXIT_FAILURE);
		
	}
	printf ("old fd = %d\n", oldfd);
	printf ("new duplicate fd = %d\n", newfd);
	
	off_old = lseek (oldfd, 0, SEEK_END);
	if (off_old < 0)
	{
		fprintf (stderr, "error seeking file %s:%s\n", f_buf, strerror (errno));
		exit (EXIT_FAILURE);
	}	
	off_new = lseek (newfd, 0, SEEK_CUR);
	if (off_new < 0)
	{
		fprintf (stderr, "error seeking file %s:%s\n", f_buf, strerror (errno));
		exit (EXIT_FAILURE);
	}	
	
	printf ("oldfd file offset = %ld\n", off_old);
	printf ("newfd file offset = %ld\n", off_new);
	
	close (oldfd);
	close (newfd);
	exit (EXIT_SUCCESS);	
}

int dup_new (int oldfd)
{
	int newfd;
	newfd = fcntl (oldfd, F_DUPFD, 0);	
	if (newfd < 0)
	{
		if (errno == EBADF)
		{
			return -1;
		}
	}
	
	return newfd;
}

void *xcalloc (int nr, int spe)
{
	char *ptr;
	ptr = calloc (nr, spe);
	if (ptr == NULL)
	{
		printf ("fatal : not enough memory\n");
		exit (EXIT_FAILURE);
	}
	return ptr;
}

void print_usage (void) 
{
	printf ("./dup_fcntl --open/-o [filename]\n --help/-h will print usage\n --version/-v prints version\n");
}

void print_version (void)
{
	printf ("0.1\n");
}
