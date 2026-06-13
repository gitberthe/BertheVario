#ifndef AR_DEBUGGER_H
#define AR_DEBUGGER_H

#include <Arduino.h>

// #define DEBUG_SERIAL Serial
// #define EN_DEBUG

/*
*   For debuging.
*/

#ifdef EN_DEBUG

/*
 * With New Line
 */
#define DEBUGL(x)       DEBUG_SERIAL.println(x)
#define DEBUGLF(x)      DEBUG_SERIAL.println(F(x))
#define DEBUGL2(x, y)   DEBUG_SERIAL.print(F(x)); DEBUG_SERIAL.println(y)
#define DEBUGLP(x, y)   DEBUG_SERIAL.println(x, y)

/*
 * Without New Line
 */
#define DEBUGX(...)     DEBUG_SERIAL.printf(__VA_ARGS__)
#define DEBUG(x)        DEBUG_SERIAL.print(x)
#define DEBUGF(x)       DEBUG_SERIAL.print(F(x))
#define DEBUGP(x, y)    DEBUG_SERIAL.print(x, y)

#else

/*
 * With New Line
 */
#define DEBUGL(x)       while(false)
#define DEBUGLF(x)      while(false)
#define DEBUGL2(x, y)   while(false)
#define DEBUGLP(x, y)   while(false)

/*
 * Without New Line
 */
#define DEBUGX(...)     while(false)
#define DEBUG(x)        while(false)
#define DEBUGF(x)       while(false)
#define DEBUGP(x, y)    while(false)

#endif

#endif