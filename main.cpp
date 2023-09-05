/*! \mainpage Ejercicio Titulo
 * \date 01/01/2023
 * \author Nombre
 * \section genDesc Descripcion general
 * [Complete aqui con su descripcion]
 *
 * \section desarrollos Observaciones generales
 * [Complete aqui con sus observaciones]
 *
 * \section changelog Registro de cambios
 *
 * |   Fecha    | Descripcion                                    |
 * |:----------:|:-----------------------------------------------|
 * | 01/01/2023 | Creacion del documento                         |
 *
 */



/* Includes ------------------------------------------------------------------*/
#include "mbed.h"
#include <math.h>

/* END Includes --------------------------------------------------------------*/


/* typedef -------------------------------------------------------------------*/

typedef union{
    struct{
        uint8_t bit7:1;
        uint8_t bit6:1;
        uint8_t bit5:1;
        uint8_t bit4:1;
        uint8_t bit3:1;
        uint8_t bit2:1;
        uint8_t bit1:1;
        uint8_t bit0:1;
    }bits;
    uint8_t byte;
}_uByte;

typedef void (*OnButton)(void *button_param);

typedef enum{
    UP,
    FALLING,
    DOWN,
    RISING
}e_estados;

typedef enum{
    INTRO,
    LEVEL,
    START,
    GAME
}e_simon;

typedef struct
{
    e_estados estado;
    uint8_t index;    
    uint32_t press_time;
    OnButton button_function;
    void *button_param;
}_boton;

typedef struct{

uint8_t *secuencia;
uint32_t intervalo;
uint8_t index;
uint8_t length;

}_seq;

/* END typedef ---------------------------------------------------------------*/

/* define --------------------------------------------------------------------*/
#define interval 100
#define LIMIT 0x0F
#define BUTTON_FLAG flags.bits.bit0
#define BUTTON_FREQ 40
/* END define ----------------------------------------------------------------*/

/* hardware configuration ----------------------------------------------------*/

BusOut LED(PB_6,PB_7,PB_14,PB_15);
BusIn BUT(PA_4,PA_5,PA_6,PA_7);
DigitalOut HB(PC_13);
/* END hardware configuration ------------------------------------------------*/


/* Function prototypes -------------------------------------------------------*/
/**
 * @brief detecta el estado actual del boton seleccionado.
 * 
 * @param bot el parametro boton.
 */
void sense_boton(_boton *bot);/**
 * @brief Reproduce una secuencia en los LEDs
 * 
 * @param secuencia  contiene la secuencia de luces
 */
void set_led(_seq *secuencia);
void simon_task(e_simon sim);
/* END Function prototypes ---------------------------------------------------*/


/* Global variables ----------------------------------------------------------*/
Timer mytimer1;
_uByte flags;
_boton boton[4];
_seq inicio;
uint32_t duracion=0;
uint32_t timerled=0;//unsigned int timer entero
uint8_t mask=0;
const uint8_t secuencia_inicio[7]={
    1,3,2,6,4,12,8
};
e_simon simon=INTRO;


/* END Global variables ------------------------------------------------------*/


/* Function prototypes user code ----------------------------------------------*/

void sense_boton(_boton *bot){ 
    
    mask=0;
    mask|=(1<<bot->index);
    if((BUT.read()&mask)!=0){
        BUTTON_FLAG=1;
    }else{
        BUTTON_FLAG=0;
    }
    switch (bot->estado)
        {
        case UP:
            if(!BUTTON_FLAG){
                bot->estado=FALLING;
            }
            break;
        case FALLING:
            if (!BUTTON_FLAG)
            {
                duracion=mytimer1.read_ms();
                bot->estado=DOWN;
            }else{
                bot->estado=UP;
            }
            break;
        case DOWN:
            if (BUTTON_FLAG)
            {
                bot->estado=RISING;
            }
            break;
        case RISING:
            if (BUTTON_FLAG)
            {

                bot->estado=UP;
                bot->press_time=(mytimer1.read_ms()-duracion);
                HB=!HB;
            }else{
                bot->estado=DOWN;
            }

            break;
        default:
            break;
        }
}

void set_led(_seq *secuencia){
    if((mytimer1.read_ms()-timerled)>secuencia->intervalo){
        timerled=mytimer1.read_ms();
        LED.write(secuencia->secuencia[secuencia->index]);
        if(secuencia->index<secuencia->length){
            secuencia->index++;
        }else{
            secuencia->index=0;
        }
    }
}

void simon_task(e_simon *sim){
    switch (*sim)
    {
    case INTRO:
        set_led(&inicio);
        break;
    case LEVEL:


        break;
    
    default:
        break;
    }

}

/* END Function prototypes user code ------------------------------------------*/

int main()
{
/* Local variables -----------------------------------------------------------*/
    flags.byte=0;
    BUT.mode(PullUp);
    inicio.secuencia=(uint8_t*)secuencia_inicio;
    inicio.intervalo=200;
    inicio.index=0;  
    inicio.length=(sizeof(secuencia_inicio)/sizeof(secuencia_inicio[0])) ; 
    uint32_t timerboton=0;

/* END Local variables -------------------------------------------------------*/
    mytimer1.start();

/* User code -----------------------------------------------------------------*/
    for(uint8_t i=0;i<4;i++){
        boton[i].index=i;
    }

    while(1){
            if((mytimer1.read_ms()-timerboton)>BUTTON_FREQ){
                timerboton=mytimer1.read_ms();
                sense_boton(&boton[0]);
                sense_boton(&boton[1]);
                sense_boton(&boton[2]);
                sense_boton(&boton[3]);
            }

        simon_task(&simon);

    	
    }

/* END User code -------------------------------------------------------------*/
}


