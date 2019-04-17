//------------------------------------------------------------------------------
/// \file
///
/// \brief

#include <avr/io.h>
#include <avr/interrupt.h>
#include "pfm.h"
#include "wiring_private.h"//añadido


//#include <avr/cores/arduino/wiring_pulse.c> // nofunciona
//extern unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long ); no terminaba de ir
//------------------------------------------------------------------------------

pfm::pfm(uint8_t p_nativo,
                      uint16_t t_on_us,
                      uint16_t t_off_minimo_us,
                      uint16_t t_off_maximo_us)
{
    p_nativo_ = p_nativo;

    t_on_ciclos_ = PFM_CPU_CLOCK_MHZ*t_on_us;
    t_off_minimo_us_ = t_off_minimo_us;
    t_off_maximo_us_ = t_off_maximo_us;

    t_off_ciclos_ = PFM_CPU_CLOCK_MHZ*t_off_minimo_us_;
    nuevo_t_off_ciclos_ = PFM_CPU_CLOCK_MHZ*t_off_minimo_us_;

    activo_ = false;
    cambiar_t_off_ = false;
    siguiente_flanco_es_positivo_ = true;


    // ATmega 32u4
    #if defined (__AVR_ATmega32U4__)
        // Configuración del pin de salida del timer
        switch (p_nativo_)
        {
            // Timer 3 Channel A
            case 5:
                
                DDRC  &= ~(1u << PC6);  // Disable Pin PC6 as INPUT, Pin 5 
                PORTC &= ~(1u << PC6);  // PC6, Input No pullup   

                TCCR3A &= ~(3u << 6);   // TCCR3A[7:6] = 00.    Normal port operation, OC3A disconnected
                TIMSK3 &= ~(1u << 1);   // TIMSK3[1] = 0.       Disable Timer/Counter3 Output Compare A Match interrupt.
                OCR3CH = 0;
                OCR3CL = 0;

                // Configuración del Timer 3
                TCCR3A &= ~(3u << 0);   // TCCR3A[1:0] = 00.    WGM1[1:0] Timer Normal Operation
                TCCR3B  = 1u;           // TTCCR1B[2:0] = 001.  No Prescaling; TCCR3A[1:0] = 00.    WGM1[1:0] Timer Normal Operation
                TCCR3C  = 0;            // FOC1n Force Output Compare disabled
                break;

            // Timer 1 Channel A
            case 9:
                DDRB  &= ~(1u << PB5);  // Disable Pin PB5 as INPUT, Pin 9   
                PORTB &= ~(1u << PB5);  // PB5, Input No pullup
                
                TCCR1A &= ~(3u << 6);   // TCCR1A[7:6] = 00.    Normal port operation, OC1A disconnected.
                TIMSK1 &= ~(1u << 1);   // TIMSK1[1] = 0.       Disable Timer/Counter1 Output Compare A Match interrupt.
                OCR1AH = 0;
                OCR1AL = 0;
                break;

            // Timer 1 Channel B
            case 10:
                DDRB  &= ~(1u << PB6);  // Disable Pin PB6 as INPUT, Pin 10
                PORTB &= ~(1u << PB6);  // PB6, Input No pullup
                
                TCCR1A &= ~(3u << 4);   // TCCR1A[5:4] = 00.    Normal port operation, OC1B disconnected
                TIMSK1 &= ~(1u << 2);   // TIMSK1[2] = 0.       Disable Timer/Counter1 Output Compare B Match interrupt.
                OCR1BH = 0;
                OCR1BL = 0;
                break;

            // Timer 1 Channel C
            case 11:
                DDRB  &= ~(1u << PB7);  // Disable Pin PB7 as INPUT, Pin 11
                PORTB &= ~(1u << PB7);  // PB7, Input No pullup

                TCCR1A &= ~(3u << 2);   // TCCR1A[3:2] = 00.    Normal port operation, OC1C disconnected
                TIMSK1 &= ~(1u << 3);   // TIMSK1[3] = 0.       Disable Timer/Counter1 Output Compare C Match interrupt.
                OCR1CH = 0;
                OCR1CL = 0;
                break;
        }
    // ATtiny 84
    #elif defined (__AVR_ATtiny84__)  
        // Configuración del pin de salida del timer
        switch (p_nativo_)
        {
            // Timer 1 Channel A
            case 6: 
                DDRA  &= ~(1u << PA6);  // Disable Pin PA6 as INPUT
                PORTA &= ~(1u << PA6);  // PA6, No pullup

                TCCR1A &= ~(3u << 6);   // TCCR1A[7:6] = 00.    Normal port operation, OC1A disconnected
                TIMSK1 &= ~(1u << 1);   // TIMSK1[1] = 0.       Disable Timer/Counter1 Output Compare A Match interrupt.
                OCR1AH = 0;
                OCR1AL = 0;
                break;

            // Timer 1 Channel B
            case 5:
                DDRA  &= ~(1u << PA5);  // Disable Pin PA5 as INPUT 
                PORTA &= ~(1u << PA5);  // PA5, No pullup

                TCCR1A &= ~(3u << 4);   // TCCR1A[5:4] = 00.    Normal port operation, OC1B disconnected
                TIMSK1 &= ~(1u << 2);   // TIMSK1[2] = 0.       Disable Timer/Counter1 Output Compare B Match interrupt.
                OCR1BH = 0;
                OCR1BL = 0;
                break;
        }
    #endif 


    // ATmega 32u4 + ATtiny84
    // Configuración del Timer 1
    TCCR1A &= ~(3u << 0);   // TCCR3A[1:0] = 00.    WGM1[1:0] Timer Normal Operation
    TCCR1B  = 1u;           // TTCCR1B[2:0] = 001.  No Prescaling; TCCR3A[1:0] = 00.    WGM1[1:0] Timer Normal Operation
    TCCR1C  = 0;            // FOC1n Force Output Compare disabled


                            // TCCR1A[7:6]  = 00.   Normal port operation, OC1A disconnected
                            // TCCR1A[5:4]  = 00.   Normal port operation, OC1B disconnected
                            // TCCR1A[3:2]  = 00.   Normal port operation, OC1C disconnected
    // TCCR1A &= ~(3u << 0);   // TCCR1A[1:0]  = 00.   WGM1[1:0] Timer Normal Operation
    // TCCR1B &= ~(1u << 7);   // TCCR1B[7]    = 0.    ICNC1 Noise Canceler disabled 
    // TCCR1B &= ~(1u << 6);   // TCCR1B[6]    = 0.    ICES1
    //                         // TCCR1B[5]    = 0.    Reserved;
    // TCCR1B &= ~(3u << 3);   // TCCR1B[4:3]  = 00.   WGM1[3:2] Timer Normal Operation
    // TCCR1B |=  (1u << 0);   // TTCCR1B[2:0] = 001.  No Prescaling 
    // TCCR1C  = 0;            // FOC1n Force Output Compare disabled

    

    // TCCR1A = 0;
    // TCCR1B = 1u;
    // TCCR1C = 0;
    // TCCR3A = 0;
    // TCCR3B = 1u;
    // TCCR3C = 0;
    // TCNT1H = 0;
    // TCNT1L = 0;
    // OCR1AH = 0;
    // OCR1AL = 0;
    // OCR1BH = 0;
    // OCR1BL = 0; 
    // ICR1H  = 0;
    // ICR1L  = 0;
    // TIMSK1 = 0;
    // TIFR1  = 0;  
    // TIMSK3 = 0;
    // TIFR3  = 0; 

}

//------------------------------------------------------------------------------
///
void pfm::ajustar_t_off(uint16_t nuevo_t_off_us)
{
    // Ajustar nuevo_t_off_us al rango definido durante la construcción
    if (nuevo_t_off_us > t_off_maximo_us_)
        nuevo_t_off_us = t_off_maximo_us_;
    if (nuevo_t_off_us < t_off_minimo_us_)
        nuevo_t_off_us = t_off_minimo_us_;

    nuevo_t_off_ciclos_ = PFM_CPU_CLOCK_MHZ*nuevo_t_off_us;
    cambiar_t_off_ = true;
    habilitar_canal();
    
}
unsigned long pfm::lecturadepulso(int pin)
{
    
    return (pulseIn(13,LOW));
   
}
int pfm::lectura_de_angulo(int pin)
{
    return map (lecturadepulso(pin), PFM_T_OFF_MAXIMO_US_POR_DEFECTO,PFM_T_OFF_MINIMO_US_POR_DEFECTO,0, 180);

}


//------------------------------------------------------------------------------
///
void pfm::ajustar_frecuencia(uint16_t frecuencia_hz)
{
    uint32_t T_us;
    
     T_us = 1000000/frecuencia_hz;
    
    nuevo_t_off_ciclos_ = PFM_CPU_CLOCK_MHZ*T_us - t_on_ciclos_;
    cambiar_t_off_ = true;        
    habilitar_canal();
    
}

//------------------------------------------------------------------------------
///
void pfm::habilitar_canal(void)
{
    if (!activo_)
    {
        #if defined (__AVR_ATmega32U4__)
            switch (p_nativo_)
            {
                // Habilitar Timer 3 Canal A
                case 5:
                    DDRC   |=  (1u << PC6); // Pin PC6 como OUTPUT
                    PORTC  &= ~(1u << PC6); // Pin PC6 como LOW

                    // Hacer que se produzca una primera interrupción
                    TCCR3A |= 3u << 6;      // TCCR3A[7:6] = 11 => Set on compare match. Victor: El modo toggle no va bien. La fase cambia a veces.
                    OCR3A   = TCNT3 + 100*PFM_CPU_CLOCK_MHZ;  // Primera int. dentro de 100 us.
                    TIMSK3 |= 1u << 1;      // TIMSK3[1] = 1  => When this bit is written to one, and the I-flag in the Status Register is set (interrupts globally enabled), the Timer/Counter3 Output Compare A Match interrupt is enabled.
                    SREG   |= 1u << 7;      // SREG[7]   = 1  => The Global Interrupt Enable bit must be set for the interrupts to be enabled.
                    break;

                // Habilitar Timer 1 Canal A
                case 9:
                    DDRB   |=  (1u << PB5); // Pin PB5 como OUTPUT
                    PORTB  &= ~(1u << PB5); // Pin PB5 como LOW

                    // Hacer que se produzca una primera interrupción
                    TCCR1A |= 3u << 6;      // TCCR1A[7:6] = 11 => Set on compare match. Victor: El modo toggle no va bien. La fase cambia a veces.
                    OCR1A   = TCNT1 + 100*PFM_CPU_CLOCK_MHZ;  // Primera int. dentro de 100 us.
                    TIMSK1 |= 1u << 1;      // TIMSK1[1] = 1  => When this bit is written to one, and the I-flag in the Status Register is set (interrupts globally enabled), the Timer/Counter1 Output Compare A Match interrupt is enabled.
                    SREG   |= 1u << 7;      // SREG[7]   = 1  => The Global Interrupt Enable bit must be set for the interrupts to be enabled.
                    break;

                // Habilitar Timer 1 Canal B
                case 10:
                    DDRB   |=  (1u << PB6); // Pin PB6 como OUTPUT
                    PORTB  &= ~(1u << PB6); // Pin PB6 como LOW

                    // Hacer que se produzca una primera interrupción
                    TCCR1A |= 3u << 4;      // TCCR1A[5:4] = 11 => Set on compare match. Victor: El modo toggle no va bien. La fase cambia a veces.
                    OCR1B   = TCNT1 + 100*PFM_CPU_CLOCK_MHZ;  // Primera int. dentro de 100 us.
                    TIMSK1 |= 1u << 2;      // TIMSK1[2] = 1  => When this bit is written to one, and the I-flag in the Status Register is set (interrupts globally enabled), the Timer/Counter1 Output Compare B Match interrupt is enabled.
                    SREG   |= 1u << 7;      // SREG[7]   = 1  => The Global Interrupt Enable bit must be set for the interrupts to be enabled.
                    break;

                // Habilitar Timer 1 Canal C
                case 11:
                    DDRB   |=  (1u << PB7); // Pin PB7 como OUTPUT
                    PORTB  &= ~(1u << PB7); // Pin PB7 como LOW

                    // Hacer que se produzca una primera interrupción
                    TCCR1A |= 3u << 2;      // TCCR1A[3:2] = 11 => Set on compare match. Victor: El modo toggle no va bien. La fase cambia a veces.
                    OCR1B   = TCNT1 + 100*PFM_CPU_CLOCK_MHZ;  // Primera int. dentro de 100 us.
                    TIMSK1 |= 1u << 3;      // TIMSK1[3] = 1  => When this bit is written to one, and the I-flag in the Status Register is set (interrupts globally enabled), the Timer/Counter1 Output Compare C Match interrupt is enabled.
                    SREG   |= 1u << 7;      // SREG[7]   = 1  => The Global Interrupt Enable bit must be set for the interrupts to be enabled.
                    break;
            }
        
            siguiente_flanco_es_positivo_ = true;
            activo_ = true;

        #elif defined (__AVR_ATtiny84__)
            switch (p_nativo_)
            {
                // Habilitar Timer 1 Canal A
                case 6:
                    DDRA   |=  (1u << PA6); // Pin PA6 como OUTPUT
                    PORTA  &= ~(1u << PA6); // Pin PA6 como LOW

                    // Hacer que se produzca una primera interrupción
                    TCCR1A |= 3u << 6;      // TCCR1A[7:6] = 11 => Set on compare match. Victor: El modo toggle no va bien. La fase cambia a veces.
                    OCR1A   = TCNT1 + 100*PFM_CPU_CLOCK_MHZ;  // Primera int. dentro de 100 us.
                    TIMSK1 |= 1u << 1;      // TIMSK1[1] = 1  => When this bit is written to one, and the I-flag in the Status Register is set (interrupts globally enabled), the Timer/Counter1 Output Compare A Match interrupt is enabled.
                    SREG   |= 1u << 7;      // SREG[7]   = 1  => The Global Interrupt Enable bit must be set for the interrupts to be enabled.
                    break;

                // Habilitar Timer 1 Canal B
                case 5:
                    DDRA   |=  (1u << PA5); // Pin PA5 como OUTPUT
                    PORTA  &= ~(1u << PA5); // Pin PA5 como LOW

                    // Hacer que se produzca una primera interrupción
                    TCCR1A |= 3u << 4;      // TCCR1A[5:4] = 11 => Set on compare match. Victor: El modo toggle no va bien. La fase cambia a veces.
                    OCR1B   = TCNT1 + 100*PFM_CPU_CLOCK_MHZ;  // Primera int. dentro de 100 us.
                    TIMSK1 |= 1u << 2;      // TIMSK1[2] = 1  => When this bit is written to one, and the I-flag in the Status Register is set (interrupts globally enabled), the Timer/Counter1 Output Compare B Match interrupt is enabled.
                    SREG   |= 1u << 7;      // SREG[7]   = 1  => The Global Interrupt Enable bit must be set for the interrupts to be enabled.
                    break;
            }

            siguiente_flanco_es_positivo_ = true;
            activo_ = true;
        #endif        
    }
}

//------------------------------------------------------------------------------
///
void pfm::deshabilitar_canal(void)
{   
    #if defined (__AVR_ATmega32U4__)
        switch (p_nativo_)
        {
            // Deshabilitar Timer 3 Canal A
            case 5:
                DDRC   &= ~(1u << PC6); // Disable Pin PC6 setting as INPUT 
                PORTC  &= ~(1u << PC6); // No pull-up 
                TCCR3A &= ~(3u << 6);   // TCCR3A[7:6] = 00.    Normal port operation, OC3A disconnected.
                TIMSK3 &= ~(1u << 1);   // TIMSK3[1] = 0.       Disable Timer/Counter3 Output Compare A Match interrupt.
                break;

            // Deshabilitar Timer 1 Canal A
            case 9:
                DDRB   &= ~(1u << PB5); // Disable Pin PB5 setting as INPUT 
                PORTB  &= ~(1u << PB5); // No pull-up 
                TCCR1A &= ~(3u << 6);   // TCCR1A[7:6] = 00.    Normal port operation, OC1A disconnected.
                TIMSK1 &= ~(1u << 1);   // TIMSK1[1] = 0.       Disable Timer/Counter1 Output Compare A Match interrupt.
                break;

            // Deshabilitar Timer 1 Canal B
            case 10:
                DDRB   &= ~(1u << PB6); // Disable Pin PB6 setting as INPUT 
                PORTB  &= ~(1u << PB6); // No pull-up 
                TCCR1A &= ~(3u << 4);   // TCCR1A[5:4] = 00.    Normal port operation, OC1B disconnected.
                TIMSK1 &= ~(1u << 2);   // TIMSK1[2] = 0.       Disable Timer/Counter1 Output Compare A Match interrupt.
                break;

            // Deshabilitar Timer 1 Canal C
            case 11:
                DDRB   &= ~(1u << PB7); // Disable Pin PB7 setting as INPUT 
                PORTB  &= ~(1u << PB7); // No pull-up 
                TCCR1A &= ~(3u << 2);   // TCCR1A[3:2] = 00.    Normal port operation, OC1C disconnected.
                TIMSK1 &= ~(1u << 3);   // TIMSK1[3] = 0.       Disable Timer/Counter1 Output Compare C Match interrupt.
                break;
        }
        
        activo_ = false;

    #elif defined (__AVR_ATtiny84__)
        switch (p_nativo_)
        {
            // Deshabilitar Timer 1 Canal A
            case 6:
                DDRA   &= ~(1u << PA6); // Disable Pin PA6 setting as INPUT 
                PORTA  &= ~(1u << PA6); // No pull-up 
                TCCR1A &= ~(3u << 6);   // TCCR1A[7:6] = 00.    Normal port operation, OC1A disconnected.
                TIMSK1 &= ~(1u << 1);   // TIMSK1[1] = 0.       Disable Timer/Counter1 Output Compare A Match interrupt.
                break;

            // Deshabilitar Timer 1 Canal B
            case 5:
                DDRA   &= ~(1u << PA5); // Disable Pin PA5 setting as INPUT 
                PORTA  &= ~(1u << PA5); // No pull-up 
                TCCR1A &= ~(3u << 4);   // TCCR1A[5:4] = 00.    Normal port operation, OC1B disconnected.
                TIMSK1 &= ~(1u << 2);   // TIMSK1[2] = 0.       Disable Timer/Counter1 Output Compare A Match interrupt.
                break;
        }

        activo_ = false;
    #endif
}


//------------------------------------------------------------------------------
///
void pfm::manejador_compx_irq_(void)
{
    if (cambiar_t_off_)
    {
        t_off_ciclos_  = nuevo_t_off_ciclos_;
        cambiar_t_off_ = false;
    }

    if (siguiente_flanco_es_positivo_)
    {
        #if defined (__AVR_ATmega32U4__)
            switch (p_nativo_)
            {
                // Timer 3 Canal A
                case 5:
                    OCR3A  += t_off_ciclos_;
                    TCCR3A |= 1u << 6; 
                    break;

                // Timer 1 Canal A
                case 9:
                    OCR1A  += t_off_ciclos_;
                    TCCR1A |= 1u << 6;   
                    break;

                // Timer 1 Canal B
                case 10:
                    OCR1B  += t_off_ciclos_;
                    TCCR1A |= 1u << 4;
                    break;

                // Timer 1 Canal C
                case 11:
                    OCR1C  += t_off_ciclos_;
                    TCCR1A |= 1u << 2; 
                    break;
            }
        #endif

        #if defined (__AVR_ATtiny84__)
            switch (p_nativo_)
            {
                // Timer 1 Canal A
                case 6: 
                    OCR1A  += t_off_ciclos_;
                    TCCR1A |= 1u << 6;     
                    break;

                // Timer 1 Canal B
                case 5:
                    OCR1B  += t_off_ciclos_;
                    TCCR1A |= 1u << 4; 
                    break;
            }
        #endif 
    }
    else // siguiente_flanco_es_negativo
    {
        #if defined (__AVR_ATmega32U4__)
            switch (p_nativo_)
            {
                // Timer 3 Canal A
                case 5:
                    OCR3A  += t_on_ciclos_;
                    TCCR3A &= ~(1u << 6); 
                    break;
                
                // Timer 1 Canal A
                case 9:
                    OCR1A  += t_on_ciclos_;
                    TCCR1A &= ~(1u << 6);  
                    break;

                // Timer 1 Canal B
                case 10:
                    OCR1B  += t_on_ciclos_;
                    TCCR1A &= ~(1u << 4); 
                    break;

                // Timer 1 Canal C
                case 11:
                    OCR1C  += t_on_ciclos_;
                    TCCR1A &= ~(1u << 2); 
                    break;
            }
        #endif

        #if defined (__AVR_ATtiny84__)
            switch (p_nativo_)
            {
                // Timer 1 Canal A
                case 6:
                    OCR1A  += t_on_ciclos_;
                    TCCR1A &= ~(1u << 6);
                    break;

                // Timer 1 Canal B
                case 5:
                    OCR1B  += t_on_ciclos_;
                    TCCR1A &= ~(1u << 4); 
                    break;
            }
        #endif    
    }
    siguiente_flanco_es_positivo_ = !siguiente_flanco_es_positivo_;
}

//------------------------------------------------------------------------------
///
#if defined (__AVR_ATmega32U4__)    
    pfm pfm_5  = pfm(5);
    pfm pfm_9  = pfm(9);
    pfm pfm_10 = pfm(10); 
    pfm pfm_11 = pfm(11);

#elif defined (__AVR_ATtiny84__)
    pfm pfm_5 = pfm(5);
    pfm pfm_6 = pfm(6);
#endif


//------------------------------------------------------------------------------
///
#if defined (__AVR_ATmega32U4__)    
	ISR(TIMER1_COMPA_vect)
	{
	    pfm_9.manejador_compx_irq_();
	}

	ISR(TIMER1_COMPB_vect)
	{
	    pfm_10.manejador_compx_irq_();
	}

    ISR(TIMER1_COMPC_vect)
    {
        pfm_11.manejador_compx_irq_();
    }

    ISR(TIMER3_COMPA_vect)
    {
        pfm_5.manejador_compx_irq_();
    }

#elif defined (__AVR_ATtiny84__)    

	ISR(TIM1_COMPA_vect)
	{
	    pfm_6.manejador_compx_irq_();
	}

	ISR(TIM1_COMPB_vect)
	{
	    pfm_5.manejador_compx_irq_();
	}
#endif
