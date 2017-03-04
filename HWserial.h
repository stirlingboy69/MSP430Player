#ifndef HW_SERIAL_
#define HW_SERIAL_

extern void HWInitSerial(void);
extern void HWSerial_xmit(unsigned char byte);
extern unsigned char HWSerial_read(void);

#endif //HW_SERIAL_
