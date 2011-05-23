#ifndef DATE_H
#define DATE_H

#include <stdlib.h>

#define approxidate(s) approxidate_careful((s), NULL)

enum date_mode {
	DATE_NORMAL = 0,
	DATE_RELATIVE,
	DATE_SHORT,
	DATE_LOCAL,
	DATE_ISO8601,
	DATE_RFC2822,
	DATE_RAW
};

const char *show_date_relative(unsigned long time, int tz,
			       const struct timeval *now,
			       char *timebuf,
			       size_t timebuf_size);
const char *show_date(unsigned long time, int tz, enum date_mode mode);
int parse_date_basic(const char *date, unsigned long *timestamp, int *offset);
int parse_date(const char *date, char *result, int maxlen);
enum date_mode parse_date_format(const char *format);
void datestamp(char *buf, int bufsize);
unsigned long approxidate_relative(const char *date, const struct timeval *tv);
unsigned long approxidate_careful(const char *date, int *error_ret);

#endif
