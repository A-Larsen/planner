#define _GNU_SOURCE
#include "database.h"
#include "date.h"

static sqlite3 *plans_db;

short
db_insertPlan(struct Date date, 
			  char *plan)
{
	char *sql = NULL;

	asprintf(&sql, 
			"INSERT INTO plans (Day, Month, Year, Plan) VALUES ('%d', '%d', %d, '%s')", 
			date.day,
			date.month, 
			date.year, 
			plan);

	sqlite3_stmt *stmt = NULL;
	int rc = sqlite3_prepare_v2(plans_db, 
							sql, 
							strlen(sql), 
							&stmt, 
							NULL);

	/* printf("SQL: %s\n", sql); */

	if(rc != SQLITE_OK){
		fprintf(stderr, "prepare failed: %s\n", 
				sqlite3_errmsg(plans_db));
	}

	rc = sqlite3_step(stmt);

	if(rc != SQLITE_DONE){
		fprintf(stderr, "prepare failed: %s\n", 
				sqlite3_errmsg(plans_db));

	}

	sqlite3_finalize(stmt);

	if(sql) free((void *)sql);

	return 0;
}

short
db_updatePlan(struct Date date, 
			  char *plan)
{
	char *sql = NULL;

	asprintf(&sql, 
			"UPDATE plans SET Plan = '%s' WHERE Day=%d and Month=%d and year=%d", 
			plan,
			date.day,
			date.month, 
			date.year);

	sqlite3_stmt *stmt = NULL;
	int rc = sqlite3_prepare_v2(plans_db, 
							sql, 
							strlen(sql), 
							&stmt, 
							NULL);

	/* printf("SQL: %s\n", sql); */

	if(rc != SQLITE_OK){
		fprintf(stderr, "prepare failed: %s\n", 
				sqlite3_errmsg(plans_db));
	}

	rc = sqlite3_step(stmt);

	if(rc != SQLITE_DONE){
		fprintf(stderr, "prepare failed: %s\n", 
				sqlite3_errmsg(plans_db));

	}

	sqlite3_finalize(stmt);

	if(sql) free((void *)sql);

	return 0;
}

short
db_selectPlan(struct Date date, 
			  char *plan)
{
	char *sql = NULL;
	sqlite3_stmt *stmt = NULL;

	asprintf(&sql, 
			"SELECT Plan FROM plans WHERE Day=%d and Month=%d and year=%d", 
			date.day,
			date.month, 
			date.year);


	int rc = sqlite3_prepare_v2(plans_db, 
							    sql, 
							    -1, 
							    &stmt, 
							    NULL);

	/* printf("SQL: %s\n", sql); */

	rc = sqlite3_step(stmt);

	char *ret = (char *)sqlite3_column_text(stmt, 0);
	if(ret)
		strcpy(plan, ret);
	else
		plan = NULL;


	sqlite3_finalize(stmt);

	if(sql) free((void *)sql);

	return 0;
}

short
db_createPlan(struct Date date, 
			  char *plan)
{
	char storedPlan[255] = {};
	db_selectPlan(date, storedPlan);

	if(storedPlan[0] == 0) db_insertPlan(date, plan);
	else db_updatePlan(date, plan);

	return 0;
}

int
db_init()
{
    static int rc;

    rc = sqlite3_open("plans.db", &plans_db);

    if(rc){
		fprintf(stderr, "Can't open users database: %s\n", 
				sqlite3_errmsg(plans_db));
		return 0;
    }

    return 1;
    
}
short
db_removePlan(struct Date date)
{
	char *sql = NULL;
	sqlite3_stmt *stmt = NULL;

	asprintf(&sql, 
			"DELETE FROM plans WHERE Day=%d and Month=%d and year=%d", 
			date.day,
			date.month, 
			date.year);


	int rc = sqlite3_prepare_v2(plans_db, 
							    sql, 
							    -1, 
							    &stmt, 
							    NULL);

	/* printf("SQL: %s\n", sql); */

	rc = sqlite3_step(stmt);

	/* char *ret = (char *)sqlite3_column_text(stmt, 0); */
	/* if(ret) */
	/* 	strcpy(plan, ret); */
	/* else */
	/* 	plan = NULL; */


	/* sqlite3_finalize(stmt); */

	if(sql) free((void *)sql);

	return 0;
}

void
db_close()
{
	sqlite3_close(plans_db);
}

