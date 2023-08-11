#ifndef DATABASE_H
#define DATABASE_H
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "date.h"

short db_createPlan(struct Date date, char *plan);
int db_init();
void db_close();
short db_selectPlan(struct Date date, char *plan);
short db_updatePlan(struct Date date, char *plan);
short db_insertPlan(struct Date date, char *plan);
short db_removePlan(struct Date date);

#endif
