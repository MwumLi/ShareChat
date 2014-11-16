#ifndef GETCURTIME_H
#define GETCURTIME_H 

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

/* return a cur time string by malloc
 * if use end, you need free it
 * */
char *getCurtime()
{
	time_t		curSeconds;
	struct tm	*curTime;
	char		*now;

	time(&curSeconds);
	curTime = localtime(&curSeconds);

	now = (char *)malloc(10);
	sprintf(now, "%d:%d:%d",	curTime->tm_hour, 
								curTime->tm_min, 
								curTime->tm_sec);
	return now;
}

#endif
