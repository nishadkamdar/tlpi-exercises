#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "rwv.h"

#define TRUE 1
#define FALSE 0
#define STR_SIZE 100

void print_usage (void);
void print_version (void);
int readv_new (int fd, const struct rwvec *rwv, int rwvcnt);
void *xcalloc (int nr_elements, int size_per_element);


int main (int argc, char *argv[])
{
	struct rwvec rwv[3];
	struct stat myStruct;
	int x;
	char str[STR_SIZE];
	size_t numRead, totRequired, totRead = 0;
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

	totRequired = 0;

	rwv[0].rwv_base = &myStruct;
	rwv[0].rwv_len = sizeof (struct stat);
	totRequired += rwv[0].rwv_len;

	rwv[1].rwv_base = &x;
	rwv[1].rwv_len = sizeof (x);
	totRequired += rwv[1].rwv_len;

	rwv[2].rwv_base = str;
	rwv[2].rwv_len = STR_SIZE;
	totRequired += rwv[2].rwv_len;		 

	totRead = readv_new (oldfd, rwv, 3);

	if (totRead < totRequired)
	{
		printf ("less bytes read than required\n");
	}
	
	printf ("Total bytes read = %ld\nTotal bytes required = %ld\n", totRead, totRequired);
		
	close (oldfd);
	exit (EXIT_SUCCESS);	
}
	
int readv_new (int fd, const struct rwvec *rwv, int rwvcnt)
{
	int totRead = 0;
	int temp_size;
	int i, ret;

	for (i = 0; i < rwvcnt; i++)
	{
		totRead += read (fd, rwv[i].rwv_base, rwv[i].rwv_len);
	}
	return totRead;
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
