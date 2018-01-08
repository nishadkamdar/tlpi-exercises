#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <limits.h>

#define TRUE 1
#define FALSE 0

void print_usage (void);
void print_version (void);
void *xcalloc (int nr_elemnets, int size_per_element);
int initgroups_new (const char* user, gid_t group);

int main (int argc, char *argv[])
{
	char *u_buf;
	int ret; 
	int u_len;
	struct passwd *pwd;
	gid_t pwd_gid;

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
	
	pwd = getpwnam (u_buf);
	if (pwd == NULL)
	{
		if (errno == 0)
			printf ("user not found\n");
		else
		{
			printf ("error retrieving user\n");
			exit (EXIT_FAILURE);
		}		
	}
	pwd_gid = pwd->pw_gid;
	printf ("gid obtained from passwd file is: %ld\n", (long) pwd_gid);	
	
	ret = initgroups_new (u_buf, pwd_gid);
	printf ("pw_name: %s\n pw_passwd: %s\n pw_uid: %ld\n pw_gid: %ld\n pw_gecos: %s\n pw_dir: %s\n pw_shell: %s\n", pwd->pw_name, pwd->pw_passwd,(long) pwd->pw_uid, (long) pwd->pw_gid, pwd->pw_gecos, pwd->pw_dir, pwd->pw_shell);

	free (u_buf);
	exit (EXIT_SUCCESS);	
	
}

int initgroups_new (const char *buf, gid_t pwd_gid)
{
	static struct group *grp;
	static gid_t grouplist[NGROUPS_MAX + 1];
	gid_t gid_tmp;
	int i = 0, j = 0;
	int ret;
	int gid_cnt= 0;

	while ((grp = getgrent ()) != NULL)
	{
		gid_tmp = grp->gr_gid;
		printf ("gid-tmp is %d\n", gid_tmp);
	        /*for (i = 0; (grp->gr_mem)[i]; i++)
		{
			printf ("%s", (grp->gr_mem)[i]);
		
		}*/
		printf ("\n");
	   	for (i = 0; (grp->gr_mem)[i]; i++)
		{
	//	printf ("user: %s\n", *(grp->gr_mem));
			if (strcmp (buf, (*(grp->gr_mem)) + i) == 0)
			{
				grouplist[j] = gid_tmp;
				j++;
				printf ("user nishad found in %d groups\n", j);
				break;
			}
		}
	}
	grouplist[j] = pwd_gid;
	printf ("gid_cnt = %d\n", j+1);
	printf ("[GID_LIST from /etc/group]\n");
	for (i = 0; i < j+1; i++)
	{
		printf("GID: %ld\n",(long)grouplist[i]);
	}	
	
	ret = setgroups (j+1, grouplist);
	if (ret == -1)
	{
		printf ("unable to set groups\n");
		return -1;
	}	

	gid_cnt = getgroups (j+1, grouplist);
	printf ("supp_gid_cnt = %d\n", gid_cnt);
	printf ("[SUPP_GID_LIST]\n");
	for (i = 0; i < j+1; i++)
	{
		printf("GID: %ld\n",(long)grouplist[i]);
	}	
	
	endgrent ();
	return 0;
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
