#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

void	puterr(char *str)
{
	while (*str)
	{
		write(2, str, 1);
		str++;
	}
}

void	pipe_set(int is, int *fd, int g)
{
	if (is && (dup2(fd[g], g) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
		puterr("fatal\n"), exit(1);
}

int	mycd(char **av, int i)
{
	if (i != 2)
		return puterr("too many args\n"), 1;
	if (chdir(*(av + 1)) == -1)
		return (puterr("cant cd\n"), 1);
	return (0);
}

int	myexec(char **av, int i, char **env)
{
	int	ispipe, fd[2], status, pid;

	ispipe = *(av + i) && !strcmp(*(av + i), "|");
	if (!ispipe && !strcmp(*av, "cd"))
		return (mycd(av, i));
	if (ispipe && pipe(fd) == -1)
		puterr("fatal\n"), exit(1);
	if ((pid = fork()) == -1)
		puterr("fatal\n"), exit(1);
	if (!pid)
	{
		*(av + i) = 0;
		pipe_set(ispipe, fd, 1);
		if (!strcmp(*av, "cd"))
			exit(mycd(av, i));
		printf("%s\n", *av);
		execve(*av, av, env);
		puterr("cant exec\n");
		exit(1);
	}
	waitpid(pid, &status, 0);
	pipe_set(ispipe, fd, 0);
	return (WIFEXITED(status) && WEXITSTATUS(status));
}

int	main(int ac, char **av, char **env)
{
	int	status, i;

	if (ac == 1)
		return (0);
	status = 0;
	i = 0;
	while (*(av + i))
	{
		av = av + i + 1;
		i = 0;
		while (*(av + i) && strcmp(*(av + i), "|") && strcmp(*(av + i), ";"))
			i++;
		status = myexec(av, i, env);
	}
	return (status);
}
