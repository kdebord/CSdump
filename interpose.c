
// LD_PRELOAD code courtesy of https://gist.github.com/apsun/1e144bf7639b22ff0097171fa0f8c6b1
// setitimer code courtesy of https://www.ibm.com/support/knowledgecenter/en/ssw_ibm_i_71/apis/setitime.htm
// rdmsr and wrmsr code courtesty of https://01.org/msr-tools/downloads/msr-tools-source-code

#define _GNU_SOURCE // Required for non-portable GNU dlsym extensions.
// According to lore, this must be defined before any #includes.
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE  // Required for sigaction.
#endif 

#include <stdio.h> // printf() and friends.
#include <sys/time.h> // get/setitimer().
#include <errno.h> // errno.
#include <stdlib.h> // exit().
#include <signal.h> // sigemptyset(), sigaction().
#include <string.h> // strerror().
#include <dlfcn.h> // dlsym().
#include <sys/types.h> // .
#include <sys/stat.h> //  > open() and friends
#include <fcntl.h> // .
#include <unistd.h> // pread/pwrite().
#include <stdint.h> // uint64_t
#include <assert.h> // assert macros.
#include <inttypes.h> // PRId64 macro for printing uint64_t values

#define typeof __typeof__ // Not sure while this is needed on rhetoric but not feyerabend.

#define IA32_MPERF ((off_t)0x0E7)
#define IA32_APERF ((off_t)0x0E8)
#define POWER_LIMIT ((off_t)0x610)
#define IA32_OPS ((off_t)0x0C1)
#define IA32_EVENT ((off_t)0x186)
#define POWER_STATUS ((off_t)0x611)

static struct my_msrs{
uint64_t mperf;
uint64_t aperf;
uint64_t limit;
uint64_t ops;
uint64_t event;
uint64_t power_status;
} my_msrs0, my_msrs1, my_msrs2, my_msrs3;

static int msr0_fd = -1, msr1_fd = -1, msr2_fd = -1, msr3_fd = -1;
static uint64_t zeroLimit = 18722380920356864; //adjust this for wattage required
int breakTimer = 0;
int newLimit = 800;

static void read_msrs(){
int rc;
assert( msr0_fd > 0 );
assert( msr1_fd > 0 );
assert( msr2_fd > 0 );
assert( msr3_fd > 0 );

rc = pread( msr0_fd, &my_msrs0.mperf, sizeof(my_msrs0.mperf), IA32_MPERF ); assert( 8 == rc );
rc = pread( msr0_fd, &my_msrs0.aperf, sizeof(my_msrs0.aperf), IA32_APERF ); assert( 8 == rc );
rc = pread( msr0_fd, &my_msrs0.limit, sizeof(my_msrs0.limit), POWER_LIMIT ); assert( 8 == rc );
rc = pread( msr0_fd, &my_msrs0.ops,   sizeof(my_msrs0.ops), IA32_OPS ); assert( 8 == rc );
rc = pread( msr0_fd, &my_msrs0.power_status,   sizeof(my_msrs0.power_status), POWER_STATUS ); assert( 8 == rc );

rc = pread( msr1_fd, &my_msrs1.mperf, sizeof(my_msrs1.mperf), IA32_MPERF ); assert( 8 == rc );
rc = pread( msr1_fd, &my_msrs1.aperf, sizeof(my_msrs1.aperf), IA32_APERF ); assert( 8 == rc );
rc = pread( msr1_fd, &my_msrs1.limit, sizeof(my_msrs1.limit), POWER_LIMIT ); assert( 8 == rc );
rc = pread( msr1_fd, &my_msrs1.ops,   sizeof(my_msrs1.ops), IA32_OPS ); assert( 8 == rc );
 
rc = pread( msr2_fd, &my_msrs2.mperf, sizeof(my_msrs2.mperf), IA32_MPERF ); assert( 8 == rc );
rc = pread( msr2_fd, &my_msrs2.aperf, sizeof(my_msrs2.aperf), IA32_APERF ); assert( 8 == rc );
rc = pread( msr2_fd, &my_msrs2.limit, sizeof(my_msrs2.limit), POWER_LIMIT ); assert( 8 == rc );
rc = pread( msr2_fd, &my_msrs2.ops,   sizeof(my_msrs2.ops), IA32_OPS ); assert( 8 == rc );

rc = pread( msr3_fd, &my_msrs3.mperf, sizeof(my_msrs3.mperf), IA32_MPERF ); assert( 8 == rc );
rc = pread( msr3_fd, &my_msrs3.aperf, sizeof(my_msrs3.aperf), IA32_APERF ); assert( 8 == rc );
rc = pread( msr3_fd, &my_msrs3.limit, sizeof(my_msrs3.limit), POWER_LIMIT ); assert( 8 == rc );
rc = pread( msr3_fd, &my_msrs3.ops,   sizeof(my_msrs3.ops), IA32_OPS ); assert( 8 == rc );
}

static void dump_msrs(){
static int initialized = 0;
if( !initialized ){
fprintf(stdout, "RRR aperf mperf limit power_status\n");
initialized = 1;
}

uint64_t limitWatt = 0;
limitWatt = my_msrs0.limit - zeroLimit;
// fprintf(stdout, "Actual Limit: %" PRId64 " Zero: %" PRiId64 "\tDiff: %\n" PRId64,
// my_msrs0.limit,
// zeroLimit,
// limitWatt);

int mask = 0x7fff;
fprintf(stdout, "CORE0: %" PRId64 " %" PRId64 " %" PRId64 " %" PRId64 "\n", 
my_msrs0.mperf,
my_msrs0.aperf,
my_msrs0.limit & mask,
my_msrs0.power_status);

/*
fprintf(stdout, "CORE1: %" PRId64 " %" PRId64 " %" PRId64 " %" PRId64 "\n", 
my_msrs1.mperf,
my_msrs1.aperf,
limitWatt);
fprintf(stdout, "CORE2: %" PRId64 " %" PRId64 " %" PRId64 " %" PRId64 "\n", 
my_msrs2.mperf,
my_msrs2.aperf,
limitWatt);
fprintf(stdout, "CORE3: %" PRId64 " %" PRId64 " %" PRId64 " %" PRId64 "\n", 
my_msrs3.mperf,
my_msrs3.aperf,
limitWatt);
*/

}

static void wr_msrs(int limit) {
my_msrs0.limit = zeroLimit + limit;

//my_msrs1.limit = zeroLimit + limit; 
//my_msrs2.limit = zeroLimit + limit; 
//my_msrs3.limit = zeroLimit + limit; 

pwrite( msr0_fd, &my_msrs0.limit, sizeof(my_msrs0.limit), POWER_LIMIT );
//pwrite( msr1_fd, &my_msrs1.limit, sizeof(my_msrs1.limit), POWER_LIMIT );
//pwrite( msr2_fd, &my_msrs2.limit, sizeof(my_msrs2.limit), POWER_LIMIT );
//pwrite( msr3_fd, &my_msrs3.limit, sizeof(my_msrs3.limit), POWER_LIMIT );
}

static void set_up_msr_file_descriptors(){
// Getting virtual processors mapped to physical hyperthreads, cores and sockets
// is a nightmare.  To do this correctly, see https://github.com/LLNL/libmsr/blob/master/src/msr_core.c
// For now, we're going to assume single-socket and only work off of one core.

// Also note:  the stock msr kernel module tests for capability CAP_SYS_RAWIO 
// if the program is not running as root.  There's a way of bestowing that 
// capability onto a binary, but I can't be arsed.  If you'd rather not run as
// root, use msr-safe (https://github.com/LLNL/msr-safe).  Setting file permissions
// in /dev/cpu/x/msr is not sufficient for accessing the stock msr kernel module.

// See https://lwn.net/Articles/542327/ for a bit of the politics.

msr0_fd = open("/dev/cpu/0/msr_safe", O_RDWR);
if(msr0_fd == -1){
fprintf(stderr, "QQQ %s:%d ERROR open() returned (%d) (%s).\n", 
__FILE__, __LINE__, msr0_fd, strerror(errno) );
exit(-1);
}
msr1_fd = open("/dev/cpu/1/msr_safe", O_RDWR);
if(msr1_fd == -1){
fprintf(stderr, "QQQ %s:%d ERROR open() returned (%d) (%s).\n", 
__FILE__, __LINE__, msr1_fd, strerror(errno) );
exit(-1);
}
msr2_fd = open("/dev/cpu/2/msr_safe", O_RDWR);
if(msr1_fd == -1){
fprintf(stderr, "QQQ %s:%d ERROR open() returned (%d) (%s).\n", 
__FILE__, __LINE__, msr2_fd, strerror(errno) );
exit(-1);
}
msr3_fd = open("/dev/cpu/3/msr_safe", O_RDWR);
if(msr1_fd == -1){
fprintf(stderr, "QQQ %s:%d ERROR open() returned (%d) (%s).\n", 
__FILE__, __LINE__, msr3_fd, strerror(errno) );
exit(-1);
}
}

static void mytimerhandler( int sig ) {
//fprintf(stdout, "Timer: %d ", breakTimer);

breakTimer++;

read_msrs();
dump_msrs();
wr_msrs(newLimit);
}

static void set_up_itimer(){
// Note that there are three timers and their associated signals.
// ITIMER_REAL    -> SIGALRM wall-clock time
// ITIMER_VIRTUAL -> SIGVTALRM process time
// ITIMER_PROF    -> SIGPROF profiling time
// (includes syscalls made on behalf of the process)
struct itimerval itv;
struct sigaction sact;
int rc;

sigemptyset( &sact.sa_mask ); // macro to initialize sact.
sact.sa_flags = 0; // see man page for sigaction.
 
fprintf(stdout, "Enter power limit: "); // Get new Power Limit from user.
scanf("%d", &newLimit);

fprintf(stdout, "Setting Up Instruction Retired Counter...\n\n");


my_msrs0.event = 4194496; 
pwrite( msr0_fd, &my_msrs0.event, sizeof(my_msrs0.event), IA32_EVENT );
//write msr for correct bitmask to 0x186
 
fprintf(stdout, "Warming up...\n\n");
sleep(2);
sact.sa_handler = mytimerhandler; // execute this function when the timer expires.
sigaction( SIGPROF, &sact, NULL ); // set up our signal handler.
itv.it_interval.tv_sec  =     0; // Timer interval in seconds        
itv.it_interval.tv_usec = 10000;  //   and microseconds.
itv.it_value.tv_sec     =     0;      // Time to first signal,
itv.it_value.tv_usec    = 10000;     //   same units.

rc = setitimer( ITIMER_PROF, &itv, NULL );
if(rc){
fprintf(stderr, "QQQ %s:%d ERROR setitimer returned (%d) (%s).\n", 
__FILE__, __LINE__, rc, strerror(errno) );
exit(-1); // No need to continue.  Bail out now.
}
}

// Declare a pointer to hold the original value for main().
// Being static makes sure nothing outside this file can see it.
static int (*main_orig)(int, char **, char **);

int main_hook(int argc, char **argv, char **envp)
{
// Dump out the command line arguments.
fprintf(stdout, "QQQ %s:%d Printing out command line arguments.\n",
__FILE__, __LINE__ );
if(argc == 0){
fprintf(stdout, "QQQ %s:%d argc = 0 (really?)\n",
__FILE__, __LINE__);
}else{
for (int i = 0; i < argc; ++i) {
fprintf(stdout, "QQQ %s:%d argv[%d] = %s\n", 
__FILE__, __LINE__, i, argv[i]);
}
}
 
// Open up the msr file(s)
fprintf(stdout, "QQQ %s:%d Setting up the msr file descriptors.\n",
__FILE__, __LINE__ );
set_up_msr_file_descriptors();

// Set up our itimer.
fprintf(stdout, "QQQ %s:%d Setting up itimer.\n",
__FILE__, __LINE__ );
set_up_itimer();

// Pass control to the original program.
fprintf(stdout, "QQQ %s:%d Calling the original main().\n",
__FILE__, __LINE__ );
int ret = main_orig(argc, argv, envp);

// Dump out main's return value.
fprintf(stdout, "QQQ %s:%d main() returned %d\n", 
__FILE__, __LINE__, ret);
 
printf("\nResetting MSRs! \n");
wr_msrs(800);
read_msrs();
dump_msrs();

return ret;
}

// We can't trap _start(), but we can trap __libc_start_main().
// See the definition at http://refspecs.linuxbase.org/LSB_3.1.0/LSB-generic/LSB-generic/baselib---libc-start-main-.html
int __libc_start_main(
    int (*main)(int, char **, char **), // function pointer for main()
    int argc, // number of arguments in argv
    char **argv, // list of the arguments passed to the program
    int (*init)(int, char **, char **), // function pointer for init()
    void (*fini)(void), // function pointer for fini()
    void (*rtld_fini)(void), // function pointer for rtld_fini
    void *stack_end) // Haven't been able to track this down in the standards.
{
    main_orig = main;

    // Find the real __libc_start_main() and call it.
    // Note that typeof is used instead of typedef.
    typeof(&__libc_start_main) orig = dlsym(RTLD_NEXT, "__libc_start_main");
    return orig(main_hook, argc, argv, init, fini, rtld_fini, stack_end);
}
