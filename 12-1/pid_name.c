#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <pwd.h>
#include <ctype.h>
#include <grp.h>
#include <dirent.h>

#define TRUE 1
#define FALSE 0

void print_usage (void);
void print_version (void);
void *xcalloc (int nr_elemnets, int size_per_element);
uid_t userIdFromName(const char *name);
void print_pid_cmd_user(uid_t u);

int main (int argc, char *argv[])
{
	char *u_buf;
	int ret; 
	int u_len;
	struct passwd *pwd;
	uid_t u;

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

	u = userIdFromName(u_buf);
	printf ("uid of user %s is %u\n", u_buf, u);

	print_pid_cmd_user(u);

	free (u_buf);
	exit (EXIT_SUCCESS);	
	
}

void print_pid_cmd_user(uid_t u)
{
	DIR *dirp;
	struct dirent *dp;
	char *c;
	FILE *fp;
	char line[250];
	char **pid_cmd_uid;
	char *uid_str;
	int i = 0;

	uid_str = (char *)xcalloc(32, 1);
	sprintf(uid_str, "%u", u);
	printf("uid_str is %s\n", uid_str);

	dirp = opendir("/proc");
	if (dirp == NULL)
	{
		printf("failed to open /proc dir\n");
		exit(1);
	}

	for(;;)
	{
		errno = 0;
		pid_cmd_uid = (char **) xcalloc(3, sizeof(char *));
		
		
		pid_cmd_uid[0] = (char *)xcalloc(256, 1);
		pid_cmd_uid[1] = (char *)xcalloc(256, 1);
		pid_cmd_uid[2] = (char *)xcalloc(256, 1);

		c = (char *) xcalloc(256, 1);
		
		dp = readdir(dirp);
		if (dp == NULL)
			break;

		if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
			continue;
		
		if (isdigit(dp->d_name[0]))
		{
			c = strcpy(c, "/proc/");
			c = strcat(c, dp->d_name);
			c = strcat(c,"/status");
			
			fp = fopen(c, "r");
			while (fgets(line, 250, fp) != NULL)
			{
				if (strstr(line, "Name") != NULL)
					strcpy(pid_cmd_uid[0], line);
				if (strstr(line, "Uid") != NULL)
					strcpy(pid_cmd_uid[1], line);
				if (strstr(line, "Tgid") != NULL)
					strcpy(pid_cmd_uid[2], line);
			}
				
			if(strstr(pid_cmd_uid[1], uid_str) != NULL)
			{
				printf("%s", pid_cmd_uid[0]);
				printf("%s", pid_cmd_uid[1]);
				printf("%s", pid_cmd_uid[2]);
				printf("\n");
			}
			
			fclose(fp);
		}
		
		free (c);
		free (pid_cmd_uid[0]);
		free (pid_cmd_uid[1]);
		free (pid_cmd_uid[2]);
		free (pid_cmd_uid);
	}

	if (errno != 0)
	{
		printf("failed to read dir\n");
		exit(EXIT_FAILURE);
	}

	if (closedir(dirp) == -1)
	{
		printf("failed to close dir\n");
		exit(EXIT_FAILURE);
	}
}

uid_t userIdFromName(const char *name)
{
	struct passwd *pwd;
	uid_t u;
	char *endptr;

	if (name == NULL || *name =='\0')
		return -1;

	u = strtol(name, &endptr, 10);
	if (*endptr == '\0')
		return u;

	pwd = getpwnam(name);
	if (pwd == NULL)
		return -1;

	return pwd->pw_uid;
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
