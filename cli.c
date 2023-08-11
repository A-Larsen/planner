#include "colors.h"
#include "cli.h"
#include "text.h"

#define DESCRIPTIONSIZE 500
#define CTRL_KEY(k) ((k) & 0x1f)

typedef unsigned short cal_t;

enum editorKey{
	BACKSPACE = 127,
	ARROW_LEFT = 1000,
	ARROW_RIGHT,
	ARROW_UP,
	ARROW_DOWN,
	DEL_KEY,
	HOME_KEY,
	END_KEY,
	PAGE_UP,
	PAGE_DOWN
};

char insert_text[366][DESCRIPTIONSIZE] = {};

cal_t month_off[12];
cal_t insert_text_pos[366] = {};
cal_t insert_text_size[366] = {};

typedef void (*cmd_t)(int, char **);

/* int topoffset = 6; */
int topoffset = 2;
int year = 0;

const char * 
getMonth(enum MONTHS month){
	switch(month){
		case JANUARY:   return 
"    _\r\n"
" _ | |__ _ _ _ _  _ __ _ _ _ _  _\r\n"
"| || / _` | ' \\ || / _` | '_| || |\r\n"
" \\__/\\__,_|_||_\\_,_\\__,_|_|  \\_, |\r\n"
"                             |__/";

		case FEBRUARY:  return 
" ___    _ \r\n"
"| __|__| |__ _ _ _  _ __ _ _ _ _  _\r\n"
"| _/ -_) '_ \\ '_| || / _` | '_| || |\r\n"
"|_|\\___|_.__/_|  \\_,_\\__,_|_|  \\_, |\r\n"
"                               |__/";

		case MARCH:     return 
" __  __             _ \r\n"
"|  \\/  |__ _ _ _ __| |_\r\n"
"| |\\/| / _` | '_/ _| ' \\\r\n"		
"|_|  |_\\__,_|_| \\__|_||_|";

		case APRIL:     return 
"   _            _ _\r\n"
"  /_\\  _ __ _ _(_) |\r\n"
" / _ \\| '_ \\ '_| | |\r\n"
"/_/ \\_\\ .__/_| |_|_|\r\n"
"      |_|";
		case MAY:       return 
" __  __ \r\n"
"|  \\/  |__ _ _  _ \r\n"
"| |\\/| / _` | || |\r\n"
"|_|  |_\\__,_|\\_, |\r\n"
"             |__/";

		case JUNE:      return 
"    _\r\n"
" _ | |_  _ _ _  ___\r\n"
"| || | || | ' \\/ -_)\r\n"
" \\__/ \\_,_|_||_\\___|";

		case JULY:      return 
"    _      _ \r\n"
" _ | |_  _| |_  _\r\n"
"| || | || | | || |\r\n"
" \\__/ \\_,_|_|\\_, |\r\n"
"             |__/";
		case AUGUST:    return 
"   _                    _\r\n"
"  /_\\ _  _ __ _ _  _ __| |_\r\n"
" / _ \\ || / _` | || (_-<  _|\r\n"
"/_/ \\_\\_,_\\__, |\\_,_/__/\\__|\r\n"
"          |___/";
		case SEPTEMBER: return 
" ___           _             _  \r\n"
"/ __| ___ _ __| |_ ___ _ __ | |__  ___ _ _\r\n"
"\\__ \\/ -_) '_ \\  _/ -_) '  \\| '_ \\/ -_) '_|\r\n"
"|___/\\___| .__/\\__\\___|_|_|_|_.__/\\___|_|\r\n"
"         |_|"; 

		case OCTOBER:   return(
"  ___     _       _ \r\n"
" / _ \\ __| |_ ___| |__  ___ _ _ \r\n"
"| (_) / _|  _/ _ \\ '_ \\/ -_) '_|\r\n"
" \\___/\\__|\\__\\___/_.__/\\___|_|"
);

		case NOVEMBER:  return 
" _  _                   _ \r\n"
"| \\| |_____ _____ _ __ | |__  ___ _ _ \r\n"
"|  ` / _ \\ V / -_) '  \\| '_ \\/ -_) '_|\r\n"
"|_|\\_\\___/\\_/\\___|_|_|_|_.__/\\___|_|";
		case DECEMBER:  return 
" ___                   _  \r\n"
"|   \\ ___ __ ___ _ __ | |__  ___ _ _ \r\n"
"| |) / -_) _/ -_) '  \\| '_ \\/ -_) '_|\r\n"
"|___/\\___\\__\\___|_|_|_|_.__/\\___|_|";
	
	}
}


int 
getCursorPosition(int *rows, 
                  int *cols){
	char buf[32];
	unsigned int i = 0;

	if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
		return -1;

	while (i < sizeof(buf) - 1){
		if (read(STDIN_FILENO, &buf[i], 1) != 1) 
			break; 
		if (buf[i] == 'R') break;
		i++;
	}
	buf[i] = '\0';

	if (buf[0] != '\x1b' || buf[1] != '[') return -1;
	if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;

	return 0;
}

int 
getWindowSize(int *rows, 
              int *cols){

	struct winsize ws;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0){
		if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) 
			return -1;
		return getCursorPosition(rows, cols);	
	}else{
		*cols = ws.ws_col;
		*rows = ws.ws_row;
		return 0;
	}
}
// eval might be a good thing to use // to look a memory in program int 
int getDays(enum MONTHS month, 
            int year)
{
	bool leapyear = (year % 4 == 0) && (year % 100 != 0);
	return (leapyear && (month == FEBRUARY)) ? 29 : daytab[month];
}

int getWeekDay(int day, int month, int year)
{

	return (day += month < 3 ? year-- : 
			year - 2, 23 * month / 9 + day + 4 + year / 4 - year / 100 + year / 400) % 7;
}

void
initializeYear(int year)
{
	int pos = 0;
	// pointer to each start day of every month
	for(int i = 0; i <= 12; i++){
		int days = getDays(i, year);
		month_off[i] = pos;
		for(int j = 0; j < days; j++) pos++;
	}
}

/* void dayboxes(enum MONTHS month, unsigned int year, struct Date today){ */
void 
dayboxes(){

	// use popen to use figlet
	/* weekdayHeaders(0); */

	int month = current.month;
	int year = current.year;
	int days = getDays(current.month, current.year);
	int d = 1;
	month++; // months are not zero indexed in calculation below
	/* int weekday  = (d += month < 3 ? year-- : year - 2, 23*month/9 + d + 4 + year/4- year/100 + year/400)%7; */
	int weekday  = getWeekDay(d, month, year);
	
	// negative days are skiped weekdays1.
	// -1   start month on tuesday
	// -2   start month on wednesday
	// -3   start month on thursday
	// etc.
	int day = -(weekday);
	if (selection < 0){
		selection = 0;
	}
	if (selection >= days){
		selection = days-1;
	}

	for (int j = 0; j < 6; j++){
		for (int i = 0; i < 7; i++){
			int hoffset = topoffset + 7;
			int dec = 0;
			bool selected = day < days && day == selection;
			bool istoday = (today.day == day + 1) && (today.month == month -1) && (today.year == year);

			if (istoday){
				cli_write(CYANBG);
			}

			/* int re = (boxWidth + 2 ) * i + 1; */
			int re = (boxWidth * 1.13 ) * i + 1;

			if (selected) cli_write(GREENBG);

			cli_write("\x1b[%d;%df┌", j * boxHeight + hoffset, re);
			dec = boxWidth;

			while (dec--){
				cli_write("─");
			}
			cli_write("┐\r\n");

			day++;

			if (day <= days && day > 0){
				int numlength = day != 0 ? floor(log10(day)) + 1 : 1;

				int spaces = boxWidth - numlength;

				if (istoday){
					cli_write("\x1b[%d;%df│%d", j * boxHeight + (++hoffset), re, day);
				}else{
					cli_write("\x1b[%d;%df│" DEFAULTFG "%d", j * boxHeight + (++hoffset), re, day);
				}

				if (selected) cli_write(GREENBG);

				while (spaces-- > 0) cli_write(" ");

				cli_write("│\r\n");

			}else{
				cli_write("\x1b[%d;%df│", j * boxHeight + (++hoffset), re);
				dec = boxWidth;

				while (dec--){
					cli_write(" ");
				}
				cli_write("│\r\n");
			}

			dec = boxHeight - 1;
			while (--dec > 1){
				cli_write("\x1b[%d;%df│", j * boxHeight + (++hoffset), re);
				int dec = boxWidth;

				while (dec--){
					cli_write(" ");
				}
				cli_write("│\r\n");

			}

			cli_write("\x1b[%d;%df└", j * boxHeight + (++hoffset), re);
			dec = boxWidth;

			while (dec--){
				cli_write("─");
			}
			cli_write("┘\r\n");
			cli_write(DEFAULTFG);
			cli_write(DEFAULTBG);
		}
	}

}

void
setSize(int w, 
		int h)
{
	int rows, cols;
	getWindowSize(&rows, &cols);

	boxWidth = w < 4 ? 4 : 
		(w + 2) * 7 > cols ? cols / 10: w;

	boxHeight = h < 3 ? 3 : 
		h * 6 > rows ? rows / 6 : h;

}

void
autoSize()
{
	int rows, cols;
	getWindowSize(&rows, &cols);
	// need to round down to nereast integer to prevent wraping
	/* int width = (int)floor(cols / 10); */
	int width = (int)floor(cols / 9.3f);
	int height = (int)floor(rows / 8.0f);
	/* boxWidth = width > 4 ? width + 1: 4; */
	boxWidth = width > 4 ? width: 4;
	boxHeight = height > 3 ? height : 3;
}

void 
cli_clear()
{
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);
}

void
cli_write(const char *fmt, 
          ...)
{
	va_list args;
	va_start(args, fmt);
	char str[10000];
	int len = vsprintf(str, fmt, args);

	write(STDOUT_FILENO, str, len);
	va_end(args);
}

void die(const char *s)
{
	perror(s);
	exit(1);
}

void
header(int x, 
       int y, 
	   char *date){

	int datelen = strlen(date);
	int headerlen = 16;
	int makeup = (boxWidth + 2) - datelen - 2;
	int dec = 0;

	cli_write("\x1b[%d;%df┌", y, x);
	dec = boxWidth;

	while (dec--){
		cli_write("─");
	}
	cli_write("┐\r\n");

	cli_write("\x1b[%d;%df│%s", y + 1, x, date);
	while (makeup--) cli_write(" ");
	cli_write("│\r\n");


	cli_write("\x1b[%d;%df└", y + 2, x);
	dec = boxWidth;

	while (dec--){
		cli_write("─");
	}
	cli_write("┘\r\n");

}

void 
descriptionBox()
{
	int ypos = 6;
	int width = boxWidth * 1.3;
	int height = boxHeight + 1;

	cli_write("\x1b[%d;%df", ypos, boxWidth * 8);

	cli_write("┌");

	for (int dec = width; dec > 0 ; dec--){
		cli_write("─");
	}
	cli_write("┐");

	while (height--){
		cli_write("\x1b[%d;%df", ++ypos, boxWidth * 8);
		cli_write("│");

		for (int dec = width; dec > 0 ; dec--){
			cli_write(" ");
		}
		cli_write("│");
	}

	cli_write("\x1b[%d;%df", ++ypos, boxWidth * 8);
	cli_write("└");

	for (int dec = width; dec > 0 ; dec--){
		cli_write("─");
	}
	cli_write("┘");
}

void 
taskBox()
{
	int ypos = 6 + (boxHeight ) + 3;
	int width = boxWidth * 1.3;

	int height = (boxHeight * 5) - 2;

	cli_write("\x1b[%d;%df", ypos, boxWidth * 8);

	cli_write("┌");

	for (int dec = width; dec > 0 ; dec--){
		cli_write("─");
	}
	cli_write("┐");

	while (height--){
		cli_write("\x1b[%d;%df", ++ypos, boxWidth * 8);
		cli_write("│");

		for (int dec = width; dec > 0 ; dec--){
			cli_write(" ");
		}
		cli_write("│");
	}

	cli_write("\x1b[%d;%df", ++ypos, boxWidth * 8);
	cli_write("└");

	for (int dec = width; dec > 0 ; dec--){
		cli_write("─");
	}
	cli_write("┘");
}

void 
weekdayHeaders(short offset){

	char *weekdays1[] = {
		"Sunday",
		"Monday",
		"Tuesday",
		"Wednesday",
		"Thursday",
		"Friday",
		"Saturday",
	};
	char *weekdays2[] = {
		"Sun",
		"Mon",
		"Tue",
		"Wed",
		"Thur",
		"Fri",
		"Sat",
	};
	for (int i = 0; i <= 6; i++){
		int of = topoffset + 4;
		if (boxWidth >= 9){
			header((boxWidth * 1.13) * i + 1 , of, weekdays1[(i + offset) % 7 ]);
		}else{
			header((boxWidth * 1.13) * i + 1 , of, weekdays2[(i + offset) % 7 ]);
		}
	}
	cli_write("\x1b[0;0f");
	write(STDOUT_FILENO, "\x1b[H", 3);
	cli_write("%s\r\n", getMonth(current.month));
	cli_write("\x1b[%d;%df%d", 1, 35, current.year);
}



// terminal io
struct termios orig_termios;

void disableRawMode(){
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
		die("tcsetattr");
}

void cli_enableRawMode(){
	if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
		die("tcgetattr");

	atexit(disableRawMode);

	struct termios raw = orig_termios;
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw))
		die("tcsetattr");
}

int
readKey()
{
	char c = 0;
	if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
		die("read");

	return c;
}

void
cli_removeback()
{
	write(STDOUT_FILENO, "\x1b[D", 3);
	write(STDOUT_FILENO, " ", 1);
	write(STDOUT_FILENO, "\x1b[D", 3);
}

void 
showPopupDescription(char add){

	int height = (E.screenrows/4) * 2;
	int width = (E.screencols/5);
	int ypos = E.screenrows/4;
	int xpos = (E.screencols/5) * 2;

	cli_write("\x1b[%d;%df", ypos, xpos);

	cli_write("┌");

	for (int dec = width - 2; dec > 0 ; dec--){
		cli_write("─");
	}

	cli_write("┐");
	int zerocount = 0;
	int off = month_off[current.month] + selection;
	int pos = 0;

	char plan[255] = {};
	/* char plan[255]; */
	/* char plan[255] = "nice though !!"; */
	struct Date date = {selection + 1, current.month,  current.year};
	db_selectPlan(date, plan);
	char new[255] = {};

	if(add == '\b'){

		/* new[] = '\0'; */
		int end = strlen(plan) - 1;
		if(end == 0){
			/* cli_write("hello!"); */
			// remove row here!
			db_removePlan(date);
			/* cli_write(""); */
			/* cli_write("█"); */
			return;
		}
		if(end >= 0) plan[end] = 0;
		else plan[0] = 0;

		strcpy(new, plan);
		/* new = add[] */
	}else{

		sprintf(new, "%s%c", plan, add);
	}
	/* db_selectPlan(current, plan); */
	/* printf("%s", plan); */

	for (int dec = height - 2; dec > 0 ; dec--){
		cli_write("\x1b[%d;%df", ++ypos, xpos);
		cli_write("│");

		for (int dec = width - 2; dec > 0 ; dec--){
			
			if(new[pos] != 0){
				int ch = new[pos++];
				cli_write("%c", ch);

				if(plan[0] == 0){
					if(new[0] != 0)
						db_insertPlan(date, new);
				}else{
					db_updatePlan(date, new);
				}
			}
			else if((new[pos] == 0) && (zerocount == 0)){
				cli_write("█");
				zerocount++;
			}
			else{
				cli_write(" ");
			}

			/* if(insert_text[off][insert_text_pos[off]] != 0 && */
			/* 		insert_text_pos[off] < DESCRIPTIONSIZE){ */
			/* 	int ch = insert_text[off][insert_text_pos[off]++]; */
			/* 	cli_write("%c", ch); */
			/* } */
			/* else if(insert_text[off][insert_text_pos[off]] == 0 && (zerocount == 0)){ */
			/* 	cli_write("█"); */
			/* 	zerocount++; */
			/* } */
			/* else{ */
			/* 	cli_write(" "); */
			/* } */
		}
		cli_write("│");
	}

	/* insert_text_pos[off] = 0; */

	cli_write("\x1b[%d;%df", ++ypos, xpos);

	cli_write("└");

	for (int dec = width - 2; dec > 0 ; dec--){
		cli_write("─");
	}

	cli_write("┘");
}

// w - 119
// 97, 115, 100
// need to change
// key board delay reate in the terminal with
//
// $ kbdrate
//
// or
//
// $ xset r rate 220 40
//
void
statusBar()
{
	int width = E.screencols;
	int xpos = 1;
	int ypos = E.screenrows - 1;
	char normal[] = " NORMAL \0";
	char insert[] = " INSERT \0";
	char *np = normal;
	char *ip = insert;


	cli_write(LIGHTGRAYBG);
	cli_write(BLACKFG);

	while(width--){
		if(width >= E.screencols - 7){
			cli_write("\x1b[%d;%df%s" , ypos, xpos,
			mode == NORMAL ? np++ : ip++);
		}else{
			cli_write("\x1b[%d;%df " BLUEBG, ypos, xpos);
		}
		xpos++;
	}


	cli_write(DEFAULTBG);
	cli_write(DEFAULTFG);
}

void drawLayout1(){
	weekdayHeaders(0);
	dayboxes();
	descriptionBox();
	taskBox();
}

void
cli_keyHandle()
{
	char c = readKey();
	int off = month_off[current.month] + selection;

	if(mode == NORMAL){
		// hide cursor
		/* write(STDOUT_FILENO, "\e[?25l", 6); */
		switch(c){

		case CTRL_KEY('r'):{
			cli_clear();
		}

		case 'j': {
			selection += 7;
			break;
		}

		case 'k': {
			selection -= 7;
			break;
		}

		case 'l': {
			selection += 1;
			break;
		}

		case 'i': {
			c = 0;
			mode = INSERT;
			break;
		}

		case 'n': {
			selection = 0;
			current.month = (current.month + 1) % 12;
			initializeYear(current.year);
			cli_clear();
			break;
		};

		case 'p': {
			selection = 0;
			if (current.month - 1 >= 0) current.month--;
			else current.month = 11;

			initializeYear(current.year);
			cli_clear();
			break;
		}

		case 'h': {
			selection -= 1;
			break;
		}

		case 'T':{
			cli_clear();
			selection = today.day - 1;
			current.month = today.month;
			current.year = today.year;
			break;
		}

		case 'q':{
			// show cursor
			write(STDOUT_FILENO, "\e[?25h", 6);
			db_close();
			exit(0);
			break;
		}

		case 0: {
			/* autoSize(); */
			 break;
		}
		} // switch

		if(c != 'i'){
			drawLayout1();
		}
	}

	if(mode == INSERT){
		switch(c){

		case '\x1b': { // escape
			mode = NORMAL;
			cli_clear();
			drawLayout1();
			break;
		}
		case BACKSPACE: { 
			/* if(insert_text_size[off] -1 >= 0)insert_text_size[off]--; */
			/* 	insert_text[off][insert_text_size[off]] = 0; */
			showPopupDescription('\b');
			break;
		}

		default: {
			/* write(STDOUT_FILENO, "\e[?25h", 6); */
			 /* if(insert_text_size[off] + 1 < DESCRIPTIONSIZE && (c != 0)){ */
					/* insert_text[off][insert_text_size[off]++] = c; */
					/* insert_text[off][insert_text_size[off] + 1] = 0; */
			 /* }; */
			showPopupDescription(c);
			break;
		}
		} // switch

	}
}

void
cli_init(){
	if (getWindowSize(&E.screenrows, &E.screencols) == -1) 
		die("getWindowSize");

	// hide cursor
	write(STDOUT_FILENO, "\e[?25l", 6);
}
