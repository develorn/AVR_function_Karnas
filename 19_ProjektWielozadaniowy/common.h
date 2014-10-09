/*
 * common.h
 *
 *  Created on: 2010-09-28
 *       Autor: Miros�aw Karda�
 */

#ifndef COMMON_H_
#define COMMON_H_

// Makra upraszczaj�ce dost�p do port�w
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
