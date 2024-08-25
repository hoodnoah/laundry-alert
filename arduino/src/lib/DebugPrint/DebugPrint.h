#ifndef DEBUG_PRINT_H
#define DEBUG_PRINT_H

#ifdef DEBUG

#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)

#endif

#ifndef DEBUG
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

#endif