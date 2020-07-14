


OBJS = shellDriver.o smallsh.o

SRCS = shellDriver.c smallsh.c

HEADERS = smallsh.h

shellDriver: ${OBJS} ${HEADERS}
	gcc ${OBJS} -o shellDriver

${OBJS}: ${SRCS}
	gcc -std=c99 -c $(@:.o=.c)

clean:
	rm *.o shellDriver
