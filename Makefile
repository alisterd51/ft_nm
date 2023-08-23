all:
BUILD	:= debug

build_dir := ${BUILD}

srcs_dir := srcs

BIN	:= ft_nm

exes	:= ${BIN} ${BIN2} ${BIN3}

SRCS	:= main.c

OBJS	:= ${SRCS:.c=.o}

DEPS	:= ${OBJS:.o=.d}

-include ${addprefix ${build_dir}/, ${DEPS}}

CC	:=	clang

CFLAGS.debug	:= -O0 -g3 -fsanitize=address -fsanitize=undefined -fsanitize=leak
CFLAGS.release	:=
CFLAGS	:= -Wall -Wextra -Werror ${CFLAGS.${BUILD}}

LDLIBS	:= -lc

LDFLAGS.debug	:= -g3 -fsanitize=address -fsanitize=undefined -fsanitize=leak
LDFLAGS.release	:= 
LDFLAGS	:= ${LDFLAGS.${BUILD}}

COMPILE	= ${CC} -MD -MP ${CFLAGS} -c $< -o $@
LINK	= ${CC} ${LDFLAGS} ${filter-out Makefile, $^} ${LDLIBS} -o $@

all: ${exes:%=${build_dir}/%}

${build_dir}:
	mkdir $@

${build_dir}/%.o: ${srcs_dir}/%.c Makefile | ${build_dir}
	${strip ${COMPILE}}

${build_dir}/${BIN}: ${addprefix ${build_dir}/, ${OBJS}} Makefile | ${build_dir}
	${strip ${LINK}}

${BIN}: ${build_dir}/${BIN}

clean:
	rm -f ${addprefix ${build_dir}/, ${OBJS}}
	rm -f ${addprefix ${build_dir}/, ${DEPS}}

fclean:
	rm -rf ${build_dir}

re: fclean all

.PHONY: all clean fclean re ${BIN}
