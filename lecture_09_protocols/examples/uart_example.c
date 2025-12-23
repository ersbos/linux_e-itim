/* uart_example.c - UART iletişim örneği
 * Ders 9: İletişim Protokolleri
 * 
 * Bu program seri port üzerinden veri gönderip alır.
 * 
 * Derleme: gcc -o uart_example uart_example.c
 * Çalıştırma: sudo ./uart_example /dev/ttyUSB0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>

#define BUFFER_SIZE 256

/* Hata mesajı ve çıkış */
void exit_sys(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

/* Seri port yapılandırması */
int configure_serial(int fd, int baud_rate)
{
    struct termios tty;

    /* Mevcut ayarları al */
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        return -1;
    }

    /* Baud rate ayarla */
    speed_t speed;
    switch (baud_rate) {
        case 9600:   speed = B9600;   break;
        case 19200:  speed = B19200;  break;
        case 38400:  speed = B38400;  break;
        case 57600:  speed = B57600;  break;
        case 115200: speed = B115200; break;
        default:
            fprintf(stderr, "Desteklenmeyen baud rate: %d\n", baud_rate);
            return -1;
    }
    cfsetispeed(&tty, speed);
    cfsetospeed(&tty, speed);

    /* 8N1 yapılandırması */
    tty.c_cflag &= ~PARENB;        /* Parity yok */
    tty.c_cflag &= ~CSTOPB;        /* 1 stop bit */
    tty.c_cflag &= ~CSIZE;         /* Bit maskesi temizle */
    tty.c_cflag |= CS8;            /* 8 data bit */
    
    /* Flow control kapalı */
    tty.c_cflag &= ~CRTSCTS;
    
    /* Yerel bağlantı, okuma aktif */
    tty.c_cflag |= CREAD | CLOCAL;

    /* Raw mode (canonical kapalı) */
    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ~ECHONL;
    tty.c_lflag &= ~ISIG;

    /* Input processing kapalı */
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

    /* Output processing kapalı */
    tty.c_oflag &= ~OPOST;
    tty.c_oflag &= ~ONLCR;

    /* Okuma timeout ayarları */
    tty.c_cc[VTIME] = 10;   /* 1 saniye timeout (deciseconds) */
    tty.c_cc[VMIN] = 0;     /* Minimum byte yok */

    /* Ayarları uygula */
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int fd;
    char *device = "/dev/ttyUSB0";
    int baud_rate = 115200;
    char send_buf[BUFFER_SIZE];
    char recv_buf[BUFFER_SIZE];
    ssize_t bytes;

    /* Argüman kontrolü */
    if (argc >= 2)
        device = argv[1];
    if (argc >= 3)
        baud_rate = atoi(argv[2]);

    printf("UART Iletisim Ornegi\n");
    printf("====================\n");
    printf("Cihaz: %s\n", device);
    printf("Baud rate: %d\n\n", baud_rate);

    /* Seri portu aç */
    fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
        exit_sys("open");

    /* Yapılandırma */
    if (configure_serial(fd, baud_rate) < 0) {
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Seri port acildi ve yapilandirildi.\n");
    printf("Gonderilecek mesaji yazin (cikis icin 'q'):\n\n");

    /* Ana döngü */
    while (1) {
        printf("> ");
        fflush(stdout);

        /* Kullanıcıdan giriş al */
        if (fgets(send_buf, sizeof(send_buf), stdin) == NULL)
            break;

        /* Çıkış kontrolü */
        if (send_buf[0] == 'q' || send_buf[0] == 'Q')
            break;

        /* Veri gönder */
        bytes = write(fd, send_buf, strlen(send_buf));
        if (bytes < 0)
            exit_sys("write");
        printf("Gonderildi: %zd byte\n", bytes);

        /* Yanıt bekle */
        usleep(100000);  /* 100ms bekle */
        
        bytes = read(fd, recv_buf, sizeof(recv_buf) - 1);
        if (bytes > 0) {
            recv_buf[bytes] = '\0';
            printf("Alindi [%zd byte]: %s", bytes, recv_buf);
        } else if (bytes == 0) {
            printf("Yanit yok (timeout)\n");
        } else {
            perror("read");
        }
        printf("\n");
    }

    close(fd);
    printf("\nSeri port kapatildi.\n");

    return 0;
}
