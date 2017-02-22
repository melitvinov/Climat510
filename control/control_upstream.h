#ifndef _CONTROL_UPSTREAM_H_
#define _CONTROL_UPSTREAM_H_

void write_output_bit(char fnTepl, char fnMech, char fnclr, char fnSm);
void write_output_register(uint16_t How, uint8_t fType, uint16_t nAddress,char* nErr,void* Ptr);

#endif
