#include "firmware.h"

static void usart_setup(void)
{
  /* Enable clocks for GPIO port A (for GPIO_USART2_TX) and USART2. */
  rcc_periph_clock_enable(RCC_USART2);
  rcc_periph_clock_enable(RCC_GPIOA);

  /* Setup GPIO pin GPIO_USART2_TX/GPIO9 on GPIO port A for transmit. */
  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);
  gpio_set_af(GPIOA, GPIO_AF7, GPIO2 | GPIO3);

  /* Setup UART parameters. */
  usart_set_baudrate(USART2, 115200);
  usart_set_databits(USART2, 8);
  usart_set_stopbits(USART2, USART_STOPBITS_1);
  usart_set_mode(USART2, USART_MODE_TX_RX);
  usart_set_parity(USART2, USART_PARITY_NONE);
  usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

  /* Finally enable the USART. */
  usart_enable(USART2);
}

int _write(int file, char *ptr, int len)
{
  int i;

  if (file == 1)
  {
    for (i = 0; i < len; i++)
    {
      if (ptr[i] == '\n')
      {
        usart_send_blocking(USART2, '\r');
      }
      usart_send_blocking(USART2, ptr[i]);
    }
    return i;
  }
  errno = EIO;
  return -1;
}

static volatile uint64_t ticks = 0;
uint32_t src = 20;
uint32_t dst = 0;
void sys_tick_handler(void)
{
  ticks++;
}

static void systick_setup(void)
{
  systick_set_frequency(SYSTICK_FREQ, CPU_FREQ);
  systick_counter_enable();
  systick_interrupt_enable();
}

uint64_t get_ticks(void)
{
  return ticks;
}

void delay_ms(uint64_t milleseconds)
{
  uint64_t end_time = get_ticks() + milleseconds;
  while (get_ticks() < end_time)
    ;
}

void clock_setup()
{
  rcc_clock_setup_pll(&rcc_hsi16_configs[RCC_CLOCK_VRANGE1_80MHZ]);
  rcc_periph_clock_enable(RCC_DMA1);
}

void dma_setup()
{
  dma_disable_channel(DMA1, 1);
  dma_channel_reset(DMA1, 1);

  dma_set_priority(DMA1, 1, DMA_CCR_PL_VERY_HIGH);
  dma_enable_transfer_error_interrupt(DMA1, 1);
  dma_enable_half_transfer_interrupt(DMA1, 1);
  dma_enable_transfer_complete_interrupt(DMA1, 1);

  dma_disable_memory_increment_mode(DMA1, 1);

  dma_set_number_of_data(DMA1, 1, 1);
  dma_enable_mem2mem_mode(DMA1, 1);

  dma_set_memory_size(DMA1, 1, DMA_CCR_MSIZE_32BIT);
  dma_set_memory_address(DMA1, 1, (uint32_t)&dst);

  dma_set_peripheral_address(DMA1, 1, (uint32_t)&src);
  dma_set_peripheral_size(DMA1, 1, DMA_CCR_PSIZE_32BIT);
    
  dma_set_read_from_peripheral(DMA1, 1);
}
int main(void)
{
  clock_setup();
  systick_setup();
  usart_setup();

  while (1)
  {
    dma_setup();
    printf("DMA Started ... \n");
    printf("Source -> %d \n", src);
    printf("Distination -> %d \n", dst);
    dma_enable_channel(DMA1, 1);

    while(!dma_get_interrupt_flag(DMA1, 1, DMA_TCIF)){
      printf("Waiting for transfer to complete \n");
    }
    if(dma_get_interrupt_flag(DMA1, 1, DMA_TEIF)){
      printf("Error \n");
    }else{
      printf("Data transferred to distination \n");
    }
    
    dma_disable_channel(DMA1, 1);
    printf("Source -> %d \n", src);
    printf("Distination -> %d \n", dst);
    delay_ms(3000);
    src++;
  }
  return 0;
}