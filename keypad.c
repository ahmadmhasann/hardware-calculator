/*
 * KeyPad.c
 *
 *  Created on: Aug 30, 2019
 *      Author: Ahmed Hassan
 */

#include "KeyPad_cfg.h"
#include <avr/io.h>
#include <util/delay.h>
#include "types.h"
#include "macros.h"
#include "dio.h"


void keypad_vid_init (void) {
	//SET DIRECTIONS OF COLUMNS TO BE OUTPUT
	dio_vid_set_pin_direction(KeyPadPort, C0, 1);
	dio_vid_set_pin_direction(KeyPadPort, C1, 1);
	dio_vid_set_pin_direction(KeyPadPort, C2, 1);
	dio_vid_set_pin_direction(KeyPadPort, C3, 1);
	//SET DIRECTIONS OF ROWS TO BE INPUT
	dio_vid_set_pin_direction(KeyPadPort, R0, 0);
	dio_vid_set_pin_direction(KeyPadPort, R1, 0);
	dio_vid_set_pin_direction(KeyPadPort, R2, 0);
	dio_vid_set_pin_direction(KeyPadPort, R3, 0);
	//ENABLE PULL UP TO ROWS
	dio_vid_set_pin_value(KeyPadPort, R0, 1);
	dio_vid_set_pin_value(KeyPadPort, R1, 1);
	dio_vid_set_pin_value(KeyPadPort, R2, 1);
	dio_vid_set_pin_value(KeyPadPort, R3, 1);
}
u8 keypad_u8_get_button_status (u8 button) {
	const u8 realColumn [] = {C0, C1, C2, C3};
	const u8 realRow [] = {R0, R1, R2, R3};
	u8 value = 0;
	u8 status = 0;
	u8 row = button/4;
	u8 column = button%4;
	//SET ALL COLUMNS TO BE HIGH
	dio_vid_set_pin_value(KeyPadPort, C0, 1);
	dio_vid_set_pin_value(KeyPadPort, C1, 1);
	dio_vid_set_pin_value(KeyPadPort, C2, 1);
	dio_vid_set_pin_value(KeyPadPort, C3, 1);
	//SET THE COLUMN OF THE BUTTON TO LOW
	dio_vid_set_pin_value(KeyPadPort, realColumn[column], 0);
	//CHECK IF THE ROW OF THE BUTTON IS LOW
	value = dio_u8_read_pin_value(KeyPadPort, realRow[row]);
	if (value == 0) {
		_delay_ms(5);
		value = dio_u8_read_pin_value(KeyPadPort, realRow[row]);
		if (value == 0)
			status = 1;
	}
	return status;
}







