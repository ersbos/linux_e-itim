/* gdb_practice.c - GDB pratik örneği
 * Ders 12: Debugging ve Profiling
 * 
 * Bu program GDB ile debug etme pratiği için tasarlanmıştır.
 * Breakpoint, değişken inceleme, stack trace öğrenmek için kullanılır.
 * 
 * Derleme: gcc -g -O0 -o gdb_practice gdb_practice.c
 * Debug: gdb ./gdb_practice
 * 
 * GDB komutları:
 *   break main          - main'de dur
 *   run                 - çalıştır
 *   next (n)            - sonraki satır
 *   step (s)            - fonksiyona gir
 *   print variable      - değişken değeri
 *   backtrace (bt)      - stack trace
 *   continue (c)        - devam et
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Basit yapı */
struct Person {
    char name[50];
    int age;
    float salary;
};

/* Faktöriyel hesaplama (recursion pratği) */
int factorial(int n)
{
    /* Breakpoint koyun: break factorial */
    if (n <= 1)
        return 1;
    return n * factorial(n - 1);
}

/* Fibonacci dizisi */
int fibonacci(int n)
{
    /* print n ve backtrace deneyin */
    if (n <= 1)
        return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

/* Dizi işleme */
void process_array(int *arr, int size)
{
    int i;
    int sum = 0;
    int max = arr[0];
    int min = arr[0];
    
    /* print arr[0]@5 ile tüm diziyi görüntüleyin */
    for (i = 0; i < size; i++) {
        sum += arr[i];
        if (arr[i] > max) max = arr[i];
        if (arr[i] < min) min = arr[i];
    }
    
    printf("Dizi isleme sonuclari:\n");
    printf("  Toplam: %d\n", sum);
    printf("  Ortalama: %.2f\n", (float)sum / size);
    printf("  Maksimum: %d\n", max);
    printf("  Minimum: %d\n", min);
}

/* Struct işleme */
void process_person(struct Person *p)
{
    /* print *p ile yapıyı görüntüleyin */
    /* print p->name ile alanları görüntüleyin */
    
    printf("Kisi bilgileri:\n");
    printf("  Isim: %s\n", p->name);
    printf("  Yas: %d\n", p->age);
    printf("  Maas: %.2f\n", p->salary);
}

/* Öğrenci kaydı (pointer pratği) */
struct Student {
    char name[50];
    int grades[5];
    float average;
};

void calculate_average(struct Student *s)
{
    int i;
    int sum = 0;
    
    /* watch s->average ile değişimi izleyin */
    for (i = 0; i < 5; i++) {
        sum += s->grades[i];
    }
    
    s->average = (float)sum / 5;
}

/* String işleme */
void string_operations(const char *input)
{
    char buffer[100];
    int len;
    
    /* print input ile string'i görüntüleyin */
    /* x/s input ile hex dump'ı görüntüleyin */
    
    strcpy(buffer, input);
    len = strlen(buffer);
    
    printf("String islemleri:\n");
    printf("  Girdi: %s\n", input);
    printf("  Uzunluk: %d\n", len);
    
    /* Ters çevir */
    for (int i = 0; i < len / 2; i++) {
        char temp = buffer[i];
        buffer[i] = buffer[len - 1 - i];
        buffer[len - 1 - i] = temp;
    }
    
    printf("  Ters: %s\n", buffer);
}

int main(void)
{
    int i;
    
    printf("GDB Pratik Ornegi\n");
    printf("=================\n\n");
    
    /* 1. Basit değişkenler */
    printf("1. Degiskenler\n");
    int x = 10;
    int y = 20;
    int z = x + y;
    printf("   x=%d, y=%d, z=%d\n\n", x, y, z);
    
    /* 2. Factorial (recursion) */
    printf("2. Factorial\n");
    int fact = factorial(5);
    printf("   5! = %d\n\n", fact);
    
    /* 3. Fibonacci */
    printf("3. Fibonacci\n");
    printf("   fib(10) = %d\n\n", fibonacci(10));
    
    /* 4. Dizi işleme */
    printf("4. Dizi Isleme\n");
    int arr[] = {5, 2, 8, 1, 9, 3, 7, 4, 6, 10};
    process_array(arr, 10);
    printf("\n");
    
    /* 5. Struct */
    printf("5. Struct Isleme\n");
    struct Person person = {"Ahmet Yilmaz", 30, 5500.50};
    process_person(&person);
    printf("\n");
    
    /* 6. Dinamik bellek */
    printf("6. Dinamik Bellek\n");
    struct Student *student = malloc(sizeof(struct Student));
    strcpy(student->name, "Mehmet");
    student->grades[0] = 85;
    student->grades[1] = 90;
    student->grades[2] = 78;
    student->grades[3] = 92;
    student->grades[4] = 88;
    calculate_average(student);
    printf("   %s'in ortalamasi: %.2f\n\n", student->name, student->average);
    free(student);
    
    /* 7. String işleme */
    printf("7. String Isleme\n");
    string_operations("Merhaba Dunya");
    printf("\n");
    
    /* 8. Breakpoint testi */
    printf("8. Breakpoint Testi\n");
    for (i = 0; i < 5; i++) {
        /* break gdb_practice.c:175 if i==3 deneyin */
        printf("   Iterasyon: %d\n", i);
    }
    
    printf("\nProgram sonlandi.\n");
    printf("Bu programi GDB ile debug edin ve komutlari ogrenin.\n");
    
    return 0;
}
