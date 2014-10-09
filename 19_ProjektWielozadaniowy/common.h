/*
 * common.h
 *
 *  Created on: 2010-09-28
 *       Autor: Miros³aw Kardaœ
 */

#ifndef COMMON_H_
#define COMMON_H_

// Makra upraszczaj¹ce dostêp do portów
// *** PORT
#define PORT(x) SPORT(x)
#define SPORT(x) (PORT##x)
// *** PIN
#define PIN(x) SPIN(x)
#define SPIN(x) (PIN##x)
// *** DDR
#define DDR(x) SDDR(x)
#define SDDR(x) (DDR##x)

#endif /* COMMON_H_ */
