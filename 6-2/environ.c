#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

extern char **environ;

void print_usage (void);
void print_version (void);
void *xcalloc (int nr_elements, int size_per_element);

int main (int argc, char *argv[])
{

	int e_len, ret;
	char *e_buf;

	static struct option opts[] = 
	{
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{"env", required_argument, 0, 'e'},
		{0,0,0,0}
	};

	if (argc == 1)
	{
		print_usage ();
		exit (EXIT_FAILURE);
	}
	while ((ret = getopt_long (argc, argv, "hve:", opts, NULL)) != -1 )
	{
		switch (ret)
		{
			case 'h':	
				print_usage ();
				exit (EXIT_SUCCESS);
				break;
			case 'v':
				print_version ();
				exit (EXIT_SUCCESS);
				break;
			case 'e':	
				e_len = strlen (optarg);
				e_buf = (char *) xcalloc (1, e_len+1);
				strncpy (e_buf, optarg, e_len);
				check_env (e_buf, e_len);
				printenv_new ();
				break;
			default:
				print_usage ();
				exit (EXIT_FAILURE);
		}
	}

	free (e_buf);
	exit (EXIT_SUCCESS);
}

printenv_new (void)
{
	char **ep;

	for (ep = environ; *ep != NULL; ep++)
		puts (*ep);
}

setenv_new (char *name, char *value, int ov)
{
	char *env_var;
	char *n_buf, *v_buf;
	int n_len, v_len;
	char *ret;
		
	n_len = strlen (name);
	v_len = strlen (value);

	n_buf = (char *)xcalloc (1, n_len+1);
	v_buf = (char *)xcalloc (1, v_len+1);

	strncpy (n_buf, name, n_len);
	strncpy (v_buf, value, v_len);

	env_var = xcalloc (1, n_len+v_len+2);
	strncpy (env_var, n_buf, n_len);
	env_var[n_len] = '=';
	strncpy (&env_var[n_len+1], v_buf, v_len);

	printf ("env_var : %s\n", env_var);
		
	if (ov == 0)
	{	
		if ((ret = getenv (n_buf)) == NULL)
		{
			putenv (env_var);
			printf ("setting environment\n");
		}
		else
		{
			printf ("env exists: cannot overwrite\n");
			return 0;
		}	
	}
	else 
	{
		putenv (env_var);
	}
	
	return 0;	
}

check_env (char *e_buf, int len)
{
	const char s[2] = "=";
	char *name, *value;
		
	name = strtok (e_buf, s);
	value = strtok (NULL, s);

	printf ("name: %s\n", name);
	printf ("value: %s\n", value);

	setenv_new (name, value, 1);
}

void print_usage (void)
{
	printf ("./environ --env/-e [NAME=VALUE] \n --help/-h prints usage \n --version/-v prints version\n");
}

void print_version (void)
{
	printf ("0.1\n");
}

void *xcalloc (int nr, int spe)
{
	char *ptr;
	ptr = calloc (nr, spe);
	if (ptr == NULL)
	{
		printf ("fatal : unable to allocate memory\n");
		exit (EXIT_FAILURE);
	}
	
	return ptr;	
}
