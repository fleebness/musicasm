#if !defined CONSOLE_WAIT_HPP
#define CONSOLE_WAIT_HPP

#if defined _MSC_VER
#include <conio.h>
#endif // _MSC_VER
#if defined TVR_POSIX
		// From:
		// http://stackoverflow.com/questions/421860/capture-characters-from-standard-input-without-waiting-for-enter-to-be-pressed
#include <stdio.h>
#include <sys/ioctl.h> // For FIONREAD
#include <termios.h>
#include <stdbool.h>
inline int _kbhit()
{
	static bool initflag = false;
	static const int STDIN = 0;

	if (!initflag) {
		// Use termios to turn off line buffering
		struct termios term;
		tcgetattr(STDIN, &term);
		term.c_lflag &= ~ICANON;
		tcsetattr(STDIN, TCSANOW, &term);
		setbuf(stdin, 0);
		initflag = true;
	}

	int nbbytes;
	ioctl(STDIN, FIONREAD, &nbbytes);  // 0 is STDIN
	return nbbytes;
}
#define _getch getchar
#endif // TVR_POSIX

#endif // CONSOLE_WAIT_HPP

