/********************************************************************
 * @file    getcharTimeout.h
 * @author  Franz Korf 
 *          HAW-Hamburg
 *          Berliner Tor  7
 *          D-20099 Hamburg
 * @version V0.5
 * @date    Apr 2018
 * @brief   Implementation von getchar mit timeout
 *                         
 ******************************************************************
 */

#ifndef _GETCHARTIMEOUT_H
#define _GETCHARTIMEOUT_H

/**
 * @brief Diese Funktion entspricht der Unix Funktion getchar, wobei 
 *        sie um eine timeout ergänzt wurde.
 *        Diese Funktion sollte nicht mit anderen Funktionen zum lesen
 *        von stdin kombiniert weden.
 * @param timeout Timeout in Sekunden.
 * @return        Der return Wert entspricht dem von getchar. Im Falle eines 
 *                Timeout wird EOF zurück gegeben.
 */
extern int  getcharTimeout(unsigned int timeout);

#endif
//EOF

