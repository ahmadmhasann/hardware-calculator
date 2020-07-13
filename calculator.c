/*
 * Calculator.c
 *
 *  Created on: Aug 31, 2019
 *      Author: Ahmed Hassan
 */
#include "macros.h"
#include "types.h"
#include "Keypad.h"
#include "calculator.h"
#include "lcd.h"
#include <util/delay.h>


void calculator_vid_calc (void) {
	while (1) {
		u8 i=0, j=0, keyStatus, keyFlag[16] = {0}, currentOperation=0;
		u8 previousOperation=0, numberEnteredFlag=0, digitsCounter=0, resultFloating=0;
		s32 enteredNumber=0, result=0;
		keypad_vid_init();
		while (1) {
			for (i=0; i<16; i++) {

				/*
				 * CHECKING IF THE BUTTON IS PRESSED
				 */
				keyStatus = keypad_u8_get_button_status(i);
				if (keyStatus == 1 && keyFlag[i]==0) {

					/*
					 * IF ANY BUTTON IS PRESSED, INCREMENTING THE DIGITS COUNTER
					 * AND SHIFTING THE DISPLAY TO LEFT IF NUMBER OF DIGITS EXCEEDS 16 DIGIT
					 */
					digitsCounter++;
					if (digitsCounter>16)
						lcd_vid_write_command(0b00011011);

					/*
					 * CHECKING IF THE KEY PRESSED IS A NUMBER NOT OPERATION
					 */
					if (calculator_u8_get_Real_value(i) < 10) {

						/*
						 * IF THE NUMBER IS PRESSED WHILE SHOWING PREVIOUS OPERATION
						 * (THE LAST OPERATION IS "EQUAL")
						 * THEN CLEARING ALL DISPLAY AND SETTING VARIABLES TO ZERO
						 */
						if (currentOperation == calcEqual) {
							lcd_vid_init();
							result = 0;
							currentOperation = 0;
							resultFloating = 0;
						}
						numberEnteredFlag = 1;
						lcd_vid_write_number(calculator_u8_get_Real_value(i));
						enteredNumber = calculator_u8_get_Real_value(i) + enteredNumber*10;
					}

					/*
					 * THE FOLLOWING IS WHEN THE KEY PRESSED IS AN OPERATION NOT NUMBER
					 */
					else {
						calculator_vid_write_operation (i);
						previousOperation = currentOperation;
						currentOperation = i;


						if (i == calcDelete)
							return;

						/*
						 * IF THE USER PRESSED ANY SIGN, WHILE THE LAST OPERATION IS
						 * STILL EXISTS ON THE LCD (PREVIOUS OPERATION IS EQUAL)
						 */
						if (previousOperation == calcEqual) {
							lcd_vid_init();
							lcd_vid_write_string((u8*)"Ans");
							if (currentOperation==calcMinus)
								previousOperation = calcPlus;
							else
								previousOperation = currentOperation;
						}

						/*
						 * IF THE USER ENTERED ANY OPERATION, AFTER ENTERING THE NUMBER
						 */
						if (numberEnteredFlag) {
							switch (previousOperation) {
							case calcPlus:
								/*
								 * HANDLE THE CASE OF ADDING -VE FLOATING NUMBER
								 * FOR EXAMPLE -2.5+5
								 * -2+5 = 3 > 0
								 * 3--=2
								 * THE ANSWER IS 2.5
								 * FOR EXAMPLE -2.5+1
								 * -2+1 = -1
								 * THE ANSWER IS -1.5
								 */
								if (result<0 && resultFloating!=0 && result+enteredNumber>0)
									result--;
								result+=enteredNumber;
								break;

							case calcMinus:
								/*
								 * HANDLE THE CASE OF SUBSTRACTING +VE FLOATING NUMBER
								 * FOR EXAMPLE 2.5-5
								 * 2-5 = -3 < 0
								 * -3++=-2
								 * THE ANSWER IS -2.5
								 * FOR EXAMPLE 2.5-1
								 * 2-1 = 1>0
								 * THE ANSWER IS 1.5
								 */
								if (result>0 && resultFloating!=0 && result-enteredNumber<0)
									result++;
								result-=enteredNumber;
								break;

							case calcMul:
								resultFloating = resultFloating * enteredNumber;
								result*=enteredNumber;

								/*
								 * IF THE FLOATING PART EXCEEDS 100
								 * THEN THE FLOATING DIVIDED BY 100 WILL BE ADDED TO THE RESULT
								 * AND THE FLOATING WILL BE OLD FLOATING % 100
								 * FOR EXAMPLE
								 * 4.20*6
								 * 4*6=24
								 * 20*6=120
								 * 24+1 = 25
								 * THE ANSWER IS 25.20
								 */
								if (resultFloating >99) {
									result = result + resultFloating/100;
									resultFloating = resultFloating % 100;
								}

								break;
							case calcDiv:
								if (enteredNumber!=0) {
									/*
									 * THE FOLLOWING IS JUST MATHEMATICS :`D
									 */
									if (result>=0)
										resultFloating = resultFloating/enteredNumber + (((result%enteredNumber)*100) / enteredNumber);
									else
										resultFloating = resultFloating/enteredNumber + ((((-1*result)%enteredNumber)*100) / enteredNumber);
									result/=enteredNumber;

								}

								/*
								 * CANT DIVIDE BY ZERO
								 */
								else {
									lcd_vid_set_position(1, 0);
									lcd_vid_write_string((u8*)"MathError");

									/*
									 * WAITING FOE PRESSING CLEAR BUTTON
									 */
									while (1) {
										for (i=0; i<16; i++) {
											keyStatus = keypad_u8_get_button_status(i);
											if (keyStatus == 1 && keyFlag[i]==0) {
												if (i == calcDelete)
													return;
												keyFlag[i] = 1;
											}
											else if (keyStatus == 0) {
												keyFlag[i] = 0;
											}
										}
									}
								}
								break;

							/*
							 * THE FOLLOWING CASE IS WHEN THE OPERATION IS ENTERED AFTER
							 * THE FIRST NUMMBER, THEN THE FIRST NUMBER IS THE RESULT
							 */
							case 0:
								result = enteredNumber;
								break;
							default:
								break;
							}

							if (currentOperation == calcEqual) {

								/*
								 * SHIFTING RIGHT TO INITIAL POSITION
								 */
								for (j=0; j<(digitsCounter-16); j++) {
									lcd_vid_write_command(0b00011111);
								}
								digitsCounter=0;
								lcd_vid_set_position(1, 0);
								lcd_vid_write_number(result);

								/*
								 * SHOWING FLOATING PART ONLY IF EXISTS
								 */
								if (resultFloating>0) {
									lcd_vid_write_charachter('.');
									if (resultFloating<10)
										lcd_vid_write_charachter('0');
									lcd_vid_write_number(resultFloating);
								}

							}

							enteredNumber=0;
							numberEnteredFlag = 0;
						}

						/*
						 * IF THE USER ENTERED ANY OPERATION, AFTER ENTERING ANOTHER OPERATION
						 * THESE ALSO ARE MATHEMATICS
						 */
						else {
							if (currentOperation==calcMinus && previousOperation==calcMinus) {
								previousOperation = calcPlus;
								currentOperation = calcPlus;
							}
							else if (currentOperation==calcMinus && previousOperation==calcPlus) {
								previousOperation = calcMinus;
								currentOperation = calcMinus;
							}
							else if (currentOperation==calcPlus && previousOperation==calcMinus) {
								previousOperation = calcMinus;
								currentOperation = calcMinus;
							}
							else if (currentOperation==calcPlus && previousOperation==calcPlus) {
								previousOperation = calcPlus;
								currentOperation = calcPlus;
							}
							else if (currentOperation==calcMinus && previousOperation==calcMul) {
								result*=-1;
								currentOperation = calcMul;
							}
							else if (currentOperation==calcMinus && previousOperation==calcDiv) {
								result*=-1;
								currentOperation = calcDiv;
							}
							else if (currentOperation==calcPlus && previousOperation==calcDiv) {
								currentOperation = calcDiv;
							}
							else if (currentOperation==calcPlus && previousOperation==calcMul) {
								currentOperation = calcMul;
							}
							else if (currentOperation==calcMinus && previousOperation==0) {
								previousOperation = calcMinus;
								currentOperation = calcMinus;
							}

							/*
							 * CAN`T WRITE +* OR -* OR +/ OR -/
							 * CAN`T WRITE * OR / AS THE FIRST DIGIT
							 */
							else if ((currentOperation==calcMul && previousOperation==calcPlus)
									|| (currentOperation==calcMul && previousOperation==calcMinus)
									|| (currentOperation==calcDiv && previousOperation==calcMul)
									|| (currentOperation==calcMul && previousOperation==calcDiv)
									|| (currentOperation==calcMul && previousOperation==0)
									|| (currentOperation==calcDiv && previousOperation==0) ) {
								calculator_vid_write_operation (i);
								lcd_vid_set_position(1, 0);
								lcd_vid_write_string((u8*)"SyntaxError");
								while (1) {
									for (i=0; i<16; i++) {
										keyStatus = keypad_u8_get_button_status(i);
										if (keyStatus == 1 && keyFlag[i]==0) {
											if (i == calcDelete)
												return;
											keyFlag[i] = 1;
										}
										else if (keyStatus == 0) {
											keyFlag[i] = 0;
										}
									}
								}
							}


						}

					}
					keyFlag[i] = 1;
				}
				else if (keyStatus == 0) {
					keyFlag[i] = 0;
				}
			}


		}
}



}








u8 calculator_u8_get_Real_value (u8 i) {
	u8 value;
	switch (i) {
	case calcZero:
		value = 0;
		break;
	case calcOne:
		value = 1;
		break;
	case calcTwo:
		value = 2;
		break;
	case calcThree:
		value = 3;
		break;
	case calcFour:
		value = 4;
		break;
	case calcFive:
		value = 5;
		break;
	case calcSix:
		value = 6;
		break;
	case calcSeven:
		value = 7;
		break;
	case calcEight:
		value = 8;
		break;
	case calcNine:
		value = 9;
		break;
	default:
		value = 10;
		break;

	}
	return value;
}



void calculator_vid_write_operation (u8 i) {
	switch (i) {
	case calcPlus:
		lcd_vid_write_charachter('+');
		break;
	case calcMinus:
		lcd_vid_write_charachter('-');
		break;
	case calcMul:
		lcd_vid_write_charachter('*');
		break;
	case calcDiv:
		lcd_vid_write_charachter('/');
		break;
	default:
		break;

	}
}

void calculator_vid_run_calculator (void) {
	lcd_vid_init();
	while (1) {
		lcd_vid_clear_screan ();
		lcd_vid_set_position(0, 0);
		calculator_vid_calc();
	}
}

