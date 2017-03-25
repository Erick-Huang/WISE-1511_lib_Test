#ifndef __HAL_DEF_H__
#define __HAL_DEF_H__

#ifndef TRUE
#define TRUE 1
#else
#ifdef __IAR_SYSTEMS_ICC__
#warning "Macro TRUE already defined"
#endif
#endif

#ifndef FALSE
#define FALSE 0
#else
#ifdef __IAR_SYSTEMS_ICC__
#warning "Macro FALSE already defined"
#endif
#endif

#ifndef SUCCESS
#define SUCCESS 0
#else
#warning "Macro SUCCESS already defined"
#endif

#ifndef FAILED
#ifndef WIN32
#define FAILED  1
#endif
#else
#ifdef __IAR_SYSTEMS_ICC__
#warning "Macro FAILED already defined"
#endif
#endif

#define uint8 unsigned char
#define uint16 unsigned short
#define uint32 unsigned int


#endif
