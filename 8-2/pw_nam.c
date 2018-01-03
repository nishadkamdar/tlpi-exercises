#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <pwd.h>

#define TRUE 1
#define FALSE 0

void print_usage (void);
void print_version (void);
void *xcalloc (int nr_elemnets, int size_per_element);
struct passwd *getpwnam_new (char *buf);

int main (int argc, char *argv[])
{
	char *u_buf;
	int ret; 
	int u_len;
	struct passwd *pwd;

	static struct option opts[] = 
	{
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{"name", required_argument, 0, 'n'},
		{0, 0, 0, 0}
	};

	if (argc < 2)
	{
		print_usage();
		exit (EXIT_SUCCESS);
	}

	while ((ret = getopt_long (argc, argv, "hvn:", opts, NULL)) != -1)
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
			case 'n':
				u_len = strlen (optarg);
				u_buf = (char *) xcalloc (1, u_len+1);
				strncpy (u_buf, optarg, u_len);	
				break;
			default	:
				print_usage ();
				exit (EXIT_FAILURE);	
		}
	
	}

	pwd = getpwnam_new (u_buf);
	printf ("pw_name: %s\n pw_passwd: %s\n pw_uid: %ld\n pw_gid: %ld\n pw_gecos: %s\n pw_dir: %s\n pw_shell: %s\n", pwd->pw_name, pwd->pw_passwd,(long) pwd->pw_uid, (long) pwd->pw_gid, pwd->pw_gecos, pwd->pw_dir, pwd->pw_shell);

	free (u_buf);
	exit (EXIT_SUCCESS);	
	
}

struct passwd *getpwnam_new (char *buf)
{
	static struct passwd *pwd;

	while ((pwd = getpwent ()) != NULL)
	{
		if (strcmp (buf, pwd->pw_name) == 0)
		{
			endpwent ();
			return pwd;	
		}
	}
	printf ("user not found\n");	
	endpwent ();
	return NULL;
}
void print_usage ()
{
	printf ("./pw_nam --name/-n [user name] \n --help/-h prints the usage\n --version/-v prints the version\n");
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
