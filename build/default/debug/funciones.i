# 1 "funciones.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 288 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "E:/descargas/packs/Microchip/PIC18F-K_DFP/1.4.87/xc8\\pic\\include\\language_support.h" 1 3
# 2 "<built-in>" 2
# 1 "funciones.c" 2
# 1 "./funciones.h" 1
# 12 "./funciones.h"
long filtrarFIR1(int in);
long filtrarFIR2(int in);
long filtrarFIR3(int in);



typedef struct coef_iir_2_ord {
  float num[3];
  float den[3];
  float w[3];
} coef_iir_2_ord;

void inicializar_iir_2_ord(float*num, float*den, float*w, coef_iir_2_ord* ir);

float filtrarIIR(float in,coef_iir_2_ord* ir, float gain);
# 1 "funciones.c" 2
# 22 "funciones.c"
const int BL = 31;
const char B[31] = {
    -1088, 880, 1344, -896, -1680, 912, 2128, -912, -2832,
      928, 4080, -928, -6896, 944, 20880, 31888, 20880, 944,
    -6896, -928, 4080, 928, -2832, -912, 2128, 912, -1680,
     -896, 1344, 880, -1088
};


volatile int x[31];
volatile int k=0;

long filtrarFIR1(int in)
{
  int i = 1;
  x[k] = (int)in;
  long y = 0;
  for (i = 1; i <= BL; i++)
  {
    y += (long)B[i - 1] * (long)x[(i + k) % BL];
  }

  k = (k + 1) % BL;
  return y>>12;



}

long filtrarFIR2(int in)
{
  int i = 0;
  x[k] = in;
  int inx = k;
  long y = 0;
  for (i = 0; i < BL; ++i) {
    y += (long)x[inx] * (long)B[i];
    inx = inx != 0 ? inx - 1 : BL - 1;
  }
  k++;
  k = (k >= BL) ? 0 : k;

  return y>>12;
}

long filtrarFIR3(int in)
{
  int i = 0;
  x[k] = in;
  int inx = k;
  char *apuntadorcoef = &B[0];
  int *apuntadorarrc = &x[inx];


  long y = 0;
  for (i = 0; i < BL; ++i) {
    y += (long)(*apuntadorarrc) * (long)(*apuntadorcoef);
    apuntadorcoef++;
    if (inx != 0) {
      apuntadorarrc--;
      inx--;
    }
    else {
      apuntadorarrc = &x[BL - 1];
      inx = BL - 1;
    }
  }
  k++;
  k = (k >= BL) ? 0 : k;


  return y>>12;
}




void inicializar_iir_2_ord(float*num, float*den, float*w, coef_iir_2_ord* ir)
{
  for (int i = 0; i < 3; i++) {
    ir->num[i] = num[i];
    ir->den[i] = den[i];
    ir->w[i] = w[i];
  }
}


float filtrarIIR(float in, coef_iir_2_ord* ir, float gain) {
  float y;






  ir->w[0] = gain*(ir->den[0] * in) - (ir->den[1] * ir->w[1]) - (ir->den[2] * ir->w[2]);
  y = ((ir->num[0] * ir->w[0]) + (ir->num[1] * ir->w[1]) + (ir->num[2] * ir->w[2]));
  ir->w[2] = ir->w[1];
  ir->w[1] = ir->w[0];
  return y;
}
