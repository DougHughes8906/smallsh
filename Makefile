


OBJS = shellDriver.o smallsh.o

SRCS = shellDriver.c smallsh.c

HEADERS = smallsh.h

smallsh: ${OBJS} ${HEADERS}
	gcc ${OBJS} -o smallsh

${OBJS}: ${SRCS}
	gcc -std=c99 -c $(@:.o=.c)

clean:
	rm *.o smallsh
