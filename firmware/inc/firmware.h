#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/dma.h>
#include <errno.h>
#include <stdio.h>


#define CPU_FREQ      (84000000)
#define SYSTICK_FREQ  (1000)