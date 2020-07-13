/*
 * calculator.c
 *
 * Created: 6/22/2020 07:36:21
 * Author : Hassan
 */ 
#define F_CPU 8000000
#include <avr/io.h>
#include "types.h"
#include "calculator.h"

int main(void)
{
    calculator_vid_run_calculator();
}

