/*
 * temp_inline.h
 *
 *  Created on: 2010-05-14
 *       Autor: Miros³aw Kardaœ
 */

#ifndef TEMP_INLINE_H_
#define TEMP_INLINE_H_

inline void pomiar_t(void)
{
	DS18X20_start_meas( DS18X20_POWER_EXTERN, NULL );
}

inline void odczyt_t(void)
{
	temperatura.status = DS18X20_read_meas( temperatura.id, &temperatura.subzero, &temperatura.cel, &temperatura.cel_frac_bits);

	if(temperatura.status == DS18X20_OK)
	{
		if(temperatura.subzero) temperatura.wartosc = ((temperatura.cel*10)+temperatura.cel_frac_bits)*-1;
		else temperatura.wartosc = ((temperatura.cel*10)+temperatura.cel_frac_bits);
	}

}

inline int get_temp(void)
{
	return temperatura.wartosc;
}


#endif /* TEMP_INLINE_H_ */
