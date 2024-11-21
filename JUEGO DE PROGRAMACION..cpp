#include <windows.h>
#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <ctime> 

#define ARRIBA 72
#define IZQUIERDA 75
#define DERECHA  77
#define ABAJO 80
#define ESC 27
#define PAUSA 'p'

int cuerpo[200][2]; 
int n = 1; 
int tamano_serpiente = 5; 
int x = 10, y = 12; 
int sent = DERECHA; 
char tecla;
int xcom = 30, ycom = 15; 
int xcom_esp = 0, ycom_esp = 0;

int vel = 100; 
int punt = 0; 
bool pausa = false; 
bool comida_especial_activa = false; 
time_t tiempo_comida_especial = 0; 

time_t inicio_juego;         
time_t tiempo_pausa_total = 0; 
time_t tiempo_inicio_pausa; 

int velocidad_original = 100;    
bool velocidad_alterada = false;
time_t inicio_velocidad_alterada; 

void posicion(int x, int y) {
    HANDLE hCon;
    COORD dwPos;
    dwPos.X = x;
    dwPos.Y = y;
    hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hCon, dwPos);
}

void pintar() {
    for (int i = 2; i < 78; i++) {
        posicion(i, 3); printf("%c", 205); 
        posicion(i, 23); printf("%c", 205); 
    }
    for (int v = 4; v < 23; v++) {
        posicion(2, v); printf("%c", 186); 
        posicion(77, v); printf("%c", 186); 
    }
    posicion(2, 3); printf("%c", 201);
    posicion(2, 23); printf("%c", 200); 
    posicion(77, 3); printf("%c", 187); 
    posicion(77, 23); printf("%c", 188); 
}

void guardar_posicion() {
    cuerpo[n][0] = x;
    cuerpo[n][1] = y;
    cuerpo[0][0] = x;
    cuerpo[0][1] = y;
    n++;
    if (n == tamano_serpiente) n = 1; 
}

void dibujar_cuerpo() {
    for (int i = 1; i < tamano_serpiente; i++) {
        posicion(cuerpo[i][0], cuerpo[i][1]); printf("*"); 
    }
}

void borrar_cuerpo() {
    for (int i = 1; i < tamano_serpiente; i++) {
        posicion(cuerpo[i][0], cuerpo[i][1]); printf(" "); 
    }
}

void tecleado() {
    if (_kbhit()) { 
        tecla = _getch();
        switch (tecla) {
            case ARRIBA:
                if (sent != ABAJO) sent = ARRIBA;
                break;
            case ABAJO:
                if (sent != ARRIBA) sent = ABAJO;
                break;
            case DERECHA:
                if (sent != IZQUIERDA) sent = DERECHA;
                break;
            case IZQUIERDA:
                if (sent != DERECHA) sent = IZQUIERDA;
                break;
            case PAUSA:
                pausa = !pausa;
                if (pausa) {
                    tiempo_inicio_pausa = time(NULL); 
                    posicion(31, 13);
                    printf("Juego en pausa. Presiona 'P' para continuar.");
                } else {
                    tiempo_pausa_total += time(NULL) - tiempo_inicio_pausa;
                    posicion(31, 13);
                    printf("                                     ");
                }
                break;
        }
    }
}

void comida() {
    if (x == xcom && y == ycom) { 
        xcom = (rand() % 73) + 4; 
        ycom = (rand() % 19) + 4;

        tamano_serpiente++; 
        punt += 10; 

        posicion(xcom, ycom); printf("%c", '*'); 
    }
}

void comida_especial() {
    time_t tiempo_actual = time(NULL);

    if (!comida_especial_activa && tiempo_actual % 30 == 0) {
        xcom_esp = (rand() % 73) + 4; 
        ycom_esp = (rand() % 19) + 4;
        comida_especial_activa = true;
        tiempo_comida_especial = tiempo_actual;
        posicion(xcom_esp, ycom_esp); printf("%c", '@'); 
    }

    if (comida_especial_activa && (tiempo_actual - tiempo_comida_especial >= 10)) {
        posicion(xcom_esp, ycom_esp); printf(" "); 
        comida_especial_activa = false;
    }

    if (comida_especial_activa && x == xcom_esp && y == ycom_esp) {
        comida_especial_activa = false;
        posicion(xcom_esp, ycom_esp); printf(" "); 
        velocidad_alterada = true;               
        inicio_velocidad_alterada = tiempo_actual; 

        if (rand() % 2 == 0) {
            vel = vel > 50 ? vel - 50 : vel; 
        } else {
            vel = vel < 300 ? vel + 50 : vel; 
        }
    }

    if (velocidad_alterada && (tiempo_actual - inicio_velocidad_alterada >= 10)) {
        vel = velocidad_original;          
        velocidad_alterada = false;       
    }
}

void mostrar_temporizador() {
    time_t tiempo_actual = time(NULL);
    int tiempo_transcurrido = tiempo_actual - inicio_juego - tiempo_pausa_total;

    posicion(60, 2);
    printf("Tiempo: %02d:%02d", tiempo_transcurrido / 60, tiempo_transcurrido % 60);
}

bool game_over() {
    if (y == 3 || y == 23 || x == 2 || x == 77) return true; 
    for (int j = n - 1; j > 0; j--) { 
        if (cuerpo[j][0] == x && cuerpo[j][1] == y)
            return true;
    }
    return false;
}

void mostrar_puntaje_final() {
    posicion(2, 25);
    printf("Puntaje final: %d", punt);
}

int main() {
    srand(time(NULL)); 
    inicio_juego = time(NULL); 
    pintar();
    posicion(xcom, ycom); printf("%c", '*'); 

    while (tecla != ESC && !game_over()) { 
        if (!pausa) {
            borrar_cuerpo();
            guardar_posicion();
            dibujar_cuerpo();
            comida();
            comida_especial();
            mostrar_temporizador(); 
            tecleado();

            switch (sent) { 
                case ARRIBA: y--; break;
                case ABAJO: y++; break;
                case DERECHA: x++; break;
                case IZQUIERDA: x--; break;
            }

            Sleep(vel);
            posicion(4, 2);
            printf("Puntaje: %d", punt);
        } else {
            tecleado();
            Sleep(100);
        }
    }

    mostrar_puntaje_final();
    posicion(0, 26);
    system("pause>null"); 
    return 0;
}

