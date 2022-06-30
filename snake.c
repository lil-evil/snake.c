#include <stdio.h>
#include <ncurses.h>
#include <locale.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define APPLE 'o' /* assets */
#define HEAD '#'

int initGame(int speed, int napples); /* functions declaration */
int render(WINDOW *display, int key, int *uy, int *ux, int *z, int **queue, int *points, int **apples);

int main()
{
    int speed = 15, napples = 12; /* user choice in futurs updates */
    srand(time(0));
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    refresh();

    if (!has_colors())
    {
        char err1[] = "your terminal does not support colors.";
        char err2[] = "press any key to exit.";
        mvprintw(LINES / 2, COLS / 2 - (sizeof err1) / 2, err1);
        mvprintw((LINES / 2) + 1, COLS / 2 - (sizeof err2) / 2, err2);
        getch();
        endwin();
        return 1;
    }

    int retry = 1;
    while (retry)
    {
        retry = initGame(speed, napples); /* start the game */
    }

    endwin();
    return 0;
}

char *convertTime(int t)
{
    char *str = malloc(256);
    if (t < 60)
    {
        sprintf(str, "%ds", t);
    }
    else if (t > 60 && t < 3600)
    {
        sprintf(str, "%dmin %ds", t / 60, t - (t / 60) * 60);
    }
    return str;
}

int initGame(int speed, int napples)
{
    WINDOW *display = newwin(LINES - 3, COLS, 0, 0); /* game display */
    box(display, ACS_VLINE, ACS_HLINE);
    wrefresh(display);

    WINDOW *informations = newwin(3, COLS, LINES - 3, 0); /* game informations */
    box(informations, ACS_VLINE, ACS_HLINE);
    mvwprintw(informations, 1, 2, "[x]exit / [p]pause");
    wrefresh(informations);

    init_pair(1, COLOR_RED, COLOR_BLACK); /* colors */

    int uy = (LINES - 3) / 2, ux = COLS / 2, uz = 0; /* user coordonates */
    int quit = 1, pause = 1;                         /* states */
    int points = 0;                                  /* player points */
    int stime = 0;                                   /* play ts */
    timeout(10 * speed);                             /* input method */
    keypad(stdscr, true);

    char footer[] = "points: %d   time: %s ";

    int *queue[((LINES - 4) * COLS - 1)];
    queue[0] = (int *)malloc(sizeof(int)); /* head */
    queue[0][0] = uy;
    queue[0][1] = ux;
    queue[0][2] = uz;
    queue[0][3] = 0;

    int *apples[((LINES - 4) * COLS - 1)];
    apples[0] = (int *)malloc(sizeof(int));
    apples[0][0] = napples; /* max apple */
    apples[1] = (int *)malloc(sizeof(int));
    apples[1][0] = 0;       /* nb apples */

    int btime = time(NULL);

    int key = -1, sp = 0; /* keyboard, user exit */
    while (quit)
    {

        key = getch();
        if (key == 'x')
        {
            quit = 0;
            sp = 1;
        } /* exit */
        if (key == 'p')
        {
            pause = !pause;
            char p[] = "       ";
            mvwprintw(display, ((LINES - 3) / 2), COLS / 2 - (sizeof p) / 2, p);
        } /* pause */
        if (key == KEY_RESIZE)
        {
            box(display, ACS_VLINE, ACS_HLINE);
            wrefresh(display);
            box(informations, ACS_VLINE, ACS_HLINE);
            mvwprintw(informations, 1, 2, "[x]exit / [p]pause");
        }

        if (pause && quit)
        {
            quit = render(display, key, &uy, &ux, &uz, queue, &points, apples); /* render and update game */
        }
        else
        {
            char p1[] = "       ";
            char p2[] = " PAUSE ";
            char p3[] = "       ";
            wattron(display, A_BOLD);
            mvwprintw(display, ((LINES - 3) / 2), (COLS / 2) - (sizeof p2) / 2, p2);
            wattroff(display, A_BOLD);
            wattron(display, A_DIM);
            mvwprintw(display, ((LINES - 3) / 2) - 1, (COLS / 2) - (sizeof p1) / 2, p1);
            mvwprintw(display, ((LINES - 3) / 2) + 1, (COLS / 2) - (sizeof p3) / 2, p3);
            wattroff(display, A_DIM);
        }
        wrefresh(display);

        stime = time(NULL) - btime;                                                                     /* time handler */
        mvwprintw(informations, 1, COLS / 2 - (sizeof footer) / 2, footer, points, convertTime(stime)); /* informations */
        wrefresh(informations);
    }

    /* gameover */
    if (!sp)
    {
        char msg[] = "GAME OVER!";
        char retry[] = "press [r] to retry";
        char ex[] = "press [x] to exit";
        mvprintw((LINES / 2) - 1, COLS / 2 - (sizeof msg) / 2, msg);
        mvprintw(LINES / 2, COLS / 2 - (sizeof retry) / 2, retry);
        mvprintw((LINES / 2) + 1, COLS / 2 - (sizeof ex) / 2, ex);

        timeout(-1);
        int state=1;
        while(state){
            int k = getch();
            state=0;
            if (k == 'r')
                return 1;
            else if(k == 'x') 
                return 0;
            else state=1;
        }
    }
    return 0;
}

void drawApple(WINDOW *display, int **apples)
{
    int count = 0;
    int lowerx = 2, upperx = COLS - 2;
    int lowery = 2, uppery = LINES - 5;
    for (int i = 0; i <= apples[0][0]; i++){
        if(apples[i+2] != 0)continue;
        int y = (rand() % (uppery - lowery + 1)) + lowery;
        int x = (rand() % (upperx - lowerx + 1)) + lowerx;
        apples[i + 2] = (int *)malloc(sizeof(int));
        apples[i + 2][0] = y;
        apples[i + 2][1] = x;
        apples[1][0]+=1;
    }
    for (int i = 0; i <= apples[0][0]; i++){
        if(apples[i+2]==0)continue;
        mvwprintw(display, apples[i+2][0], apples[i+2][1], "%c", APPLE);
    }
}

int colisions(WINDOW *display, int x, int y, int *z, int *signal, int points, int **queue)
{
    int sig = 1, ret = 1;
    /*if(sig && queue[1] != 0){
        if(queue[1][0]==y && queue[1][1]==x){   //flemme
            sig=0;
            ret=0;
            *signal=-1;
        }
    }*/
    if(sig){
        for (int i = points; i >= 0; i--)
        {
            if (queue[i] == 0)
                continue;
            if (i == 0)
                continue;

            if (queue[i][0] == y && queue[i][1] == x)
            {
                *signal = 3;
                sig = 0;
                ret = 0;
                break;
            }
        }
    }
    if (sig)
    {
        switch (*signal)
        {
        case KEY_UP:
            if (y < 1)
            {
                *signal = 2;
                sig = 0;
                ret = 0;
            }
            break;
        case KEY_DOWN:
            if (y > LINES - 5)
            {
                *signal = 2;
                sig = 0;
                ret = 0;
            }
            break;
        case KEY_LEFT:
            if (x < 1)
            {
                *signal = 2;
                sig = 0;
                ret = 0;
            }
            break;
        case KEY_RIGHT:
            if (x > COLS - 2)
            {
                *signal = 2;
                sig = 0;
                ret = 0;
            }
            break;
        }
    }

    if (mvwinch(display, y, x) == APPLE)
    {
        *signal = 1;
        sig = 0;
    }
    if (sig)
        *signal = 0;
    return ret;
}

int render(WINDOW *display, int key, int *y, int *x, int *z, int **queue, int *points, int **apples){
    /* update y, x */
    int signal = key; /* -1=bad direction, 0=nothing, 1=+point, 2=border, 3=self */
    switch (key)
    {
    case KEY_UP:
        if (colisions(display, *x, *y - 1, z, &signal, *points, queue))
        {
            *y -= 1;
            *z = 0;
        }
        break;
    case KEY_DOWN:
        if (colisions(display, *x, *y + 1, z, &signal, *points, queue))
        {
            *y += 1;
            *z = 2;
        }
        break;
    case KEY_LEFT:
        if (colisions(display, *x - 1, *y, z, &signal, *points, queue))
        {
            *x -= 1;
            *z = 3;
        }
        break;
    case KEY_RIGHT:
        if (colisions(display, *x + 1, *y, z, &signal, *points, queue))
        {
            *x += 1;
            *z = 1;
        }
        break;
    default:
        int ox = *x, oy = *y;
        if (*z == 0)
        {
            oy -= 1;
            signal = KEY_UP;
        }
        else if (*z == 2)
        {
            oy += 1;
            signal = KEY_DOWN;
        }
        else if (*z == 3)
        {
            ox -= 1;
            signal = KEY_LEFT;
        }
        else if (*z == 1)
        {
            ox += 1;
            signal = KEY_RIGHT;
        }
        if (colisions(display, ox, oy, z, &signal, *points, queue))
        {
            *x = ox;
            *y = oy;
        }
        break;
    }
    if (signal == 1)
    {
        *points += 1;
        queue[*points] = (int *)malloc(sizeof(int));
        queue[*points][0] = *y;
        queue[*points][1] = *x;
        queue[*points][2] = *z;
        queue[*points][3] = 0;
        
        for (int i = 0; i <= apples[0][0]; i++){
            if(apples[i+2][0] != *y && apples[i+2][1] != *x) continue;
            apples[i + 2] = 0;
            apples[1][0]-=1;
        }
    }
    drawApple(display, apples);
    /* queue */
    for (int i = *points; i >= 0; i--){
        if (queue[i] == 0)
            continue;
        if (queue[i][3] == 1)
            continue;

        /* update queue */
        if (i == 0)
        {
            queue[i][0] = *y;
            queue[i][1] = *x;
            queue[i][2] = *z;
        }
        else
        {
            queue[i][0] = queue[i - 1][0];
            queue[i][1] = queue[i - 1][1];
            queue[i][2] = queue[i - 1][2];
        }

        /* render queue */
        if (i==0) wattron(display, A_BOLD);
        else wattron(display, A_DIM);
        mvwprintw(display, queue[i][0], queue[i][1], "%c", HEAD);
        if (i==0) wattroff(display, A_BOLD);
        else wattroff(display, A_DIM);
    }
    int ox = queue[*points][1], oy = queue[*points][0], oz = queue[*points][2];
    if (oz == 0)
    {
        oy += 1;
    }
    else if (oz == 2)
    {
        oy -= 1;
    }
    else if (oz == 3)
    {
        ox += 1;
    }
    else if (oz == 1)
    {
        ox -= 1;
    }
    if(signal <2)mvwprintw(display, oy, ox, " ");

    if (signal > 1)
        return 0;
    return 1;
}