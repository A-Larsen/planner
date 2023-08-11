#ifndef __CLI__
#define __CLI__

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#include "database.h"
#include "date.h"

#define HISTORYLEN 4
#define ARGSMAX 8
#define SCREENWIDTH 50
#define SCREENHEIGHT l0

extern int boxHeight;
extern int boxWidth;
extern int selection;

enum MODE {
	NORMAL,
	INSERT
};

enum MONTHS {
	JANUARY, FEBRUARY, MARCH,
	APRIL,   MAY,      JUNE,
	JULY,    AUGUST,   SEPTEMBER,
	OCTOBER, NOVEMBER, DECEMBER
};
enum WEEKDAYS {
	SUNDAY,
	MONDAY, TUESDAY, WEDNESDAY,
	THURSDAY, FRIDAY, SATURDAY,
};

/* struct Date { */
/* 	unsigned short year; */
/* 	unsigned short month; */
/* 	unsigned short day; */
/* }; */

struct Task {
	unsigned short priority;
	char shortDescription[128];
	char notepath[128];
	struct Date startDate;
	struct Date endDate;
};

struct GConfig{
	int screenrows;
	int screencols;
};

static char daytab[12] = {
   /*JANUARY  FEBRUARY  MARCH*/
	 31,      28,       31, 
   /*APRIL    MAY       JUNE*/
	 30,      31,       30, 
   /*JULY     AUGUST    SEPTEMBER*/
	 31,      31,       30, 
   /*OCTOBER  NOVEMBER DECEMBER*/
	 31,      30,       31
};

extern struct GConfig E;
/* extern struct Date today; */
/* extern struct Date current; */
extern enum MODE mode;

void cli_init();
void cli_clear();
void cli_enableRawMode();
void cli_keyHandle();
void cli_write(const char *fmt, ...);
void cli_free();
void cli_clearBackground();
int getWindowSize(int *rows, int *cols);
void header(int x, int y, char *date);
void weekdayHeaders(short offset);
void dayboxes();
void setSize(int w, int h);
void monthTitle(enum MONTHS month);
void autoSize();
void descriptionBox();
void taskBox();
void popupDescription();
void statusBar();
void initializeYear(int year);
int readKey();

#endif
