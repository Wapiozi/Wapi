#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/time.h>


#define ScrH 20         // screen hight
#define ScrW 10         // screen width
#define Walls "\u25A0 " //"\xE2\x96\xA1 "
#define Block "\u25A3 "
#define Empty ". "
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"
#define CLEAR_SCREEN "\e[1;1H\e[2J"

//#define Wall "\xE2\x96\xA1 "  //"\xE2\x96\xAB "

int epollfd;

int sock;

//int Quant = 0;

int arr[ScrW + 10][ScrH + 10];
int ServArr[ScrW + 10][ScrH + 10];

struct kord {
    int x, y;
};

struct cent {
    int x, y;
};

struct kord4 {
    struct kord k1, k2, k3, k4;
    struct cent center;
};

void PrintField() {
    printf(CLEAR_SCREEN);
    for (int i = 0; i <= ScrH; i++) {
        //write(1, Walls, 4);
        printf(GRN Walls RESET);
        for (int j = 0; j <= ScrW; j++) {
            if (arr[j][i] > 0) {
                //write(1, "\u25A3 ", 4);
                printf(CYN Block RESET);
            } else {
                //write(1, ". ",2);
                printf(MAG Empty RESET);
            }
        }
        //write(1, Walls, 4);
        printf(GRN Walls RESET);
        printf("    ");
        //write(1,"    ",4);
        //write(1, Walls, 4);
        printf(RED Walls RESET);
        for (int j = 0; j <= ScrW; j++) {
            if (ServArr[j][i] > 0) {
                //write(1, "\u25A3 ", 4);
                printf(YEL Block RESET);
            } else {
                //write(1, ". ",2);
                printf(RED Empty RESET);
            }
        }
        //write(1, Walls, 4);
        printf(RED Walls RESET);
        printf("\n");
        //write(1, "\n", 1);
    }
    for (int j = 0; j <= ScrW + 2; j++) {
      //  write(1, Walls, 4);
        printf(GRN Walls RESET);
    }
    printf("    ");
    //write(1, "    ", 4);
    for (int j = 0; j <= ScrW + 2; j++) {
        //write(1, Walls, 4);
        printf(RED Walls RESET);
    }
    printf("\n");
    //printf("\n there are some blocks in game, by now : %d",Quant);
}
/*
void PrintField() {
    char s = '\n';
    system("clear");
    for (int y = 1; y < ScrH; y++) {
        for (int x = 1; x < ScrW; x++) {
            if (arr[x][y] == 1) {
                write(1, "\u25A0", 3);
                write(1, " ", 1);
            } else if (arr[x][y] == 2 || arr[x][y] == 3) {
                write(1, "\u25A3", 3);
                write(1, " ", 1);
            } else {
                write(1, ".", 1);
                write(1, " ", 1);
            }
        }
        write(1, &s, 1);
    }
}
*/

int contact(struct kord4 k) {
    if (arr[k.k1.x][k.k1.y + 1] != 0) {
        return 1;
    } else if (arr[k.k2.x][k.k2.y + 1] != 0) {
        return 1;
    } else if (arr[k.k3.x][k.k3.y + 1] != 0) {
        return 1;
    } else if (arr[k.k4.x][k.k4.y + 1] != 0) {
        return 1;
    } else
        return 0;
}

void ClearK(struct kord4 k) {
    arr[k.k1.x][k.k1.y] = 0;
    arr[k.k2.x][k.k2.y] = 0;
    arr[k.k3.x][k.k3.y] = 0;
    arr[k.k4.x][k.k4.y] = 0;
}

void AddArr(struct kord4 k) { // later there should be colors added
    arr[k.k1.x][k.k1.y] = 1;
    arr[k.k2.x][k.k2.y] = 1;
    arr[k.k3.x][k.k3.y] = 1;
    arr[k.k4.x][k.k4.y] = 1;
}

int check3x3(struct kord4 *k) {
    if ((arr[k->center.x + 1][k->center.y - 1] == 0) &&
        (arr[k->center.x + 1][k->center.y] == 0) &&
        (arr[k->center.x + 1][k->center.y + 1] == 0) &&
        (arr[k->center.x][k->center.y - 1] == 0) &&           //smth wrong is here
        (arr[k->center.x][k->center.y] == 0) &&
        (arr[k->center.x][k->center.y + 1] == 0) &&
        (arr[k->center.x - 1][k->center.y - 1] == 0) &&
        (arr[k->center.x - 1][k->center.y] == 0) &&
        (arr[k->center.x - 1][k->center.y + 1] == 0) &&
        (k->center.x > 1) && (k->center.x < (ScrW))
        ) {
        //arr[10000][10000] = 1;
        return 1;
    } else {
        return 0;
    }
}

int fall(struct kord4 *k){
  if (contact(*k) == 0) {
      k->k1.y++;
      k->k2.y++;
      k->k3.y++;
      k->k4.y++;
      k->center.y++;
      return 1;
  } else
      return 0;
}
int KeyMovement(struct kord4 *k, char c) {
    if (c == 's') { // move down
        fall(k);
        // if cant move down
    } else if (c == 'a') { // move left
        if ((k->k1.x != 0) && (k->k2.x != 0) && (k->k3.x != 0) &&
            (k->k4.x != 0) && (arr[k->k1.x - 1][k->k1.y] == 0) &&
            (arr[k->k2.x - 1][k->k2.y] == 0) &&
            (arr[k->k3.x - 1][k->k3.y] == 0) &&
            (arr[k->k4.x - 1][k->k4.y] == 0)) {
            k->k1.x--;
            k->k2.x--;
            k->k3.x--;
            k->k4.x--;
            k->center.x--;
        }
    } else if (c == 'd') { // move right
        if ((k->k1.x != ScrW) && (k->k2.x != ScrW) && (k->k3.x != ScrW) &&
            (k->k4.x != ScrW) && (arr[k->k1.x + 1][k->k1.y] == 0) &&
            (arr[k->k2.x + 1][k->k2.y] == 0) &&
            (arr[k->k3.x + 1][k->k3.y] == 0) &&
            (arr[k->k4.x + 1][k->k4.y] == 0)) {
            k->k1.x++;
            k->k2.x++;
            k->k3.x++;
            k->k4.x++;
            k->center.x++;
        }
    } else if (c == 'q') { // rotate left
        if (check3x3(k)) {
            int kx[5], ky[5];
            kx[1] = k->center.x + k->k1.y - k->center.y;
            kx[2] = k->center.x + k->k2.y - k->center.y;
            kx[3] = k->center.x + k->k3.y - k->center.y;
            kx[4] = k->center.x + k->k4.y - k->center.y;

            ky[1] = k->center.y - k->k1.x + k->center.x;
            ky[2] = k->center.y - k->k2.x + k->center.x;
            ky[3] = k->center.y - k->k3.x + k->center.x;
            ky[4] = k->center.y - k->k4.x + k->center.x;

            k->k1.x = kx[1];
            k->k2.x = kx[2];
            k->k3.x = kx[3];
            k->k4.x = kx[4];

            k->k1.y = ky[1];
            k->k2.y = ky[2];
            k->k3.y = ky[3];
            k->k4.y = ky[4];
        }
    } else if (c == 'e') { // rotate right
        if (check3x3(k) == 1) {
            int kx[5], ky[5];
            kx[1] = k->center.x - k->k1.y + k->center.y;
            kx[2] = k->center.x - k->k2.y + k->center.y;
            kx[3] = k->center.x - k->k3.y + k->center.y;
            kx[4] = k->center.x - k->k4.y + k->center.y;

            ky[1] = k->center.y + k->k1.x - k->center.x;
            ky[2] = k->center.y + k->k2.x - k->center.x;
            ky[3] = k->center.y + k->k3.x - k->center.x;
            ky[4] = k->center.y + k->k4.x - k->center.x;

            k->k1.x = kx[1];
            k->k2.x = kx[2];
            k->k3.x = kx[3];
            k->k4.x = kx[4];

            k->k1.y = ky[1];
            k->k2.y = ky[2];
            k->k3.y = ky[3];
            k->k4.y = ky[4];
        }
    }
    return 0;
}

void moveArr(int maxi) {
    for (int i = maxi; i > 0; i--) {
        for (int j = 0; j <= ScrW; j++) {
            arr[j][i] = arr[j][i-1];
        }
    }
}

void DestFL() {
    for (int i = 0; i <= ScrH; i++) {
        int fullline = 1;
        for (int j = 0; j <= ScrW; j++) {
            if (arr[j][i] == 0) {
                fullline = 0;
            }
        }
        if (fullline == 1){
            moveArr(i);
        }
    }
}

void Step(struct kord4 k) {
    struct epoll_event events[3];
    // printf("Step");
    int td = 0;
    for (;;) {
        td++;
        int qofc = epoll_wait(epollfd, events, 2, 300);
        ClearK(k);
        for (int n = 0; n <= qofc; n++) {
            if (events[n].data.fd == 0) {
                char c[16];
                int n = read(0, c, sizeof c);
                // if (n > 0) { KeyMovement(&k, c[n - 1]); }
                for (int i = 0; i < n; i++) {
                    KeyMovement(&k, c[i]);
                }
            }
        }
        DestFL(); // destroy full lines of *
        // if (KeyMovement(k, c) == 0)
        //   break;
        if (contact(k) == 0) {
            if ((td % 2) == 1) {
              fall(&k);
            }
            AddArr(k);
        } else {
            AddArr(k);
            break;
        }

        write(sock,arr,sizeof arr);
        read(sock,ServArr,sizeof arr);
        PrintField();
    }
}

void AddToField(int r) {
    // system("clear");
    // printf("%d",ScrH);
    // char c;
    // read(0,&c,1);
    //Quant++;
    struct kord4 k;

    if (r == 0) { // sqr
        k.k1.y = 0;
        k.k2.y = 0;
        k.k3.y = 1;
        k.k4.y = 1;

        k.k1.x = ScrW / 2;
        k.k2.x = (ScrW / 2) + 1;
        k.k3.x = ScrW / 2;
        k.k4.x = (ScrW / 2) + 1;

        k.center.x = (ScrW / 2) + 1;
        k.center.y = 1;

    } else if (r == 1) { // t

        k.k1.y = 1;
        k.k2.y = 1;
        k.k3.y = 0;
        k.k4.y = 1;

        k.k1.x = ScrW / 2;
        k.k2.x = ScrW / 2 - 1;
        k.k3.x = ScrW / 2;
        k.k4.x = ScrW / 2 + 1;

        k.center.x = ScrW / 2;
        k.center.y = 1;

    } else if (r == 2) { // l
        k.k1.y = 0;
        k.k2.y = 1;
        k.k3.y = 2;
        k.k4.y = 3;

        k.k1.x = ScrW / 2;
        k.k2.x = ScrW / 2;
        k.k3.x = ScrW / 2;
        k.k4.x = ScrW / 2;

        k.center.x = ScrW / 2;
        k.center.y = 1;

    } else if (r == 3) { // z
        k.k1.y = 0;
        k.k2.y = 0;
        k.k3.y = 1;
        k.k4.y = 1;

        k.k1.x = (ScrW / 2) - 1;
        k.k2.x = ScrW / 2;
        k.k3.x = ScrW / 2;
        k.k4.x = (ScrW / 2) + 1;

        k.center.x = ScrW / 2;
        k.center.y = 0;
    } else if (r == 4) { // s
        k.k1.y = 1;
        k.k2.y = 1;
        k.k3.y = 0;
        k.k4.y = 0;

        k.k1.x = (ScrW / 2) - 1;
        k.k2.x = ScrW / 2;
        k.k3.x = ScrW / 2;
        k.k4.x = (ScrW / 2) + 1;

        k.center.x = ScrW / 2;
        k.center.y = 0;
    } else if (r == 5) { // g
        k.k1.y = 0;
        k.k2.y = 0;
        k.k3.y = 1;
        k.k4.y = 2;

        k.k1.x = (ScrW / 2) + 1;
        k.k2.x = ScrW / 2;
        k.k3.x = ScrW / 2;
        k.k4.x = ScrW / 2;

        k.center.x = ScrW / 2;
        k.center.y = 1;
    } else if (r == 6) { // L
        k.k1.y = 0;
        k.k2.y = 1;
        k.k3.y = 2;
        k.k4.y = 2;

        k.k1.x = ScrW / 2;
        k.k2.x = ScrW / 2;
        k.k3.x = ScrW / 2;
        k.k4.x = (ScrW / 2) + 1;

        k.center.x = ScrW / 2;
        k.center.y = 1;
    }

    Step(k);
}

int main(int argc, char const *argv[]) {
    struct epoll_event ev;

    int fl = fcntl(0, F_GETFL);
    fl |= O_NONBLOCK;
    fcntl(0, F_SETFL, fl);

    epollfd = epoll_create1(0);
    // int sockfd;
    if (epollfd == -1)
        return 1;

    ev.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;

    epoll_ctl(epollfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);

    // ev.events = EPOLLOU;
    // ev.data.fd = STDOUT_FILENO;
    // epoll_ctl(epollfd, EPOLL_CTL_ADD, STDOUT_FILENO, &ev);

    struct termios old_tio, new_tio;
    tcgetattr(STDIN_FILENO, &old_tio);
    new_tio = old_tio;
    new_tio.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

    //__________________________________SERV_implementation

    sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in dest;

    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    dest.sin_port = htons(2400);

    int Her = 1; // Hemitated Enp resolv.
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *)&Her, sizeof(Her));

    connect(sock, (struct sockaddr *)&dest, sizeof(struct sockaddr_in));

    //__________________________________SERV_END

    for (int i = 0; i <= ScrW; i++) {
        arr[i][ScrH + 1] = 1;
    }


    struct timeval start;
    gettimeofday(&start, NULL);
    srandom(start.tv_usec);

    for (;;) {
        int r = rand() % 7;

        AddToField(r);
        /*
            0 - Square
            1 - T
            2 - Line
            3 - Z
            4 - S
            5 - G
            6 - L
        */
    }

    /*
        char c;
        for (int i = 0;; i++) {
            read(0, &c, 1);
            write(1, &c, 1);
        }
    */
    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
}
