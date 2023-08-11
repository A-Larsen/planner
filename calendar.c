#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <getopt.h>
#include "database.h"

#include "cli.h"
#include "config.h" 
// https://www.hackerearth.com/blog/developers/how-to-find-the-day-of-a-week/
// https://www.timeanddate.com/date/doomsday-calculator.html
// https://www.timeanddate.com/date/doomsday-weekday.html
// https://stackoverflow.com/questions/6054016/c-program-to-find-day-of-week-given-date

// leap year adds an extra day to february
// I a year is evenly divisable by four but not evenly divisable by 100 it is a
// leap year


struct GConfig E;
int boxWidth;
int boxHeight;
int selection = 0;
int month = 0;
struct Date today;
struct Date current;
enum MODE mode = NORMAL;

int
main(int argc, char **argv){
	bool doAutoSize = true;
	int ch = 0;

	while((ch = getopt(argc, argv, "s:")) != -1){

		switch(ch){
			case 's':{ // adjust to a set size
				/* printf("%s", optarg); */
				 if(optarg){
					printf("%s", optarg);
					boxWidth = 5;
					boxHeight = 3;
					doAutoSize=false;
				 }

				break;
			 }

			case '?':
				fprintf(stderr, "usage: %s [-v emailverifiy] [-p port]\n", argv[0]);
				exit(EXIT_FAILURE);
				break;

		}
	}
	/* return 0; */

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	today.year = current.year = tm.tm_year + 1900;
	today.month = current.month = tm.tm_mon;
	today.day = current.day = tm.tm_mday;
	db_init();
	/* db_createPlan(today, "right..."); */
	/* char thing[254] = {}; */
	/* db_selectPlan(today, thing); */

	/* printf("%s\n", thing); */
	/* db_close(); */
	/* return 0; */



	cli_enableRawMode();
	if(doAutoSize) autoSize();
	cli_init();

	cli_clear();
	weekdayHeaders(0);
	dayboxes();
	descriptionBox();
	taskBox();
	initializeYear(current.year);
	/* statusBar(); */

		/* popupDescription(); */

	while(1){
		int pcols = E.screencols;
		int prows = E.screenrows;
		getWindowSize(&E.screenrows, &E.screencols);

		if((pcols != E.screencols) || (prows != E.screenrows)){
			cli_clear();
		}

		cli_keyHandle();

		statusBar();
	}
}
