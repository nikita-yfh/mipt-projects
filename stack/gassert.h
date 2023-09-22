#ifndef NDEBUG

#ifdef _ASSERT_H
#undef assert
#endif

#define assert(expr)                                                            \
    {                                                                           \
        if(!(expr))                                                             \
            _gassertFailed(#expr, __FILE__, __LINE__, __PRETTY_FUNCTION__);     \
    }


#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <stdio.h>

static struct termios old, current;

char _mygetch() {
  tcgetattr(0, &old); /* grab old terminal i/o settings */
  current = old; /* make new settings same as old settings */
  current.c_lflag &= ~ICANON; /* disable buffered i/o */
  current.c_lflag &= ~ECHO; /* set no echo mode */
  tcsetattr(0, TCSANOW, &current); /* use these new terminal i/o settings now */
  char ch = getchar();
  tcsetattr(0, TCSANOW, &old);
  return ch;
}

void _runGDB() {
	//int pipe
}


void _gassertFailed(const char *expr, const char *file, int line, const char *func) {
	fprintf(stderr, "%s:%d: %s: Assertion `%s` failed\n", file, line, func, expr);
	printf("Press `g` to run GDB, another key to exit: ");

	if(_mygetch() == 'g')
		_runGDB();
	exit(1);
}


#endif
