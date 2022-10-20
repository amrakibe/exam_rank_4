/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amrakibe <amrakibe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/13 06:12:00 by amrakibe          #+#    #+#             */
/*   Updated: 2022/10/20 13:04:34 by amrakibe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int G_var;
int print_error(char *error)
{
	int i = -1;
	while (error[++i])
	{
		write(2, &error[i], 1);
	}
	return (1);
}

int _cd(char **av, int i)
{
	if (i != 2)
		return (print_error("error: bad argument \n"));
	if (chdir(av[1]))
		return (print_error("error: cd: cannot change directory") && print_error(av[1]) && print_error("\n"));
	return (1);
}

int _execve(char **av, char **env, int i)
{
	int p[2];
	int pid;
	int pip;

	int r = 0;
	pip = (av[i] && av[i][0] == '|');
	if (pip && pipe(p) == -1)
		return (print_error("error: fatal\n"));
	pid = fork();
	if (pid == 0)
	{
		av[i] = 0;
		if (dup2(G_var, 0) == -1 || close(G_var) == -1)
			return (print_error("error: fatal\n"));
		if(pip && (dup2(p[1], 1) == -1 || close(p[0]) == -1 || close(p[1]) == -1))
			return (print_error("error: fatal\n"));
		
		execve(av[0], av, env);
		return(print_error("error: cannot execut ") && print_error(av[0]) && print_error("\n"));
	}
	if(pip && (dup2(p[0], G_var) == -1 || close(p[0]) == -1 || close(p[1]) == -1))
		return(print_error("error: fatal\n"));
	if(!pip && (dup2(0, G_var) == -1 && waitpid(pid, &r, 0) == -1))
		return(print_error("error: fatal\n"));
	return(r > 0);
}

int main(int ac , char **av, char **env)
{
	int i;
	int r;
	(void)ac;

	i = 0;
	r = 0;
	G_var = dup(0);
	
	while(av[i] && av[++i])
	{
		av = av + i;
		i = 0;
		while(
			(av[i] && av[i][0] != '|' && av[i][0] != ';') ||
			(av[i] && av[i][0] == '|' && av[i][1] != '\0') ||
			(av[i] && av[i][0] == ';' && av[i][1] != '\0') ||
			(av[i] && av[i][0] != ';' && av[i][1] != '\0'))
		i++;
		if(!strcmp(av[0] , "cd"))
			r = _cd(av, i);
		else if(i)
			r = _execve(av, env, i);
	}
	return((dup2(0, G_var) == -1 && print_error("error: fatal\n")) || r);
}
