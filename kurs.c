/*
    Использую  WinAPI
*/

#include <windows.h>

#define WINDOW_WIDTH 550 //Ширина окна
#define WINDOW_HEIGHT 600 //Высота окна

#define CITY_RADIUS 15 //Радиус кружка города
#define HORSE_RADIUS 10 //Радиус кружка лошади

#define HORSE_COUNT 5 //Количество лошадей

/*Структура лошади*/
typedef struct {
    int x;
    int y;
    int fromPointX;
    int fromPointY;
} Horse;

HWND hwnd;
HDC hdc;

HBRUSH horseBrush, cityBrush; //Заливка
HPEN blackPen, whitePen; //Обводка

Horse horses[HORSE_COUNT];
DWORD hid1, hid2, hid3, hid4, hid5, did; //Идентификаторы потоков
HANDLE Semaphore1, Semaphore2, Semaphore3, Semaphore4, Semaphore5, Semaphore6, Semaphore7, Semaphore8, Semaphore9, Semaphore10, Semaphore11;

int citisPosX[] = {50, 500}; //Координаты X городов
int citisPosY[] = {50, 500}; //Координаты Y городов
char* citiesNames[] = {"City A", "City B", "City C", "City D", "City F"}; //Название городов

/*Создание цветов для карты и лошадей*/
void createColors() {
    //Заливка для лошадей и городов
    horseBrush = CreateSolidBrush(RGB(0, 255, 0)); //зеленый цвет
    cityBrush = CreateSolidBrush(RGB(255, 0, 0)); //красный цвет
    //Обводка для лошадей и городов
    blackPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); //черный цвет
    whitePen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255)); //белый цвет
}

/*Отрисовка города*/
void drawCity(int x, int y, int index) {
    SelectObject(hdc, cityBrush); //выбор кисти
    SelectObject(hdc, blackPen); //выбор обводки
    Ellipse(hdc, x - CITY_RADIUS, y - CITY_RADIUS, x + CITY_RADIUS, y + CITY_RADIUS);
}

/*Отрисовка карты*/
void drawMap() {
    /*Отрисовка линий*/
    SelectObject(hdc, blackPen);

    /*Вертикальные*/
    /*Лево*/
    MoveToEx(hdc, 50, 50, NULL);
    LineTo(hdc, 50, 500);
    /*Право*/
    MoveToEx(hdc, 500, 50, NULL);
    LineTo(hdc, 500, 500);
    /*Диагональ*/
    MoveToEx(hdc, 50, 50, NULL);
    LineTo(hdc, 500, 500);
    MoveToEx(hdc, 500, 50, NULL);
    LineTo(hdc, 50, 500);

    /*Отрисовка городов*/
    /*Высь*/
    drawCity(50, 50, 0); //левый город
   	drawCity(500, 50, 2); //правый город
    /*Седедина*/
    drawCity(50, 275, 3); //левый город
   	drawCity(275, 275, 4); //цетральный город
   	drawCity(500, 275, 5); //правый город
    /*Низ*/
    drawCity(50, 500, 6); //левый город
   	drawCity(500, 500, 8); //правый город
    /*Диагональ*/
    drawCity(175, 175, 9);
    drawCity(375, 375, 10);
    drawCity(375, 175, 10);
    drawCity(175, 375, 10);

    /*Название городов*/
    TextOut(hdc, 70, 40, citiesNames[0], strlen(citiesNames[0])); //Город A
    TextOut(hdc, 440, 40, citiesNames[1], strlen(citiesNames[1])); //Город B
    TextOut(hdc, 260, 230, citiesNames[2], strlen(citiesNames[2]));//Город С
    TextOut(hdc, 70, 490, citiesNames[3], strlen(citiesNames[3])); //Город D
    TextOut(hdc, 440, 490, citiesNames[4], strlen(citiesNames[4])); //Город F
}

/*Отрисовка лошади*/
void drawHorse(int index) {
    Horse horse = horses[index];
    SelectObject(hdc, horseBrush); //выбор кисти
    SelectObject(hdc, whitePen);  //выбор обводки
    Ellipse(hdc, horse.x - HORSE_RADIUS, horse.y - HORSE_RADIUS, horse.x + HORSE_RADIUS, horse.y + HORSE_RADIUS); //рисование круга
}

/*Создание лошади*/
void createHorse(int xPos, int yPos, int index) {
    Horse horse = {
        .x = xPos,
		.y = yPos,
        .fromPointX = xPos,
        .fromPointY = yPos,
    };
    horses[index] = horse;
}

/*Движение лошади*/
void doStep(Horse *horse, int random, int random2) {
    int fromPointX = horse -> fromPointX;
    int fromPointY = horse -> fromPointY;
    /*Из города A*/
    if(fromPointX == 50 && fromPointY == 50) {
        if(random == 0) {
            //движение вниз
            horse -> y++;       
        }
        else {
            //движение по диагонали
            horse -> x++;
            horse -> y++;
        }
    }
    /*Из города B*/
    if(fromPointX == 500 && fromPointY == 50) {
        if(random == 1) {
            //движение вниз
            horse -> y++;
        }
        else {
            //движение по диагонали
            horse -> x--;
            horse -> y++;
        }
    }
    /*Из города D*/
    if(fromPointX == 50 && fromPointY == 500) {
        if(random == 0) {
            //движение вверх
            horse -> y--;
        }
        else {
            //движение по диагонали
            horse -> x++;
            horse -> y--;
        }
    }
    /*Из города F*/
    if(fromPointX == 500 && fromPointY == 500) {
        if(random == 1) {
            //движение по диагонали
            horse -> x--;
            horse -> y--;
        }
        else {
            //движение вверх
            horse -> y--; 
        }
    }
}

/*Отрисовка всех элементов*/
DWORD WINAPI drawAllThread(void* arg) {
    while (1) {
    drawMap(); //отрисовки карты 
        for (int i = 0; i < HORSE_COUNT; i++) {
            drawHorse(i); // отрисовки лошади
        }
    }
}

/*Нить лошади*/
DWORD WINAPI horseThread(void* arg) {
    //определение номера лошади по номеру потока
    int index = (int)arg;
    //случайное задание идентификатора потопка
    srand(GetCurrentThreadId());
    //случайные, начальные координаты лошади 
    int x = citisPosX[rand() % 2];
    int y = citisPosY[rand() % 2];
    int random = rand() % 2;
    int random2 = rand() % 4;
    //создание лошади
    createHorse(x, y, index);
    do{
        //вызов метода шага лошади
        doStep(&horses[index], random, random2);
        //проверка лошади в точке A
        if (horses[index].x == 50 && horses[index].y == 50){
            WaitForSingleObject(Semaphore1, 5000+rand() % 1000); // вызов семаформа точки A
            //задание новой случайно позиции появления лошади
            int x = citisPosX[rand() % 2];
            int y = citisPosY[rand() % 2];
            int random = rand() % 2;
            createHorse(x, y, index);
        }
        //проверка лошади в точке B
        if (horses[index].x == 50 && horses[index].y == 500) {
            WaitForSingleObject(Semaphore2, 5000+rand() % 1000); // вызов семаформа точки B
            //задание новой случайно позиции появления лошади
            int x = citisPosX[rand() % 2];
            int y = citisPosY[rand() % 2];
            int random = rand() % 2;
            createHorse(x, y, index);
        }
        //проверка лошади в точке C
        if (horses[index].x == 500 && horses[index].y == 50){
            WaitForSingleObject(Semaphore3, 5000+rand() % 1000); // вызов семаформа точки C
            //задание новой случайно позиции появления лошади
            int x = citisPosX[rand() % 2];
            int y = citisPosY[rand() % 2];
            int random = rand() % 2;
            createHorse(x, y, index);
        }
        //проверка лошади в точке D
        if (horses[index].x == 500 && horses[index].y == 500) {
            WaitForSingleObject(Semaphore4, 5000+rand() % 1000); // вызов семаформа точки D
            //задание новой случайно позиции появления лошади
            int x = citisPosX[rand() % 2];
            int y = citisPosY[rand() % 2];
            int random = rand() % 2;
            createHorse(x, y, index);
        }
        //проверка лошади в точке F
        if (horses[index].x == 275 && horses[index].y == 275) {
            WaitForSingleObject(Semaphore5, 5000+rand() % 1000); // вызов семаформа точки F
            //задание новой случайно позиции появления лошади
            int x = citisPosX[rand() % 2];  
            int y = citisPosY[rand() % 2];
            int random = rand() % 2;
            createHorse(x, y, index);
        }
        //временные пункты, точки замены лошадей
        if (horses[index].x == 50 && horses[index].y == 275) {
            WaitForSingleObject(Semaphore6, 5000+rand() % 1000); //вызов семафора точки 1
        }
        if (horses[index].x == 175 && horses[index].y == 175) {
            WaitForSingleObject(Semaphore7, 5000+rand() % 1000); //вызов семафора точки 2
        }
        if (horses[index].x == 175 && horses[index].y == 375) {
            WaitForSingleObject(Semaphore8, 5000+rand() % 1000); //вызов семафора точки 3
        }
        if (horses[index].x == 375 && horses[index].y == 175) {
            WaitForSingleObject(Semaphore9, 5000+rand() % 1000); //вызов семафора точки 4
        }
        if (horses[index].x == 375 && horses[index].y == 375) {
            WaitForSingleObject(Semaphore10, 5000+rand() % 1000); //вызов семафора точки 5
        }
        if (horses[index].x == 500 && horses[index].y == 275) {
            WaitForSingleObject(Semaphore11, 5000+rand() % 1000); //вызов семафора точки 6
        }
        Sleep(10);
    } while(1); 
}

/*Создание семафоров*/
void CreateSemaphores() {
	Semaphore1 = CreateSemaphore(NULL, 5, 5, NULL);
    Semaphore2 = CreateSemaphore(NULL, 5, 5, NULL);
    Semaphore3 = CreateSemaphore(NULL, 5, 5, NULL);
    Semaphore4 = CreateSemaphore(NULL, 5, 5, NULL);
    Semaphore5 = CreateSemaphore(NULL, 5, 5, NULL);
    Semaphore6 = CreateSemaphore(NULL, 5, 5, NULL);
    Semaphore7 = CreateSemaphore(NULL, 5, 5, NULL);
    Semaphore8 = CreateSemaphore(NULL, 5, 5, NULL);
    Semaphore9 = CreateSemaphore(NULL, 5, 5, NULL);
    Semaphore10 = CreateSemaphore(NULL, 5, 5, NULL);
    Semaphore11 = CreateSemaphore(NULL, 5, 5, NULL);
}

/*Создание потоков*/
void CreateThreads() {
    HANDLE horse1 = CreateThread(NULL, 0, horseThread, (void*)0,  0, &hid1);
    HANDLE horse2 = CreateThread(NULL, 0, horseThread, (void*)1,  0, &hid2);
    HANDLE horse3 = CreateThread(NULL, 0, horseThread, (void*)2,  0, &hid3);
    HANDLE horse4 = CreateThread(NULL, 0, horseThread, (void*)3,  0, &hid4);
    HANDLE horse5 = CreateThread(NULL, 0, horseThread, (void*)4,  0, &hid5);
    HANDLE draw = CreateThread(NULL, 0, drawAllThread, (void*)5,  0, &did);
}

/*Обработка событий окна*/
LRESULT WINAPI WinProc(HWND hwnd, UINT tmsg, WPARAM wParam, LPARAM lParam) {
    switch(tmsg) {
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
        case WM_CLOSE: {
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProc(hwnd, tmsg, wParam, lParam);
}

/*Создание окна и запуск всех объектов*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow) {
    MSG msg;
    WNDCLASS w;

    memset(&w,0,sizeof(WNDCLASS)); //выделение памяти классу
    w.lpszClassName = "Yams"; //определение имени класса
    w.lpfnWndProc = WinProc; //процесс
    w.hInstance = hInstance;
    w.hCursor=LoadCursor(NULL, IDC_ARROW); //фиксация значка курсора
    w.hbrBackground = (HBRUSH)(WHITE_BRUSH); //фон окна белый
    RegisterClass(&w); //регистрация класса 

    //создание окна с именем Yams
    hwnd = CreateWindow("Yams", "Yams", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT , NULL, NULL, hInstance, NULL);
    ShowWindow(hwnd,nCmdShow); //показать окно 

    hdc = GetDC(hwnd);
    createColors(); //создание цветов
    CreateSemaphores(); //создание семафоров
    CreateThreads(); //создание потоков
    
    while(GetMessage(&msg,NULL,0,0)) DispatchMessage(&msg);
    return msg.wParam;
}