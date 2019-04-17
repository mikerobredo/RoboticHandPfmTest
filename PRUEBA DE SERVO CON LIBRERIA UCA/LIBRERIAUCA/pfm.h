//------------------------------------------------------------------------------
/// \file
//	Victor Sanchez Corbacho
// 	Javier Villaverde Ramallo
//	8 de Noviembre de 2017 
/// \brief

#ifndef PFM_H
#define PFM_H

#define PFM_CANAL_A                     (1u << 0)
#define PFM_CANAL_B                     (1u << 1)
#define PFM_T_ON_US_POR_DEFECTO         600u   // antes era 10u, se ha cambiado a 20u porque la interrupción tarda más tiempo y genera un pulso en alto inestable
#define PFM_T_OFF_MINIMO_US_POR_DEFECTO 1000u
#define PFM_T_OFF_MAXIMO_US_POR_DEFECTO 2000u
#define PFM_CPU_CLOCK_MHZ               16u

//------------------------------------------------------------------------------
///
class pfm
{
	private:
		uint8_t p_nativo_;

		uint16_t t_on_ciclos_;
		uint16_t t_off_minimo_us_;
		uint16_t t_off_maximo_us_;
		
		uint16_t t_off_ciclos_;
		uint16_t nuevo_t_off_ciclos_;

		bool     activo_;
		bool     cambiar_t_off_;
		bool     siguiente_flanco_es_positivo_;

	public:
        //pfm(void);
        pfm(uint8_t p_nativo,
        	     uint16_t t_on_us = PFM_T_ON_US_POR_DEFECTO,
                 uint16_t t_off_minimo = PFM_T_OFF_MINIMO_US_POR_DEFECTO,
                 uint16_t t_off_maximo = PFM_T_OFF_MAXIMO_US_POR_DEFECTO);
		void habilitar_canal(void);                         
		void deshabilitar_canal(void);        
		void ajustar_t_off(uint16_t nuevo_t_off_us);
		void ajustar_frecuencia(uint16_t frecuencia_hz);
        void manejador_compx_irq_(void);
};

#if defined (__AVR_ATmega32U4__)
    extern pfm pfm_5;
    extern pfm pfm_9;
    extern pfm pfm_10;
    extern pfm pfm_11;

#elif defined (__AVR_ATtiny84__)
    extern pfm pfm_5;
    extern pfm pfm_6;
#endif

#endif // PFM_H
