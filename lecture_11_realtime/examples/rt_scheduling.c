/* rt_scheduling.c - Real-time scheduling örneği
 * Ders 11: Gerçek Zamanlı Linux
 * 
 * Bu program SCHED_FIFO ve SCHED_RR politikalarını gösterir.
 * 
 * Derleme: gcc -o rt_scheduling rt_scheduling.c -lpthread
 * Çalıştırma: sudo ./rt_scheduling
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <time.h>

#define RT_PRIORITY 80
#define ITERATIONS  1000000

/* Hata mesajı ve çıkış */
void exit_sys(const char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}

/* Scheduling policy ismini al */
const char *get_policy_name(int policy)
{
    switch (policy) {
        case SCHED_OTHER: return "SCHED_OTHER (Normal)";
        case SCHED_FIFO:  return "SCHED_FIFO (RT FIFO)";
        case SCHED_RR:    return "SCHED_RR (RT Round-Robin)";
        #ifdef SCHED_DEADLINE
        case SCHED_DEADLINE: return "SCHED_DEADLINE";
        #endif
        default: return "Unknown";
    }
}

/* Mevcut scheduling bilgisini göster */
void show_scheduling_info(void)
{
    struct sched_param param;
    int policy;
    
    policy = sched_getscheduler(0);
    sched_getparam(0, &param);
    
    printf("Mevcut Scheduling:\n");
    printf("  Policy: %s\n", get_policy_name(policy));
    printf("  Priority: %d\n", param.sched_priority);
    printf("\n");
}

/* Priority aralıklarını göster */
void show_priority_ranges(void)
{
    printf("Priority Araliklari:\n");
    printf("  SCHED_FIFO: %d - %d\n",
           sched_get_priority_min(SCHED_FIFO),
           sched_get_priority_max(SCHED_FIFO));
    printf("  SCHED_RR:   %d - %d\n",
           sched_get_priority_min(SCHED_RR),
           sched_get_priority_max(SCHED_RR));
    printf("\n");
}

/* CPU-bound iş yükü simülasyonu */
unsigned long long cpu_work(int iterations)
{
    unsigned long long sum = 0;
    volatile int i;
    
    for (i = 0; i < iterations; i++) {
        sum += i * i;
    }
    
    return sum;
}

/* Zaman ölçümü */
long long time_diff_ns(struct timespec *start, struct timespec *end)
{
    return (end->tv_sec - start->tv_sec) * 1000000000LL +
           (end->tv_nsec - start->tv_nsec);
}

/* Policy ile test yap */
void run_test(int policy, int priority)
{
    struct sched_param param;
    struct timespec start, end;
    long long elapsed_ns;
    unsigned long long result;
    
    printf("Test: %s (priority=%d)\n", get_policy_name(policy), priority);
    
    /* Policy ayarla */
    param.sched_priority = priority;
    if (sched_setscheduler(0, policy, &param) < 0) {
        fprintf(stderr, "  Hata: %s\n", strerror(errno));
        fprintf(stderr, "  (Root yetkisi gerekli olabilir)\n");
        return;
    }
    
    /* Zaman ölçümü ile iş yükü */
    clock_gettime(CLOCK_MONOTONIC, &start);
    result = cpu_work(ITERATIONS);
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    elapsed_ns = time_diff_ns(&start, &end);
    
    printf("  Sonuc: %llu\n", result);
    printf("  Sure: %.3f ms\n", elapsed_ns / 1000000.0);
    printf("\n");
}

int main(void)
{
    struct sched_param original_param;
    int original_policy;
    
    printf("Real-Time Scheduling Ornegi\n");
    printf("===========================\n\n");
    
    /* Orijinal ayarları sakla */
    original_policy = sched_getscheduler(0);
    sched_getparam(0, &original_param);
    
    /* Bilgileri göster */
    show_scheduling_info();
    show_priority_ranges();
    
    /* Belleği kilitle (page fault önlemek için) */
    if (mlockall(MCL_CURRENT | MCL_FUTURE) < 0) {
        perror("mlockall (opsiyonel)");
        /* Devam et, kritik değil */
    }
    
    /* Testler */
    printf("=== Test Sonuclari ===\n\n");
    
    /* Normal scheduling ile test */
    struct sched_param normal_param = {0};
    sched_setscheduler(0, SCHED_OTHER, &normal_param);
    run_test(SCHED_OTHER, 0);
    
    /* SCHED_FIFO ile test */
    run_test(SCHED_FIFO, RT_PRIORITY);
    
    /* SCHED_RR ile test */
    run_test(SCHED_RR, RT_PRIORITY);
    
    /* Orijinal ayarlara geri dön */
    sched_setscheduler(0, original_policy, &original_param);
    
    /* Bellek kilidini aç */
    munlockall();
    
    printf("Test tamamlandi.\n");
    printf("Not: RT sonuclarinin daha tutarli olmasi beklenir.\n");
    
    return 0;
}
