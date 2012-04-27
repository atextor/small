/*
 * cpuload.c
 * Outputs the current CPU load (in %) every second on Linux
 * Andreas Textor <textor.andreas@googlemail.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int system_cpu(void) {
	unsigned int cpuload;
	u_int64_t load, total, oload, ototal;
	u_int64_t ab, ac, ad, ae;
	static int oldload;
	static int oldtotal;
	FILE *stat;

	stat = fopen("/proc/stat", "r");
	fscanf(stat, "%*s %Ld %Ld %Ld %Ld", &ab, &ac, &ad, &ae);
	fclose(stat);

	load = ab + ac + ad;
	total = ab + ac + ad + ae;

	oload = oldload;
	ototal = oldtotal;

	oldload = load;
	oldtotal = total;

	if (ototal == 0) {
		cpuload = 0;
	} else if ((total - ototal) <= 0) {
		cpuload = 100;
	} else {
		cpuload = (100 * (load - oload)) / (total - ototal);
	}
	return cpuload;
}

int main() {
	while (1) {
		sleep(1);
		printf("%d\n", system_cpu());
		fflush(stdout);
	};
	return 0;
}

