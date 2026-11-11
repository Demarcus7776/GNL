/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohachad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/08 21:46:27 by mohachad          #+#    #+#             */
/*   Updated: 2026/11/08 21:46:29 by mohachad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

static int	init_stash(char **stash)
{
	if (!*stash)
	{
		*stash = malloc(1);
		if (!*stash)
			return (0);
		(*stash)[0] = '\0';
	}
	return (1);
}

static void	clear_stash(char **stash, char *buf)
{
	free(buf);
	free(*stash);
	*stash = NULL;
}

static int	read_stash(int fd, char *buf, char **stash)
{
	ssize_t	count;
	char	*tmp;

	count = read(fd, buf, BUFFER_SIZE);
	while (count > 0)
	{
		buf[count] = '\0';
		tmp = ft_strjoin(*stash, buf);
		if (!tmp)
			return (-1);
		free(*stash);
		*stash = tmp;
		if (ft_strchr(*stash, '\n'))
			break ;
		count = read(fd, buf, BUFFER_SIZE);
	}
	if (count == -1)
		return (-1);
	return (1);
}

static char	*get_line(char **stash)
{
	char	*line;

	line = extract(*stash);
	if (!line)
		return (NULL);
	*stash = update(*stash);
	return (line);
}

char	*get_next_line(int fd)
{
	char		*buf;
	static char	*stash[1024];
	char		*line;

	if (fd < 0 || fd >= 1024 || BUFFER_SIZE <= 0)
		return (NULL);
	buf = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!buf)
		return (NULL);
	if (!init_stash(&stash[fd]))
		return (free(buf), NULL);
	if (read_stash(fd, buf, &stash[fd]) == -1)
		return (clear_stash(&stash[fd], buf), NULL);
	if (!stash[fd] || !stash[fd][0])
		return (clear_stash(&stash[fd], buf), NULL);
	line = get_line(&stash[fd]);
	free(buf);
	return (line);
}
