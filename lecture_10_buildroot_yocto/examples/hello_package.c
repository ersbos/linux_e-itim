/* hello_package.c - Buildroot özel paket örneği
 * Ders 10: Buildroot ve Yocto
 * 
 * Bu basit uygulama, Buildroot'a özel paket olarak nasıl
 * ekleneceğini göstermek için oluşturulmuştur.
 * 
 * Derleme: make
 * Cross-compile: make CROSS_COMPILE=arm-linux-gnueabihf-
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>

#define VERSION "1.0.0"

void print_banner(void)
{
    printf("\n");
    printf("╔══════════════════════════════════════════╗\n");
    printf("║     GOMULU LINUX SISTEMLERI KURSU        ║\n");
    printf("║              Custom Package              ║\n");
    printf("║              Version %s              ║\n", VERSION);
    printf("╚══════════════════════════════════════════╝\n");
    printf("\n");
}

void print_system_info(void)
{
    struct utsname uts;
    
    if (uname(&uts) != 0) {
        perror("uname");
        return;
    }
    
    printf("Sistem Bilgileri:\n");
    printf("  Isletim Sistemi: %s\n", uts.sysname);
    printf("  Hostname: %s\n", uts.nodename);
    printf("  Kernel: %s\n", uts.release);
    printf("  Kernel Version: %s\n", uts.version);
    printf("  Mimari: %s\n", uts.machine);
    printf("\n");
}

void print_cpu_info(void)
{
    FILE *fp;
    char line[256];
    int found = 0;
    
    fp = fopen("/proc/cpuinfo", "r");
    if (fp == NULL) {
        perror("/proc/cpuinfo");
        return;
    }
    
    printf("CPU Bilgileri:\n");
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strncmp(line, "model name", 10) == 0 && !found) {
            printf("  %s", line);
            found = 1;
        }
        if (strncmp(line, "Hardware", 8) == 0) {
            printf("  %s", line);
        }
        if (strncmp(line, "processor", 9) == 0) {
            /* CPU sayısını say */
        }
    }
    
    fclose(fp);
    printf("\n");
}

void print_memory_info(void)
{
    FILE *fp;
    char line[256];
    
    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        perror("/proc/meminfo");
        return;
    }
    
    printf("Bellek Bilgileri:\n");
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strncmp(line, "MemTotal", 8) == 0)
            printf("  %s", line);
        if (strncmp(line, "MemFree", 7) == 0)
            printf("  %s", line);
        if (strncmp(line, "MemAvailable", 12) == 0)
            printf("  %s", line);
    }
    
    fclose(fp);
    printf("\n");
}

int main(int argc, char *argv[])
{
    int verbose = 0;
    int i;
    
    /* Argümanları kontrol et */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0)
            verbose = 1;
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("Kullanim: %s [-v|--verbose] [-h|--help]\n", argv[0]);
            printf("  -v, --verbose  Detayli bilgi goster\n");
            printf("  -h, --help     Bu mesaji goster\n");
            return 0;
        }
    }
    
    print_banner();
    print_system_info();
    
    if (verbose) {
        print_cpu_info();
        print_memory_info();
    }
    
    printf("Bu program Buildroot ile ozel paket olarak derlenebilir.\n");
    printf("Daha fazla bilgi icin notlara bakin.\n\n");
    
    return 0;
}
