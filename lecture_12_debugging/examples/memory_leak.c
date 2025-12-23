/* memory_leak.c - Bellek sızıntısı örneği (Valgrind için)
 * Ders 12: Debugging ve Profiling
 * 
 * Bu program çeşitli bellek hataları içerir.
 * Valgrind ile analiz için kullanılır.
 * 
 * Derleme: gcc -g -O0 -o memory_leak memory_leak.c
 * 
 * Valgrind analizi:
 *   valgrind --leak-check=full ./memory_leak
 *   valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./memory_leak
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 1. Basit bellek sızıntısı */
void simple_leak(void)
{
    printf("Test 1: Basit bellek sizintisi\n");
    int *ptr = malloc(100 * sizeof(int));
    ptr[0] = 42;
    printf("  Ayrilan: 400 bytes, serbest birakilmadi\n");
    /* free(ptr) unutuldu! */
}

/* 2. Döngüde sızıntı */
void loop_leak(int count)
{
    int i;
    printf("Test 2: Dongu icinde sizinti (%d iterasyon)\n", count);
    
    for (i = 0; i < count; i++) {
        char *str = malloc(64);
        sprintf(str, "String %d", i);
        /* free(str) unutuldu! */
    }
    printf("  Ayrilan: %d x 64 = %d bytes\n", count, count * 64);
}

/* 3. Başlatılmamış bellek kullanımı */
void uninitialized_use(void)
{
    int x;
    int *ptr = malloc(sizeof(int));
    
    printf("Test 3: Baslatilmamis bellek kullanimi\n");
    
    /* Başlatılmamış değişken kullanımı */
    if (x > 0) {  /* x başlatılmadı! */
        printf("  x pozitif\n");
    }
    
    /* Başlatılmamış heap bellek kullanımı */
    printf("  ptr degeri (baslatilamadi): %d\n", *ptr);
    
    free(ptr);
}

/* 4. Geçersiz okuma */
void invalid_read(void)
{
    printf("Test 4: Gecersiz okuma (bounds aşımı)\n");
    
    int *arr = malloc(5 * sizeof(int));
    for (int i = 0; i < 5; i++)
        arr[i] = i;
    
    /* Geçersiz okuma - dizin sınırı aşıldı */
    int value = arr[10];  /* Invalid read! */
    printf("  arr[10] = %d (invalid read)\n", value);
    
    free(arr);
}

/* 5. Geçersiz yazma */
void invalid_write(void)
{
    printf("Test 5: Gecersiz yazma (buffer overflow)\n");
    
    int *arr = malloc(5 * sizeof(int));
    
    /* Geçersiz yazma - dizin sınırı aşıldı */
    arr[10] = 999;  /* Invalid write! */
    printf("  arr[10] = 999 yazildi (invalid write)\n");
    
    free(arr);
}

/* 6. Use after free */
void use_after_free(void)
{
    printf("Test 6: Use after free\n");
    
    int *ptr = malloc(sizeof(int));
    *ptr = 42;
    free(ptr);
    
    /* Serbest bırakılmış belleğe erişim */
    printf("  Freed pointer degeri: %d\n", *ptr);  /* Use after free! */
}

/* 7. Struct içinde sızıntı */
struct Node {
    int data;
    struct Node *next;
    char *name;
};

void struct_leak(void)
{
    printf("Test 7: Struct icinde sizinti\n");
    
    struct Node *node = malloc(sizeof(struct Node));
    node->data = 100;
    node->name = malloc(32);
    strcpy(node->name, "Test Node");
    node->next = NULL;
    
    /* Sadece node serbest bırakıldı, node->name sızdı */
    free(node);
    /* free(node->name) unutuldu! */
    
    printf("  node serbest birakildi ama node->name sizdi\n");
}

void print_menu(void)
{
    printf("\n");
    printf("Bellek Hatasi Ornekleri\n");
    printf("=======================\n");
    printf("1. Basit bellek sizintisi\n");
    printf("2. Dongu icinde sizinti\n");
    printf("3. Baslatilmamis bellek kullanimi\n");
    printf("4. Gecersiz okuma\n");
    printf("5. Gecersiz yazma\n");
    printf("6. Use after free\n");
    printf("7. Struct icinde sizinti\n");
    printf("8. Tum testler\n");
    printf("0. Cikis\n");
    printf("\n");
    printf("Analiz icin: valgrind --leak-check=full ./memory_leak\n");
    printf("\n");
}

int main(void)
{
    int choice;
    
    print_menu();
    printf("Secim: ");
    
    if (scanf("%d", &choice) != 1) {
        printf("Gecersiz giris.\n");
        return 1;
    }
    
    printf("\n");
    
    switch (choice) {
        case 1:
            simple_leak();
            break;
        case 2:
            loop_leak(10);
            break;
        case 3:
            uninitialized_use();
            break;
        case 4:
            invalid_read();
            break;
        case 5:
            invalid_write();
            break;
        case 6:
            use_after_free();
            break;
        case 7:
            struct_leak();
            break;
        case 8:
            simple_leak();
            loop_leak(5);
            uninitialized_use();
            invalid_read();
            invalid_write();
            struct_leak();
            break;
        case 0:
            printf("Cikiliyor...\n");
            return 0;
        default:
            printf("Gecersiz secim.\n");
            return 1;
    }
    
    printf("\nProgram sonlandi. Valgrind ciktisini inceleyin.\n");
    
    return 0;
}
