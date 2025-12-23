/**
 * @file hello_embedded.c
 * @brief İlk gömülü Linux programı
 * 
 * Bu program gömülü sistemde temel sistem bilgilerini görüntüler.
 * 
 * Derleme (host - kendi makinede):
 *   gcc -o hello hello_embedded.c
 * 
 * Cross-compile (BeagleBone Black için - 32-bit ARM):
 *   arm-linux-gnueabihf-gcc -o hello hello_embedded.c
 * 
 * Cross-compile (Raspberry Pi 64-bit için):
 *   aarch64-linux-gnu-gcc -o hello hello_embedded.c
 * 
 * Çalıştırma:
 *   ./hello
 * 
 * Target'a kopyalama:
 *   scp hello debian@192.168.7.2:~/
 *   scp hello pi@raspberrypi.local:~/
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>

#define LINE_WIDTH 50

void print_header(const char *title);
void print_line(void);
void print_system_info(void);
void print_cpu_info(void);
void print_memory_info(void);

int main(void)
{
    printf("\n");
    print_header("GÖMÜLÜ LINUX MERHABA DÜNYA");
    printf("\n");
    
    print_system_info();
    print_cpu_info();
    print_memory_info();
    
    print_line();
    printf("Program başarıyla tamamlandı!\n\n");
    
    return 0;
}

void print_header(const char *title)
{
    int len = strlen(title);
    int padding = (LINE_WIDTH - len - 2) / 2;
    
    print_line();
    printf("*");
    for (int i = 0; i < padding; i++) printf(" ");
    printf("%s", title);
    for (int i = 0; i < LINE_WIDTH - padding - len - 2; i++) printf(" ");
    printf("*\n");
    print_line();
}

void print_line(void)
{
    for (int i = 0; i < LINE_WIDTH; i++) printf("=");
    printf("\n");
}

void print_system_info(void)
{
    struct utsname sys_info;
    char hostname[256];
    
    printf("\n--- SISTEM BİLGİSİ ---\n");
    
    // Hostname
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        printf("Hostname    : %s\n", hostname);
    }
    
    // uname bilgileri
    if (uname(&sys_info) == 0) {
        printf("Sistem      : %s\n", sys_info.sysname);
        printf("Node        : %s\n", sys_info.nodename);
        printf("Release     : %s\n", sys_info.release);
        printf("Version     : %s\n", sys_info.version);
        printf("Machine     : %s\n", sys_info.machine);
    }
}

void print_cpu_info(void)
{
    FILE *fp;
    char line[256];
    int found_model = 0;
    int found_hardware = 0;
    
    printf("\n--- CPU BİLGİSİ ---\n");
    
    fp = fopen("/proc/cpuinfo", "r");
    if (fp == NULL) {
        printf("CPU bilgisi okunamadı!\n");
        return;
    }
    
    while (fgets(line, sizeof(line), fp)) {
        // Model name (x86 için)
        if (!found_model && strncmp(line, "model name", 10) == 0) {
            char *value = strchr(line, ':');
            if (value) {
                printf("Model       :%s", value + 1);
                found_model = 1;
            }
        }
        
        // Hardware (ARM için)
        if (!found_hardware && strncmp(line, "Hardware", 8) == 0) {
            char *value = strchr(line, ':');
            if (value) {
                printf("Hardware    :%s", value + 1);
                found_hardware = 1;
            }
        }
        
        // CPU implementer (ARM için)
        if (strncmp(line, "CPU implementer", 15) == 0) {
            char *value = strchr(line, ':');
            if (value) {
                printf("Implementer :%s", value + 1);
                break;  // Sadece ilk CPU
            }
        }
    }
    
    fclose(fp);
    
    // CPU sayısı
    long nprocs = sysconf(_SC_NPROCESSORS_ONLN);
    printf("CPU Sayısı  : %ld\n", nprocs);
}

void print_memory_info(void)
{
    FILE *fp;
    char line[256];
    unsigned long total_kb = 0, free_kb = 0, available_kb = 0;
    
    printf("\n--- BELLEK BİLGİSİ ---\n");
    
    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        printf("Bellek bilgisi okunamadı!\n");
        return;
    }
    
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line, "MemTotal: %lu", &total_kb);
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line, "MemFree: %lu", &free_kb);
        } else if (strncmp(line, "MemAvailable:", 13) == 0) {
            sscanf(line, "MemAvailable: %lu", &available_kb);
        }
    }
    
    fclose(fp);
    
    printf("Toplam RAM  : %lu MB\n", total_kb / 1024);
    printf("Boş RAM     : %lu MB\n", free_kb / 1024);
    printf("Kullanılabilir: %lu MB\n", available_kb / 1024);
}
