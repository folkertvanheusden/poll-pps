#include <sched.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <wiringPi.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/shm.h>
#include <sys/types.h>

#define NTP_KEY 1314148400

struct shmTime {
        int    mode; /* 0 - if valid is set:
                      *       use values,
                      *       clear valid
                      * 1 - if valid is set:
                      *       if count before and after read of data is equal:
                      *         use values
                      *       clear valid
                      */
        volatile int    count;
        time_t          clockTimeStampSec;
        int             clockTimeStampUSec;
        time_t          receiveTimeStampSec;
        int             receiveTimeStampUSec;
        int             leap;
        int             precision;
        int             nsamples;
        volatile int    valid;
        unsigned        clockTimeStampNSec;     /* Unsigned ns timestamps */
        unsigned        receiveTimeStampNSec;   /* Unsigned ns timestamps */
        int             dummy[8];
};

struct shmTime * get_shm_pointer(int unitNr)
{
        void *addr;
        struct shmTime *pst;
        int shmid = shmget(NTP_KEY + unitNr, sizeof(struct shmTime), IPC_CREAT);
        if (shmid == -1)
                perror("get_shm_pointer: shmget failed");

        addr = shmat(shmid, NULL, 0);
        if (addr == (void *)-1)
                perror("get_shm_pointer: shmat failed");

        pst = (struct shmTime *)addr;

        return pst;
}

int main(int argc, char *argv[])
{
	wiringPiSetup();

	pinMode(1, INPUT);  // GPIO18, pin 12

	setpriority(PRIO_PROCESS, getpid(), -20);

	mlockall(MCL_FUTURE | MCL_CURRENT);

	struct sched_param sp { 0 };
	sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
	sched_setscheduler(0, SCHED_FIFO, &sp);

	struct shmTime *pst = get_shm_pointer(0);

	struct timespec tp { 0, 0 };

	for(;;) {
		// wait for low
		while(digitalRead(1) == 1) {
		}

		// wait for high
		while(digitalRead(1) == 0) {
		}

		if (clock_gettime(CLOCK_REALTIME, &tp) == -1)
			perror("clock_gettime");

		printf("%ld.%09ld\n", tp.tv_sec, tp.tv_nsec);

		pst -> valid = 0;

		pst -> clockTimeStampSec = tp.tv_sec;
		pst -> clockTimeStampUSec = tp.tv_nsec / 1000;

		pst -> receiveTimeStampSec = tp.tv_sec + (tp.tv_nsec >= 500000000);
		pst -> receiveTimeStampUSec = 0;

		pst -> leap = pst -> mode = pst -> count = 0;

		pst -> precision = -1;   /* 0 = precision of 1 sec., -1 = 0.5s */

		pst -> valid = 1;
	}

	return 0;
}
