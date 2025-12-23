/* segfault_example.c - Kasıtlı segfault örneği (Core dump için)
 * Ders 12: Debugging ve Profiling
 * 
 * Bu program kasıtlı olarak segmentation fault oluşturur.
 * Core dump analizi eğitimi için kullanılır.
 * 
 * Derleme: gcc -g -O0 -o segfault_example segfault_example.c
 * 
 * Core dump etkinleştirme:
 *   ulimit -c unlimited
 * 
 * Çalıştırma ve analiz:
 *   ./segfault_example
 *   coredumpctl gdb  # veya: gdb ./segfault_example core
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Null pointer dereference */
void null_pointer_crash(void)
{
    printf("Null pointer dereference testi...\n");
    int *ptr = NULL;
    *ptr = 42;  /* SIGSEGV! */
}

/* Stack buffer overflow */
void stack_overflow(void)
{
    printf("Stack overflow testi...\n");
    char buffer[8];
    strcpy(buffer, "Bu cok uzun bir string ve buffer'a sigmiyor!");
}

/* Use after free */
void use_after_free(void)
{
    printf("Use after free testi...\n");
    int *ptr = malloc(sizeof(int));
    *ptr = 100;
    free(ptr);
    printf("Freed value: %d\n", *ptr);  /* Undefined behavior */
}

/* Double free */
void double_free(void)
{
    printf("Double free testi...\n");
    int *ptr = malloc(sizeof(int));
    free(ptr);
    free(ptr);  /* Double free! */
}

/* Recursive function (stack exhaustion) */
void recursive_crash(int n)
{
    char buffer[1024];  /* Stack'i hızlıca tüket */
    buffer[0] = n;
    printf("Recursion depth: %d\n", n);
    recursive_crash(n + 1);
}

void print_menu(void)
{
    printf("\n");
    printf("Crash Test Menu\n");
    printf("===============\n");
    printf("1. Null pointer dereference\n");
    printf("2. Stack buffer overflow\n");
    printf("3. Use after free\n");
    printf("4. Double free\n");
    printf("5. Stack exhaustion (recursion)\n");
    printf("0. Cikis\n");
    printf("\n");
    printf("UYARI: Bu program kasitli olarak coker!\n");
    printf("Core dump analizi icin: ulimit -c unlimited\n");
    printf("\n");
    printf("Secim: ");
}

int main(void)
{
    int choice;
    
    printf("Debugging Ornegi - Crash Test\n");
    printf("=============================\n");
    printf("Bu program GDB ile debug etme pratiği için tasarlanmıştır.\n");
    
    print_menu();
    
    if (scanf("%d", &choice) != 1) {
        printf("Gecersiz giris.\n");
        return 1;
    }
    
    switch (choice) {
        case 1:
            null_pointer_crash();
            break;
        case 2:
            stack_overflow();
            break;
        case 3:
            use_after_free();
            break;
        case 4:
            double_free();
            break;
        case 5:
            recursive_crash(1);
            break;
        case 0:
            printf("Cikiliyor...\n");
            return 0;
        default:
            printf("Gecersiz secim.\n");
            return 1;
    }
    
    printf("Bu satira ulasilmadi (program coktu).\n");
    
    return 0;
}
