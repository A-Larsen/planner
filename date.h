#ifndef DATE_H
#define DATE_H
struct Date {
	unsigned short day;
	unsigned short month;
	unsigned short year;
};

extern struct Date today;
extern struct Date current;


#endif
