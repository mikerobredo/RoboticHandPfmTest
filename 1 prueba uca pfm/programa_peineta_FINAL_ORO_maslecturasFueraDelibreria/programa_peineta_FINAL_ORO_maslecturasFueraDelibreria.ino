#include "pfm.h"
//#define PEINETA 

//parametros 
int t_reseteo = 1000;
int t_refresco = 100;
int step_pfm = 11; // default: 10

//pines usados
int pin_menique = 5;
int pin_anular = 10;
int pin_corazon = 9;
int pin_indice = 11;
int pin_pulgar = 12;
int pin_lectura=13;
int Lectura=0;



//se comunica con la libreria , creo que deberia de estar dentro de la libreria para que se maneje internamente

//filtro de grados
//selecciona el canal a escribir
void imprime_serial(int valor)
{
  Serial.println(valor);
}
void imprime_serial_text(String a)
{
  Serial.println(a);
}
void check()
{
      imprime_serial_text("tiempo en baja");
      imprime_serial(pfm_11.lecturadepulso(13));
      imprime_serial_text("angulo");
      imprime_serial(pfm_11.lectura_de_angulo(13));
}

void crearPFM (int pin, int grados)
{
  if (grados < 0)
    grados = 0;

  if (grados > 180)
    grados = 180;

  int t_off = map (grados, 0, 180,PFM_T_OFF_MAXIMO_US_POR_DEFECTO,PFM_T_OFF_MINIMO_US_POR_DEFECTO);
  switch (pin)
  {
    case 5:
      pfm_5.ajustar_t_off(t_off);
      break;
    case 9:
      pfm_9.ajustar_t_off(t_off);
      break;
    case 10:
      pfm_10.ajustar_t_off(t_off);
      break;

     case 11:
      pfm_11.ajustar_t_off(t_off);
      break;
  }
}


void peineta ()
{
  crearPFM(pin_menique,180); 
  crearPFM(pin_anular, 180); 
  crearPFM(pin_corazon,  0); 
  crearPFM(pin_indice, 180); 
  crearPFM(pin_pulgar, 180); 
}


void setup() 
{
   Serial.begin(9600);
  pinMode(1, OUTPUT);     // Control Motor 1
  pinMode(2, OUTPUT);     // Control Motor 2
  pinMode(5, OUTPUT);     // Salida PFM GRADOS
  pinMode(6, OUTPUT);     // Salida PFM CORRIENTE

  pinMode(3, INPUT);      // Pin analógico por el cual se lee la Intensidad del sensor de corriente en valor pfm que saldrá del micro
  pinMode(4, INPUT);      // Pin de lectura de los pulsos externo
  pinMode(7, INPUT);      // Pin de lectura del potenciometro interno
  pinMode(13,INPUT);      //letura

  digitalWrite(1, HIGH);  // Alto para parada de puente H
  digitalWrite(2, HIGH);  // Alto para parada de puente H

    
// ATmega32u4
#if defined (__AVR_ATmega32U4__)
  pfm_5 = pfm(5);   // Pin 5,  PC6
  pfm_9 = pfm(9);   // Pin 9,  PB5
  pfm_10 = pfm(10); // Pin 10, PB6
  pfm_11 = pfm(11); // Pin 11, PB7

  pfm_5.ajustar_t_off(1500);
  pfm_9.ajustar_t_off(1500);
  pfm_10.ajustar_t_off(1500);
  pfm_11.ajustar_t_off(1500);

//  pfm_5.deshabilitar_canal();
//  pfm_9.deshabilitar_canal();
//  pfm_10.deshabilitar_canal();
//  pfm_11.deshabilitar_canal();
  

// ATtiny84
#elif defined (__AVR_ATtiny84__)
  pfm_5 = pfm(5); // Pin 5, PA5
  pfm_6 = pfm(6); // Pin 6, PA6
 
  pfm_5.ajustar_t_off(1000);
  pfm_6.ajustar_t_off(2000);

//  pfm_5.deshabilitar_canal();
//  pfm_6.deshabilitar_canal();
#endif

// Reseteo
// Comprimir mano en puño
  for (int i = 5; i <= 11 ; i++)
       regularPFM (i, 0, 0);
  delay(2000);

#ifdef PEINETA
  peineta();
  while(1);
#endif
}

int old_val_pfm = 0;

void regularPFM (int pin, int val_pfm, int t_refresco)
{
  switch (pin)
  {
    case 5:
    case 9:
    case 10:
    case 11:
       crearPFM(pin, val_pfm);
       delay(t_refresco);
        break;
    default:
      break;
  }
}




void loop() 
{
  // Comprimir mano en puño
  for (int i = 5; i <= 11 ; i++)
    for (int val_pfm=0; val_pfm <=180; val_pfm+=step_pfm)
      {regularPFM (i, val_pfm, t_refresco);
      check();
      }

  
  
  // Descomprimir mano a saludo
  for (int i = 11; i >= 5 ; i--)
    for (int val_pfm=180; val_pfm >=0; val_pfm-=step_pfm)
       {regularPFM (i, val_pfm, t_refresco);
      check();
      }
 

// Reseteo
// Comprimir mano en puño
  for (int i = 5; i <= 11 ; i++)
       {regularPFM (i, 0, 0);
      check();
       } 
  delay(t_reseteo);
  
       
  // Abre y cierra dedo a dedo
  for (int i = 5; i <= 11 ; i++)
  {
       regularPFM (i, 10, t_refresco);
       regularPFM (i, 150, t_refresco);
      check();
  }
  delay(t_reseteo);

  // Reseteo
// Comprimir mano en puño
  for (int i = 5; i <= 11 ; i++)
       {regularPFM (i, 0, 0);
      check();
       }
  delay(t_reseteo);

}
