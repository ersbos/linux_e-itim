/**
 * @file hello_arm.c
 * @brief Cross-compilation test programı
 * 
 * Bu program sistemin ARM veya x86 olduğunu tespit eder
 * ve temel sistem bilgilerini gösterir.
 * 
 * Derleme:
 *   make              - Host için
 *   make TARGET=bbb   - BeagleBone Black için
 *   make TARGET=rpi   - Raspberry Pi için
 * 
 * Deploy:
 *   make TARGET=bbb deploy
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>

void print_arch_info(void);
void print_system_info(void);
void print_compile_info(void);

int main(void)
{
    printf("\n");
    printf("╔══════════════════════════════════════╗\n");
    printf("║   Cross-Compile Test Programi        ║\n");
    printf("╚══════════════════════════════════════╝\n\n");
    
    print_compile_info();
    print_arch_info();
    print_system_info();
    
    printf("Program basariyla calisti!\n\n");
    return 0;
}

void print_compile_info(void)
{
    printf("=== Derleme Bilgisi ===\n");
    
#if defined(__arm__)
    printf("Mimari: ARM 32-bit\n");
#elif defined(__aarch64__)
    printf("Mimari: ARM 64-bit (aarch64)\n");
#elif defined(__x86_64__)
    printf("Mimari: x86_64\n");
#elif defined(__i386__)
    printf("Mimari: i386\n");
#else
    printf("Mimari: Bilinmiyor\n");
#endif

#ifdef __GNUC__
    printf("Derleyici: GCC %d.%d.%d\n", 
           __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#endif

    printf("Derleme tarihi: %s %s\n", __DATE__, __TIME__);
    printf("\n");
}

void print_arch_info(void)
{
    printf("=== Calisma Zamani Mimari ===\n");
    
    /* Pointer boyutu */
    printf("Pointer boyutu: %zu byte (%zu-bit)\n", 
           sizeof(void*), sizeof(void*) * 8);
    
    /* int boyutu */
    printf("int boyutu: %zu byte\n", sizeof(int));
    
    /* long boyutu */
    printf("long boyutu: %zu byte\n", sizeof(long));
    
    /* Endianness testi */
    unsigned int x = 1;
    char *c = (char*)&x;
    if (*c) {
        printf("Endianness: Little Endian\n");
    } else {
        printf("Endianness: Big Endian\n");
    }
    
    printf("\n");
}

void print_system_info(void)
{
    struct utsname info;
    char hostname[256];
    
    printf("=== Sistem Bilgisi ===\n");
    
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        printf("Hostname: %s\n", hostname);
    }
    
    if (uname(&info) == 0) {
        printf("OS: %s\n", info.sysname);
        printf("Kernel: %s\n", info.release);
        printf("Machine: %s\n", info.machine);
    }
    
    printf("CPU sayisi: %ld\n", sysconf(_SC_NPROCESSORS_ONLN));
    
    printf("\n");
}
