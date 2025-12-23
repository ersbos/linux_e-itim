/* memory_lock.c - Bellek kilitleme örneği
 * Ders 11: Gerçek Zamanlı Linux
 * 
 * Bu program mlockall ile bellek kilitlemeyi gösterir.
 * Page fault'ları önleyerek determinizm sağlar.
 * 
 * Derleme: gcc -o memory_lock memory_lock.c
 * Çalıştırma: sudo ./memory_lock
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <errno.h>
#include <unistd.h>

#define BUFFER_SIZE (1024 * 1024)  /* 1 MB */
#define PREFAULT_SIZE (8 * 1024)   /* 8 KB stack prefault */

/* Hata mesajı */
void print_error(const char *msg)
{
    fprintf(stderr, "Hata: %s: %s\n", msg, strerror(errno));
}

/* Bellek limitleri göster */
void show_memory_limits(void)
{
    struct rlimit rlim;
    
    printf("Bellek Limitleri:\n");
    
    if (getrlimit(RLIMIT_MEMLOCK, &rlim) == 0) {
        printf("  RLIMIT_MEMLOCK (soft): ");
        if (rlim.rlim_cur == RLIM_INFINITY)
            printf("unlimited\n");
        else
            printf("%lu KB\n", rlim.rlim_cur / 1024);
        
        printf("  RLIMIT_MEMLOCK (hard): ");
        if (rlim.rlim_max == RLIM_INFINITY)
            printf("unlimited\n");
        else
            printf("%lu KB\n", rlim.rlim_max / 1024);
    }
    printf("\n");
}

/* Bellek kullanımı göster */
void show_memory_usage(void)
{
    FILE *fp;
    char line[256];
    
    printf("Bellek Durumu (/proc/self/status):\n");
    
    fp = fopen("/proc/self/status", "r");
    if (fp == NULL) {
        perror("fopen");
        return;
    }
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strncmp(line, "VmRSS:", 6) == 0 ||
            strncmp(line, "VmLck:", 6) == 0 ||
            strncmp(line, "VmSize:", 7) == 0 ||
            strncmp(line, "VmPeak:", 7) == 0) {
            printf("  %s", line);
        }
    }
    
    fclose(fp);
    printf("\n");
}

/* Stack prefault - sayfa hatalarını önle */
void prefault_stack(void)
{
    unsigned char dummy[PREFAULT_SIZE];
    
    /* Stack'i "touch" ederek sayfaları yükle */
    memset(dummy, 0, sizeof(dummy));
    
    printf("Stack prefault: %zu bytes\n", sizeof(dummy));
}

/* Heap prefault */
void *prefault_heap(size_t size)
{
    void *ptr = malloc(size);
    
    if (ptr == NULL) {
        perror("malloc");
        return NULL;
    }
    
    /* Heap'i "touch" ederek sayfaları yükle */
    memset(ptr, 0, size);
    
    printf("Heap prefault: %zu bytes\n", size);
    
    return ptr;
}

int main(void)
{
    void *heap_buffer;
    
    printf("Bellek Kilitleme Ornegi\n");
    printf("=======================\n\n");
    
    /* Limitleri göster */
    show_memory_limits();
    
    /* Kilitleme öncesi durum */
    printf("=== Kilitleme Oncesi ===\n");
    show_memory_usage();
    
    /* Tüm mevcut ve gelecek belleği kilitle */
    printf("mlockall(MCL_CURRENT | MCL_FUTURE) cagiriliyor...\n");
    if (mlockall(MCL_CURRENT | MCL_FUTURE) < 0) {
        print_error("mlockall");
        printf("\nNot: Root yetkisi veya uygun ulimit gereklidir.\n");
        printf("Deneyin: ulimit -l unlimited (root olarak)\n");
        printf("Veya: sudo ./memory_lock\n\n");
        /* Devam et, eğitim amaçlı */
    } else {
        printf("Basarili: Tum bellek RAM'de kilitlendi.\n\n");
    }
    
    /* Stack prefault */
    prefault_stack();
    
    /* Heap prefault */
    heap_buffer = prefault_heap(BUFFER_SIZE);
    
    printf("\n");
    
    /* Kilitleme sonrası durum */
    printf("=== Kilitleme Sonrasi ===\n");
    show_memory_usage();
    
    /* Belirli bir adresi kilitleme örneği */
    printf("=== mlock Ornegi ===\n");
    char small_buffer[4096];
    if (mlock(small_buffer, sizeof(small_buffer)) == 0) {
        printf("mlock: %zu bytes kilitlendi\n", sizeof(small_buffer));
        /* Kilidi aç */
        munlock(small_buffer, sizeof(small_buffer));
        printf("munlock: Kilit açıldı\n");
    } else {
        print_error("mlock");
    }
    
    printf("\n");
    
    /* Gerçek zamanlı iş buraya gelir */
    printf("Real-time kritik islemler burada yapilabilir.\n");
    printf("Bu noktada page fault olusmayacaktir.\n");
    printf("\n");
    
    /* Temizlik */
    if (heap_buffer)
        free(heap_buffer);
    
    /* Tüm kilitleri aç */
    printf("munlockall() cagiriliyor...\n");
    munlockall();
    printf("Tum kilitler acildi.\n\n");
    
    printf("Program sonlandi.\n");
    
    return 0;
}
