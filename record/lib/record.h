
#ifndef __RECORD_H__
#define __RECORD_H__

// Read the currently stored record. Returns 0 if no valid data is stored.
int read_record();

// Write a record value to the EEPROM
void write_record(int value);

// Clear the currently stored value
void erase_record();

#endif
