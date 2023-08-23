#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int test(const int fd) {
	if (fd < 0) {
		return (1);
	}
	return (0);
}

void	ft_error(const char *pathnm, const char *pathbin) {
	char	str[50000];

	if (errno == ENAMETOOLONG) {
		sprintf(str, "%s: Warning: could not locate \'%s\'.  reason", pathnm, pathbin);
	}
	else {
		sprintf(str, "%s: \'%s\'", pathnm, pathbin);
	}
	perror(str);
}

void	print_sym(int fd) {
	struct stat	statbuf = {0};
	int	ret_fstat;

	ret_fstat = fstat(fd, &statbuf);
	printf("%lu\n", statbuf.st_dev);
}

int	test2(const char *pathnm, const char *pathbin, int ac) {
	int fd = open(pathbin, 0);
	int ret_value = 0;

	if (test(fd) == 0) {
		if (ac > 2) {
			printf("\n%s:\n", pathbin);
		}
		print_sym(fd);
	}
	else {
		ft_error(pathnm, pathbin);
		ret_value = 1;
	}
	if (fd >= 0)
		close(fd);
	return (ret_value);
}

int	main(int ac, char **av) {
	int ret_value = 0;

	if (ac > 1) {
		for (int i = 1; i < ac; i++)
			if (test2(av[0], av[i], ac) != 0)
				ret_value = 1;
	}
	else {
		if (test2(av[0], "a.out", ac) != 0)
			ret_value = 1;
	}
	return (ret_value);
}
