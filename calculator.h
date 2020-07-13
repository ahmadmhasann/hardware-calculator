/*
 * Calculator.h
 *
 *  Created on: Aug 31, 2019
 *      Author: Ahmed Hassan
 */

#ifndef CALCULATOR_H_
#define CALCULATOR_H_

#define calcOne 8
#define calcTwo 9
#define calcThree 10
#define calcFour 4
#define calcFive 5
#define calcSix 6
#define calcSeven 0
#define calcEight 1
#define calcNine 2
#define calcZero 13
#define calcClear 12
#define calcPlus 15
#define calcMinus 11
#define calcMul 7
#define calcDiv 3
#define calcEqual 14
#define calcDelete 12

u8 calculator_u8_get_Real_value (u8 i);
void calculator_vid_write_operation (u8 i);
void calculator_vid_calc (void);
void calculator_vid_run_calculator (void);


#endif /* CALCULATOR_H_ */
