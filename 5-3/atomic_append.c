#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define BUFF_SIZE 4096

char buffer[BUFF_SIZE];

void print_usage (void);
void print_version (void);
void perform_write (void);
void *xcalloc (int nr_elements, int size_per_element);

int main (int argc, char *argv[])
{

	int fd;
	int open_flag=FALSE;
	int offset_flag=FALSE;
	int size_flag = FALSE;
	int no_app_flag = FALSE;
	int n_bytes = 0;
	int ret;
	int f_len;
	char *f_str;
				
	static struct option opts [] =
	{
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{"open", required_argument, 0, 'o'},
		{"bytes", required_argument, 0, 'b'},
		{"xclusive", no_argument, 0, 'x'},
		{0, 0, 0, 0}
	};

	while ((ret = getopt_long (argc, argv, "hvo:b:x", opts, NULL)) != -1)
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
				f_str = (char*) xcalloc (1, f_len+1);
				strncpy (f_str, optarg, f_len);
				break;
			case 'b':
				size_flag = TRUE;
				n_bytes = atoll (optarg);
				break;
			case 'x':
				no_app_flag = TRUE;
				break;			
			default:
				print_usage();
				exit (EXIT_FAILURE);
		}
	}
	
	if (argc == 1)
	{
		print_usage ();
		exit (EXIT_FAILURE);	
	}
	
	if (!(open_flag == TRUE && size_flag == TRUE))
	{
		print_usage ();
		exit (EXIT_FAILURE);
	}

	if (no_app_flag == TRUE)
	{	
		fd = open (f_str, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);	
	}
	else
	{
		fd = open (f_str, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);	
	}
	if (fd < 0)
	{
		fprintf (stderr, "error opening file %s:%s\n", f_str, strerror (errno));
		exit (EXIT_FAILURE);
	}
	while (n_bytes > 0)
	{
		if (lseek (fd, 0, SEEK_END) == -1)
		{
			fprintf (stderr, "error seeking file %s:%s\n", f_str, strerror (errno));
			exit (EXIT_FAILURE);	
		}
		if (write (fd, "t", 1) == -1)
		{
			fprintf (stderr, "error writing file %s:%s\n", f_str, strerror (errno));
			exit (EXIT_FAILURE);
		}
		n_bytes--;
	}

	close (fd);
	exit (EXIT_SUCCESS);
}

void print_usage (void)
{
	printf ("./atomic_append --open/-o [file] --bytes/-b [num bytes] --xclusive/x to open with append flag\n --help/-h prints usage\n --version/-v prints version\n");
}	

void print_version (void)
{
	printf ("0.1\n");
}

void *xcalloc (int nr, int spe)
{
	void *ptr;
	
	ptr = calloc (nr, spe);
	if (ptr == NULL)
	{
		printf ("fatal : not sufficient memory\n");
		exit (EXIT_FAILURE);
	}

	return ptr;	
}
