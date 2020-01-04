#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <gps.h>

#define GPSTIME_GPSD_HOSTNAME			"localhost"
#define GPSTIME_GPSD_PORT				"2947"
#define GPSTIME_GPSD_WAIT_MAX_MS		500 * 1000
#define GPSTIME_UPDATE_DELAY_SEC		5
#define GPSTIME_UPDATE_RETRY_DELAY_SEC	1
#define GPSTIME_WAIT_ERRNO				-99

#define GPSTIME_DEBUG

static int update_gps_data(struct gps_data_t *gps_data)
{
	int ret;

	gps_clear_fix(&gps_data->fix);

	if (!gps_waiting(gps_data, GPSTIME_GPSD_WAIT_MAX_MS))
		return GPSTIME_WAIT_ERRNO;

	ret = gps_read(gps_data);
	if (ret < 0) {
		perror("Failed to read the data from GPSD");
		return errno;
	}

	return 0;
}

int main()
{
	int ret;

	double gps_time_dbl;
	char gps_time_buff[16];
	struct timeval now;

	struct gps_data_t gps_data;

//	daemon(0, 0);

	ret = gps_open(GPSTIME_GPSD_HOSTNAME, GPSTIME_GPSD_PORT, &gps_data);
	if (ret) {
		printf("Failed to open the GPS device : %s\n", gps_errstr(ret));
		goto gps_open_fail;
	}

	gps_stream(&gps_data, 0, NULL);

	memset(&now, 0, sizeof(now));

	do {

		if (update_gps_data(&gps_data)) {
#ifdef GPSTIME_DEBUG
			printf("Waiting..\n");
#endif
			sleep(GPSTIME_UPDATE_RETRY_DELAY_SEC);
			continue;
		}

		gps_time_dbl = gps_data.fix.time;

		if (gps_data.status != STATUS_FIX ||
			(gps_data.fix.mode != MODE_2D && gps_data.fix.mode != MODE_3D) ||
			isnan(gps_time_dbl) ||
			!(gps_data.set | TIME_SET)) {
#ifdef GPSTIME_DEBUG
			printf("GPS not yet ready\n");
#endif
			sleep(GPSTIME_UPDATE_RETRY_DELAY_SEC);
			continue;
		}

		sprintf(gps_time_buff, "%.0F", gps_time_dbl);

		now.tv_sec = (__time_t) strtoul(gps_time_buff, NULL, 0);

#ifdef GPSTIME_DEBUG
			printf("Epoch Time : %lu\n", now.tv_sec);
#endif

		ret = settimeofday(&now, NULL);
		if (ret < 0) {
			perror("Failed to set the system time");
		}

		sleep(GPSTIME_UPDATE_DELAY_SEC);
	}while(1);

	return 0;

gps_open_fail: return errno;
}
