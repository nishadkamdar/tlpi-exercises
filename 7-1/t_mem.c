#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>

#define MAXALLOC 100000
#define TRUE 1
#define FALSE 0

void print_usage (void);
void print_version (void);
void *xcalloc (int nr_elemnets, int size_per_element);

int main (int argc, char *argv[])
{
	char *ptr[MAXALLOC];
	int ret; 
	int s_block = 0, nr_block = 0, free_step = 0, max_step = 0, min_step = 0;
	int step_flag = FALSE, max_flag = FALSE, min_flag = FALSE;
	char *curr_brk;	
	int i;

	static struct option opts[] = 
	{
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{"size", required_argument, 0, 's'},
		{"num", required_argument, 0 , 'n'},
		{"step", required_argument, 0, 'p'},
		{"min", required_argument, 0, 'm'},
		{"max", required_argument, 0, 'x'},
		{0, 0, 0, 0}
	};

	if (argc < 3)
	{
		print_usage();
		exit (EXIT_SUCCESS);
	}

	while ((ret = getopt_long (argc, argv, "hvs:n:p:m:x:", opts, NULL)) != -1)
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
			case 's':
				s_block = atoi (optarg);
				break;
			case 'n':
				nr_block = atoi (optarg);
				break;
			case 'p':
				step_flag = TRUE;	
				free_step = atoi (optarg);
				break;
			case 'm':	
				min_flag = TRUE;
				min_step = atoi (optarg);	
				break;
			case 'x':
				max_flag = TRUE;
				max_step = atoi (optarg);
				break;
			default	:
				print_usage ();
				exit (EXIT_FAILURE);	
		}
	
	}
	if (step_flag == FALSE)
		free_step = 1;
	if (min_flag == FALSE)
		min_step = 0;
	if (max_flag == FALSE)
		max_step = nr_block;
	
	for (i = 0; i < nr_block; i++)
	{
		curr_brk = sbrk (0);
		printf ("The current break is at %p\n", curr_brk); 	
	
		ptr [i] = malloc (s_block);
		if (ptr [i] == NULL)
		{
			printf ("fatal : out of memory\n");
			exit (EXIT_FAILURE);
		}
	}
	
	for (i = min_step; i < max_step; i += free_step)
	{
		curr_brk = sbrk (0);
		printf ("The current break is at %p\n", curr_brk); 	
	
		free (ptr [i]);
	}	
	
}

void print_usage ()
{
	printf ("./t_mem --size/-s [size per block] --num/-n [number of blocks] --step/-s [interval of freeing] --min/-m [minimum freeing interval] --max/-x [max freeing interval]\n --help/-h prints the usage\n --version/-v prints the version\n");
}

void print_version ()
{
	printf ("0.1\n");
}

void *xcalloc (int ne, int spe)
{
	char *ptr;
	
	ptr = calloc (ne, spe);
	if (ptr == NULL)
	{
		printf ("fatal: insufficient memory\n");
		exit (EXIT_FAILURE);
	}
	return ptr;	
}
