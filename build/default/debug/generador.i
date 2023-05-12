# 1 "generador.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 288 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "E:/descargas/packs/Microchip/PIC18F-K_DFP/1.4.87/xc8\\pic\\include\\language_support.h" 1 3
# 2 "<built-in>" 2
# 1 "generador.c" 2
# 1 "./generador.h" 1
# 18 "./generador.h"
typedef struct generador {
  char entrada;
  char salida;
  char frecuencia;
  int cont;
  int periodo;
  int personal;
  char error;

} generador;

int generadorSenal(generador* gen);
void inicializarGenerador(generador* gen, char entrada, char salida,int periodo,char frecuencia);
void SenalPersonalizada(generador* gen,int personal);
# 1 "generador.c" 2


int coseno[100] = {
  1023, 1021, 1015, 1005, 991, 973, 951, 926, 896, 864, 828, 788, 746, 700, 652, 601, 548, 493, 436, 377,
  316, 254, 192, 128, 64, 0, -64, -128, -192, -254, -316, -377, -436, -493, -548, -601, -652, -700, -746, -788,
  -828, -864, -896, -926, -951, -973, -991, -1005, -1015, -1021, -1023, -1021, -1015, -1005, -991, -973, -951, -926, -896, -864,
  -828, -788, -746, -700, -652, -601, -548, -493, -436, -377, -316, -254, -192, -128, -64, -0, 64, 128, 192, 254,
  316, 377, 436, 493, 548, 601, 652, 700, 746, 788, 828, 864, 896, 926, 951, 973, 991, 1005, 1015, 1021
};
# 19 "generador.c"
void SenalPersonalizada(generador* gen,int personal)
{
  gen->personal= (gen->entrada==16)? gen->personal+personal : personal;
}

int generadorSenal(generador* gen)
{
  int walk = -1;
  switch (gen->entrada)
  {
    case 2:
      if (gen->cont > gen->periodo)
      {
        gen->cont = 0;
        if (walk)
          walk = 0;
        else if (gen->salida == 4)
          walk = 1;
        else
          walk = 256;
      }
      gen->cont++;
      break;
    case 8:
    case 16:
      walk =gen->personal;
      break;
    case 1:
      if (gen->cont > gen->periodo)
      {
        gen->cont = 0;
        if (gen->salida == 4)
          walk = 1;
        else
          walk = 256;
      }
      else {
        walk = 0;
      }
      gen->cont++;
      break;
    case 4:
      walk = coseno[gen->cont];
      gen->cont += gen->frecuencia;
      gen->cont = (gen->cont >= 100) ? 0 : gen->cont ;


    default:
      gen->error = 1;
  }
  return walk;
}

void inicializarGenerador(generador* gen, char entrada, char salida, int periodo, char frecuencia)
{
  gen->entrada = entrada;
  gen->salida = salida;
  gen->periodo = periodo;
  gen->personal=-1;
  gen->frecuencia = frecuencia;
  gen->cont = 0;
  gen->error = 0;
}
