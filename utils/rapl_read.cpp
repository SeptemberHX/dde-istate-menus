/* Read the RAPL registers on recent (>sandybridge) Intel processors	*/
/*									*/
/* There are currently three ways to do this:				*/
/*	1. Read the MSRs directly with /dev/cpu/??/msr			*/
/*	2. Use the perf_event_open() interface				*/
/*	3. Read the values from the sysfs powercap interface		*/
/*									*/
/* MSR Code originally based on a (never made it upstream) linux-kernel	*/
/*	RAPL driver by Zhang Rui <rui.zhang@intel.com>			*/
/*	https://lkml.org/lkml/2011/5/26/93				*/
/* Additional contributions by:						*/
/*	Romain Dolbeau -- romain @ dolbeau.org				*/
/*									*/
/* For raw MSR access the /dev/cpu/??/msr driver must be enabled and	*/
/*	permissions set to allow read access.				*/
/*	You might need to "modprobe msr" before it will work.		*/
/*									*/
/* perf_event_open() support requires at least Linux 3.14 and to have	*/
/*	/proc/sys/kernel/perf_event_paranoid < 1			*/
/*									*/
/* the sysfs powercap interface got into the kernel in 			*/
/*	2d281d8196e38dd (3.13)						*/
/*									*/
/* Compile with:   gcc -O2 -Wall -o rapl-read rapl-read.c -lm		*/
/*									*/
/* Vince Weaver -- vincent.weaver @ maine.edu -- 11 September 2015	*/
/*									*/

#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <fcntl.h>
#include <cerrno>
#include <cinttypes>
#include <unistd.h>
#include <cstring>

#include <sys/syscall.h>
#include <linux/perf_event.h>
#include "rapl_read.h"

#include <QList>
#include <QDebug>
#include <cmath>
#include <QDateTime>

/* AMD Support */
#define MSR_AMD_RAPL_POWER_UNIT            0xc0010299

#define MSR_AMD_PKG_ENERGY_STATUS        0xc001029B
#define MSR_AMD_PP0_ENERGY_STATUS        0xc001029A



/* Intel support */

#define MSR_INTEL_RAPL_POWER_UNIT        0x606
/*
 * Platform specific RAPL Domains.
 * Note that PP1 RAPL Domain is supported on 062A only
 * And DRAM RAPL Domain is supported on 062D only
 */
/* Package RAPL Domain */
#define MSR_PKG_RAPL_POWER_LIMIT    0x610
#define MSR_INTEL_PKG_ENERGY_STATUS    0x611
#define MSR_PKG_PERF_STATUS        0x613
#define MSR_PKG_POWER_INFO        0x614

/* PP0 RAPL Domain */
#define MSR_PP0_POWER_LIMIT        0x638
#define MSR_INTEL_PP0_ENERGY_STATUS    0x639
#define MSR_PP0_POLICY            0x63A
#define MSR_PP0_PERF_STATUS        0x63B

/* PP1 RAPL Domain, may reflect to uncore devices */
#define MSR_PP1_POWER_LIMIT        0x640
#define MSR_PP1_ENERGY_STATUS        0x641
#define MSR_PP1_POLICY            0x642

/* DRAM RAPL Domain */
#define MSR_DRAM_POWER_LIMIT        0x618
#define MSR_DRAM_ENERGY_STATUS        0x619
#define MSR_DRAM_PERF_STATUS        0x61B
#define MSR_DRAM_POWER_INFO        0x61C

/* PSYS RAPL Domain */
#define MSR_PLATFORM_ENERGY_STATUS    0x64d

/* RAPL UNIT BITMASK */
#define POWER_UNIT_OFFSET    0
#define POWER_UNIT_MASK        0x0F

#define ENERGY_UNIT_OFFSET    0x08
#define ENERGY_UNIT_MASK    0x1F00

#define TIME_UNIT_OFFSET    0x10
#define TIME_UNIT_MASK        0xF000

int systemCpuModel = -1;

static int open_msr(int core) {

    char msr_filename[BUFSIZ];
    int fd;

    sprintf(msr_filename, "/dev/cpu/%d/msr", core);
    fd = open(msr_filename, O_RDONLY);
    if (fd < 0) {
        if (errno == ENXIO) {
            fprintf(stderr, "rdmsr: No CPU %d\n", core);
        } else if (errno == EIO) {
            fprintf(stderr, "rdmsr: CPU %d doesn't support MSRs\n",
                    core);
        } else {
            perror("rdmsr:open");
            fprintf(stderr, "Trying to open %s\n", msr_filename);
        }
    }

    return fd;
}

static long long read_msr(int fd, unsigned int which) {

    uint64_t data;

    if (pread(fd, &data, sizeof data, which) != sizeof data) {
        perror("rdmsr:pread");
        fprintf(stderr, "Error reading MSR %x\n", which);
        exit(127);
    }

    return (long long) data;
}

#define CPU_VENDOR_INTEL    1
#define CPU_VENDOR_AMD        2

#define CPU_SANDYBRIDGE        42
#define CPU_SANDYBRIDGE_EP    45
#define CPU_IVYBRIDGE        58
#define CPU_IVYBRIDGE_EP    62
#define CPU_HASWELL        60
#define CPU_HASWELL_ULT        69
#define CPU_HASWELL_GT3E    70
#define CPU_HASWELL_EP        63
#define CPU_BROADWELL        61
#define CPU_BROADWELL_GT3E    71
#define CPU_BROADWELL_EP    79
#define CPU_BROADWELL_DE    86
#define CPU_SKYLAKE        78
#define CPU_SKYLAKE_HS        94
#define CPU_SKYLAKE_X        85
#define CPU_ICELAKE         126
#define CPU_KNIGHTS_LANDING    87
#define CPU_KNIGHTS_MILL    133
#define CPU_KABYLAKE_MOBILE    142
#define CPU_KABYLAKE        158
#define CPU_ATOM_SILVERMONT    55
#define CPU_ATOM_AIRMONT    76
#define CPU_ATOM_MERRIFIELD    74
#define CPU_ATOM_MOOREFIELD    90
#define CPU_ATOM_GOLDMONT    92
#define CPU_ATOM_GEMINI_LAKE    122
#define CPU_ATOM_DENVERTON    95

#define CPU_AMD_FAM17H        0xc000

static unsigned int msr_rapl_units, msr_pkg_energy_status, msr_pp0_energy_status;


/* TODO: on Skylake, also may support  PSys "platform" domain,	*/
/* the whole SoC not just the package.				*/
/* see dcee75b3b7f025cc6765e6c92ba0a4e59a4d25f4			*/

static int detect_cpu(void) {

    FILE *fff;

    int vendor = -1, family, model = -1;
    char buffer[BUFSIZ], *result;
    char vendor_string[BUFSIZ];

    fff = fopen("/proc/cpuinfo", "r");
    if (fff == NULL) return -1;

    while (1) {
        result = fgets(buffer, BUFSIZ, fff);
        if (result == NULL) break;

        if (!strncmp(result, "vendor_id", 8)) {
            sscanf(result, "%*s%*s%s", vendor_string);

            if (!strncmp(vendor_string, "GenuineIntel", 12)) {
                vendor = CPU_VENDOR_INTEL;
            }
            if (!strncmp(vendor_string, "AuthenticAMD", 12)) {
                vendor = CPU_VENDOR_AMD;
            }
        }

        if (!strncmp(result, "cpu family", 10)) {
            sscanf(result, "%*s%*s%*s%d", &family);
        }

        if (!strncmp(result, "model", 5)) {
            sscanf(result, "%*s%*s%d", &model);
        }

    }

    if (vendor == CPU_VENDOR_INTEL) {
        if (family != 6) {
            printf("Wrong CPU family %d\n", family);
            return -1;
        }

        msr_rapl_units = MSR_INTEL_RAPL_POWER_UNIT;
        msr_pkg_energy_status = MSR_INTEL_PKG_ENERGY_STATUS;
        msr_pp0_energy_status = MSR_INTEL_PP0_ENERGY_STATUS;

        printf("Found ");

        switch (model) {
            case CPU_SANDYBRIDGE:
                printf("Sandybridge");
                break;
            case CPU_SANDYBRIDGE_EP:
                printf("Sandybridge-EP");
                break;
            case CPU_IVYBRIDGE:
                printf("Ivybridge");
                break;
            case CPU_IVYBRIDGE_EP:
                printf("Ivybridge-EP");
                break;
            case CPU_HASWELL:
            case CPU_HASWELL_ULT:
            case CPU_HASWELL_GT3E:
                printf("Haswell");
                break;
            case CPU_HASWELL_EP:
                printf("Haswell-EP");
                break;
            case CPU_BROADWELL:
            case CPU_BROADWELL_GT3E:
                printf("Broadwell");
                break;
            case CPU_BROADWELL_EP:
                printf("Broadwell-EP");
                break;
            case CPU_SKYLAKE:
            case CPU_SKYLAKE_HS:
                printf("Skylake");
                break;
            case CPU_SKYLAKE_X:
                printf("Skylake-X");
                break;
            case CPU_KABYLAKE:
            case CPU_KABYLAKE_MOBILE:
                printf("Kaby Lake");
                break;
            case CPU_KNIGHTS_LANDING:
                printf("Knight's Landing");
                break;
            case CPU_KNIGHTS_MILL:
                printf("Knight's Mill");
                break;
            case CPU_ATOM_GOLDMONT:
            case CPU_ATOM_GEMINI_LAKE:
            case CPU_ATOM_DENVERTON:
                printf("Atom");
                break;
            case CPU_ICELAKE:
                printf("Ice Lake");
                break;
            default:
                printf("Unsupported model %d\n", model);
                model = -1;
                break;
        }
    }

    if (vendor == CPU_VENDOR_AMD) {

        msr_rapl_units = MSR_AMD_RAPL_POWER_UNIT;
        msr_pkg_energy_status = MSR_AMD_PKG_ENERGY_STATUS;
        msr_pp0_energy_status = MSR_AMD_PP0_ENERGY_STATUS;

        if (family != 23) {
            printf("Wrong CPU family %d\n", family);
            return -1;
        }
        model = CPU_AMD_FAM17H;
    }

    fclose(fff);

    printf(" Processor type\n");

    return model;
}

#define MAX_CPUS    1024
#define MAX_PACKAGES    16

static int total_cores = 0, total_packages = 0;
static int package_map[MAX_PACKAGES];

static int detect_packages(void) {

    char filename[BUFSIZ];
    FILE *fff;
    int package;
    int i;

    for (i = 0; i < MAX_PACKAGES; i++) package_map[i] = -1;

    printf("\t");
    for (i = 0; i < MAX_CPUS; i++) {
        sprintf(filename, "/sys/devices/system/cpu/cpu%d/topology/physical_package_id", i);
        fff = fopen(filename, "r");
        if (fff == NULL) break;
        fscanf(fff, "%d", &package);
        printf("%d (%d)", i, package);
        if (i % 8 == 7) printf("\n\t"); else printf(", ");
        fclose(fff);

        if (package_map[package] == -1) {
            total_packages++;
            package_map[package] = i;
        }

    }

    printf("\n");

    total_cores = i;

    printf("\tDetected %d cores in %d packages\n\n",
           total_cores, total_packages);

    return 0;
}


/*******************************/
/* MSR code                    */
/*******************************/

static int rapl_msr(int cpu_model, QList<PowerConsumption>& pcList) {

    int fd;
    long long result;
    double cpu_energy_units[MAX_PACKAGES], dram_energy_units[MAX_PACKAGES];
    double package_before[MAX_PACKAGES];
    double pp0_before[MAX_PACKAGES];
    double pp1_before[MAX_PACKAGES];
    double dram_before[MAX_PACKAGES];
    double psys_before[MAX_PACKAGES];
    int j;

    int dram_avail = 0, pp0_avail = 0, pp1_avail = 0, psys_avail = 0;
    int different_units = 0;

    if (cpu_model < 0) {
        printf("\tUnsupported CPU model %d\n", cpu_model);
        return -1;
    }

    switch (cpu_model) {

        case CPU_SANDYBRIDGE_EP:
        case CPU_IVYBRIDGE_EP:
            pp0_avail = 1;
            pp1_avail = 0;
            dram_avail = 1;
            different_units = 0;
            psys_avail = 0;
            break;

        case CPU_HASWELL_EP:
        case CPU_BROADWELL_EP:
        case CPU_SKYLAKE_X:
            pp0_avail = 1;
            pp1_avail = 0;
            dram_avail = 1;
            different_units = 1;
            psys_avail = 0;
            break;

        case CPU_KNIGHTS_LANDING:
        case CPU_KNIGHTS_MILL:
            pp0_avail = 0;
            pp1_avail = 0;
            dram_avail = 1;
            different_units = 1;
            psys_avail = 0;
            break;

        case CPU_SANDYBRIDGE:
        case CPU_IVYBRIDGE:
            pp0_avail = 1;
            pp1_avail = 1;
            dram_avail = 0;
            different_units = 0;
            psys_avail = 0;
            break;

        case CPU_HASWELL:
        case CPU_HASWELL_ULT:
        case CPU_HASWELL_GT3E:
        case CPU_BROADWELL:
        case CPU_BROADWELL_GT3E:
        case CPU_ATOM_GOLDMONT:
        case CPU_ATOM_GEMINI_LAKE:
        case CPU_ATOM_DENVERTON:
            pp0_avail = 1;
            pp1_avail = 1;
            dram_avail = 1;
            different_units = 0;
            psys_avail = 0;
            break;

        case CPU_ICELAKE:
        case CPU_SKYLAKE:
        case CPU_SKYLAKE_HS:
        case CPU_KABYLAKE:
        case CPU_KABYLAKE_MOBILE:
            pp0_avail = 1;
            pp1_avail = 1;
            dram_avail = 1;
            different_units = 0;
            psys_avail = 1;
            break;

        case CPU_AMD_FAM17H:
            pp0_avail = 1;        // maybe
            pp1_avail = 0;
            dram_avail = 0;
            different_units = 0;
            psys_avail = 0;
            break;
    }

    for(j=0;j<total_packages;j++) {
        fd=open_msr(package_map[j]);
        if (fd < 0) {
            return 1;
        }

        /* Calculate the units used */
        result=read_msr(fd,msr_rapl_units);

        cpu_energy_units[j]=pow(0.5,(double)((result>>8)&0x1f));

        /* On Haswell EP and Knights Landing */
        /* The DRAM units differ from the CPU ones */
        if (different_units) {
            dram_energy_units[j]=pow(0.5,(double)16);
        }
        else {
            dram_energy_units[j]=cpu_energy_units[j];
        }

        close(fd);

    }

    for (j = 0; j < total_packages; j++) {

        fd = open_msr(package_map[j]);
        PowerConsumption pc {};

        /* Package Energy */
        result = read_msr(fd, msr_pkg_energy_status);
        package_before[j] = (double) result * cpu_energy_units[j];
        pc.package = package_before[j];

        /* PP0 energy */
        /* Not available on Knights* */
        /* Always returns zero on Haswell-EP? */
        if (pp0_avail) {
            result = read_msr(fd, msr_pp0_energy_status);
            pp0_before[j] = result * cpu_energy_units[j];
            pc.cores = pp0_before[j];
        }

        /* PP1 energy */
        /* not available on *Bridge-EP */
        if (pp1_avail) {
            result = read_msr(fd, MSR_PP1_ENERGY_STATUS);
            pp1_before[j] = (double) result * cpu_energy_units[j];
            pc.onCoreGpu = pp1_before[j];
        }


        /* Updated documentation (but not the Vol3B) says Haswell and	*/
        /* Broadwell have DRAM support too				*/
        if (dram_avail) {
            result = read_msr(fd, MSR_DRAM_ENERGY_STATUS);
            dram_before[j] = (double) result * dram_energy_units[j];
            pc.dram = dram_before[j];
        }

        /* Skylake and newer for Psys				*/
        if (psys_avail) {
            result = read_msr(fd, MSR_PLATFORM_ENERGY_STATUS);
            psys_before[j] = (double) result * cpu_energy_units[j];
            pc.psys = psys_before[j];
        }

        pc.timestamp = QDateTime::currentMSecsSinceEpoch();
        pcList << pc;
        close(fd);
    }
    return 0;
}

static int perf_event_open(struct perf_event_attr *hw_event_uptr,
                           pid_t pid, int cpu, int group_fd, unsigned long flags) {

    return syscall(__NR_perf_event_open, hw_event_uptr, pid, cpu,
                   group_fd, flags);
}

#define NUM_RAPL_DOMAINS    5

char rapl_domain_names[NUM_RAPL_DOMAINS][30] = {
        "energy-cores",
        "energy-gpu",
        "energy-pkg",
        "energy-ram",
        "energy-psys",
};


static int check_paranoid(void) {

    int paranoid_value;
    FILE *fff;

    fff = fopen("/proc/sys/kernel/perf_event_paranoid", "r");
    if (fff == NULL) {
        fprintf(stderr, "Error! could not open /proc/sys/kernel/perf_event_paranoid %s\n",
                strerror(errno));

        /* We can't return a negative value as that implies no paranoia */
        return 500;
    }

    fscanf(fff, "%d", &paranoid_value);
    fclose(fff);

    return paranoid_value;

}

static int rapl_perf(int core) {

    FILE *fff;
    int type;
    int config[NUM_RAPL_DOMAINS];
    char units[NUM_RAPL_DOMAINS][BUFSIZ];
    char filename[BUFSIZ];
    int fd[NUM_RAPL_DOMAINS][MAX_PACKAGES];
    double scale[NUM_RAPL_DOMAINS];
    struct perf_event_attr attr;
    long long value;
    int i, j;
    int paranoid_value;

    printf("\nTrying perf_event interface to gather results\n\n");

    fff = fopen("/sys/bus/event_source/devices/power/type", "r");
    if (fff == NULL) {
        printf("\tNo perf_event rapl support found (requires Linux 3.14)\n");
        printf("\tFalling back to raw msr support\n\n");
        return -1;
    }
    fscanf(fff, "%d", &type);
    fclose(fff);

    for (i = 0; i < NUM_RAPL_DOMAINS; i++) {

        sprintf(filename, "/sys/bus/event_source/devices/power/events/%s",
                rapl_domain_names[i]);

        fff = fopen(filename, "r");

        if (fff != NULL) {
            fscanf(fff, "event=%x", &config[i]);
            printf("\tEvent=%s Config=%d ", rapl_domain_names[i], config[i]);
            fclose(fff);
        } else {
            continue;
        }

        sprintf(filename, "/sys/bus/event_source/devices/power/events/%s.scale",
                rapl_domain_names[i]);
        fff = fopen(filename, "r");

        if (fff != NULL) {
            fscanf(fff, "%lf", &scale[i]);
            printf("scale=%g ", scale[i]);
            fclose(fff);
        }

        sprintf(filename, "/sys/bus/event_source/devices/power/events/%s.unit",
                rapl_domain_names[i]);
        fff = fopen(filename, "r");

        if (fff != NULL) {
            fscanf(fff, "%s", units[i]);
            printf("units=%s ", units[i]);
            fclose(fff);
        }

        printf("\n");
    }

    for (j = 0; j < total_packages; j++) {

        for (i = 0; i < NUM_RAPL_DOMAINS; i++) {

            fd[i][j] = -1;

            memset(&attr, 0x0, sizeof(attr));
            attr.type = type;
            attr.config = config[i];
            if (config[i] == 0) continue;

            fd[i][j] = perf_event_open(&attr, -1, package_map[j], -1, 0);
            if (fd[i][j] < 0) {
                if (errno == EACCES) {
                    paranoid_value = check_paranoid();
                    if (paranoid_value > 0) {
                        printf("\t/proc/sys/kernel/perf_event_paranoid is %d\n", paranoid_value);
                        printf("\tThe value must be 0 or lower to read system-wide RAPL values\n");
                    }

                    printf("\tPermission denied; run as root or adjust paranoid value\n\n");
                    return -1;
                } else {
                    printf("\terror opening core %d config %d: %s\n\n",
                           package_map[j], config[i], strerror(errno));
                    return -1;
                }
            }
        }
    }

    printf("\n\tSleeping 1 second\n\n");
    sleep(1);

    for (j = 0; j < total_packages; j++) {
        printf("\tPackage %d:\n", j);

        for (i = 0; i < NUM_RAPL_DOMAINS; i++) {

            if (fd[i][j] != -1) {
                read(fd[i][j], &value, 8);
                close(fd[i][j]);

                printf("\t\t%s Energy Consumed: %lf %s\n",
                       rapl_domain_names[i],
                       (double) value * scale[i],
                       units[i]);

            }

        }

    }
    printf("\n");

    return 0;
}

static int rapl_sysfs(int core) {

    char event_names[MAX_PACKAGES][NUM_RAPL_DOMAINS][256];
    char filenames[MAX_PACKAGES][NUM_RAPL_DOMAINS][256];
    char basename[MAX_PACKAGES][256];
    char tempfile[256];
    long long before[MAX_PACKAGES][NUM_RAPL_DOMAINS];
    long long after[MAX_PACKAGES][NUM_RAPL_DOMAINS];
    int valid[MAX_PACKAGES][NUM_RAPL_DOMAINS];
    int i, j;
    FILE *fff;

    printf("\nTrying sysfs powercap interface to gather results\n\n");

    /* /sys/class/powercap/intel-rapl/intel-rapl:0/ */
    /* name has name */
    /* energy_uj has energy */
    /* subdirectories intel-rapl:0:0 intel-rapl:0:1 intel-rapl:0:2 */

    for (j = 0; j < total_packages; j++) {
        i = 0;
        sprintf(basename[j], "/sys/class/powercap/intel-rapl/intel-rapl:%d",
                j);
        sprintf(tempfile, "%s/name", basename[j]);
        fff = fopen(tempfile, "r");
        if (fff == NULL) {
            fprintf(stderr, "\tCould not open %s\n", tempfile);
            return -1;
        }
        fscanf(fff, "%s", event_names[j][i]);
        valid[j][i] = 1;
        fclose(fff);
        sprintf(filenames[j][i], "%s/energy_uj", basename[j]);

        /* Handle subdomains */
        for (i = 1; i < NUM_RAPL_DOMAINS; i++) {
            sprintf(tempfile, "%s/intel-rapl:%d:%d/name",
                    basename[j], j, i - 1);
            fff = fopen(tempfile, "r");
            if (fff == NULL) {
                //fprintf(stderr,"\tCould not open %s\n",tempfile);
                valid[j][i] = 0;
                continue;
            }
            valid[j][i] = 1;
            fscanf(fff, "%s", event_names[j][i]);
            fclose(fff);
            sprintf(filenames[j][i], "%s/intel-rapl:%d:%d/energy_uj",
                    basename[j], j, i - 1);

        }
    }

    /* Gather before values */
    for (j = 0; j < total_packages; j++) {
        for (i = 0; i < NUM_RAPL_DOMAINS; i++) {
            if (valid[j][i]) {
                fff = fopen(filenames[j][i], "r");
                if (fff == NULL) {
                    fprintf(stderr, "\tError opening %s!\n", filenames[j][i]);
                } else {
                    fscanf(fff, "%lld", &before[j][i]);
                    fclose(fff);
                }
            }
        }
    }

    printf("\tSleeping 1 second\n\n");
    sleep(1);

    /* Gather after values */
    for (j = 0; j < total_packages; j++) {
        for (i = 0; i < NUM_RAPL_DOMAINS; i++) {
            if (valid[j][i]) {
                fff = fopen(filenames[j][i], "r");
                if (fff == NULL) {
                    fprintf(stderr, "\tError opening %s!\n", filenames[j][i]);
                } else {
                    fscanf(fff, "%lld", &after[j][i]);
                    fclose(fff);
                }
            }
        }
    }

    for (j = 0; j < total_packages; j++) {
        printf("\tPackage %d\n", j);
        for (i = 0; i < NUM_RAPL_DOMAINS; i++) {
            if (valid[j][i]) {
                printf("\t\t%s\t: %lfJ\n", event_names[j][i],
                       ((double) after[j][i] - (double) before[j][i]) / 1000000.0);
            }
        }
    }
    printf("\n");

    return 0;

}

bool readPowerConsumption(QList<PowerConsumption>& pc) {
    if (systemCpuModel <= 0) {
        systemCpuModel = detect_cpu();
    }
    if (total_packages <=0) {
        detect_packages();
    }

    int result = rapl_msr(systemCpuModel, pc);
    return result == 0;
}

