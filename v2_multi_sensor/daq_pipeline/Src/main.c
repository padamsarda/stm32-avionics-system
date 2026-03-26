#include "stm32f401xe.h"

// ---------------- CLOCK ----------------
void clock_init(){
    FLASH->ACR = FLASH_ACR_LATENCY_2WS
               | FLASH_ACR_ICEN
               | FLASH_ACR_DCEN
               | FLASH_ACR_PRFTEN;

    RCC->CR &= ~RCC_CR_PLLON;
    while(RCC->CR & RCC_CR_PLLRDY);

    RCC->PLLCFGR = 0;
    RCC->PLLCFGR |= (8U) | (168U <<6) | (1U<< 16);

    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY));

    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

// ---------------- TIMER ----------------
void TRGO_init(){
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->PSC = 3;
    TIM2->ARR = 2099;

    TIM2->CR2 |= (2 << 4);
    TIM2->CR1 |= TIM_CR1_CEN;
}

// ---------------- ADC ----------------
void ADC_init()
{
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    ADC1->CR2 &= ~(0xF << 24);
    ADC1->CR2 |= (6U << 24);

    ADC1->CR2 &= ~(3U << 28);
    ADC1->CR2 |= (1U << 28);

    ADC1->CR2 |= ADC_CR2_DMA;
    ADC1->CR2 |= ADC_CR2_ADON;

    ADC1->SQR1 &= ~(0xF << 20);
    ADC1->SQR3 = 0;
}

// ---------------- DMA ----------------
#define BUF_SIZE 256
volatile uint16_t adc_buf[BUF_SIZE];

void DMA_init(){
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    DMA2->LIFCR = 0xFFFFFFFF;

    ADC1->CR2 |= ADC_CR2_DDS;

    DMA2_Stream0->CR &= ~DMA_SxCR_EN;
    while (DMA2_Stream0->CR & DMA_SxCR_EN);

    DMA2_Stream0->PAR = (uint32_t)&ADC1->DR;
    DMA2_Stream0->M0AR = (uint32_t)adc_buf;
    DMA2_Stream0->NDTR = BUF_SIZE;

    DMA2_Stream0->CR |= DMA_SxCR_PSIZE_0;
    DMA2_Stream0->CR |= DMA_SxCR_MSIZE_0;
    DMA2_Stream0->CR |= DMA_SxCR_MINC;
    DMA2_Stream0->CR |= DMA_SxCR_CIRC;
    DMA2_Stream0->CR |= DMA_SxCR_HTIE;
    DMA2_Stream0->CR |= DMA_SxCR_TCIE;

    DMA2_Stream0->CR |= DMA_SxCR_EN;

    NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}

// ---------------- UART ----------------
volatile uint8_t tx_buf[512];
volatile uint16_t tx_head =0;
volatile uint16_t tx_tail =0;

void UART_push(uint8_t data)
{
    uint16_t next = (tx_head + 1) & 0x1FF;

    if (next != tx_tail)
    {
        tx_buf[tx_head] = data;
        tx_head = next;
    }
}

void USART2_IRQHandler(void)
{
    if (USART2->SR & USART_SR_TXE)
    {
        if (tx_tail != tx_head)
        {
            USART2->DR = tx_buf[tx_tail];
            tx_tail = (tx_tail + 1) & 0x1FF;
        }
        else
        {
            USART2->CR1 &= ~USART_CR1_TXEIE;
        }
    }
}


void UART_init(){
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    GPIOA->MODER &= ~(3U << 4);
    GPIOA->MODER |=  (2U << 4);

    GPIOA->AFR[0] &= ~(0xF << 8);
    GPIOA->AFR[0] |=  (7U << 8);

    USART2->BRR = 365;
    USART2->CR1 |= USART_CR1_TE;
    USART2->CR1 |= USART_CR1_UE;

    NVIC_EnableIRQ(USART2_IRQn);
}

// ---------------- DMA ISR ----------------
void DMA2_Stream0_IRQHandler(void)
{
    if (DMA2->LISR & DMA_LISR_HTIF0)
    {
        DMA2->LIFCR |= DMA_LIFCR_CHTIF0;

        for (int i = 0; i < BUF_SIZE/2; i++)
        {
            uint16_t val = adc_buf[i];
            UART_push('0' + (val % 10));
            UART_push('\r');
            UART_push('\n');


          //  for(int i=0; i<10000; i++);
        }

        USART2->CR1 |= USART_CR1_TXEIE;
    }

    if (DMA2->LISR & DMA_LISR_TCIF0)
    {
        DMA2->LIFCR |= DMA_LIFCR_CTCIF0;
    }
}

volatile uint8_t button_pressed = 0;
void EXTI15_10_IRQHandler(void){

	if (EXTI->PR & (1 << 13))
	{
	    EXTI->PR |= (1 << 13); // clear pending

	    button_pressed = 1;
	}
}

void P13_init(void){
	//initializing p13 and exti13
	// Enable GPIOC clock
	    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

	    // Enable SYSCFG clock (needed for EXTI)
	    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	    GPIOC->MODER &= ~(3 << (13 * 2)); // pin 13 at input mode

	    SYSCFG->EXTICR[3] &= ~(0xF << 4); //clears exti13
	    SYSCFG->EXTICR[3] |= (0x2 << 4); //sets exti13
	    EXTI->IMR |= (1 << 13); //allows interrupt
	    NVIC_EnableIRQ(EXTI15_10_IRQn); //enable in NVIC
	    EXTI->FTSR |= (1 << 13);   // falling edge
	    EXTI->RTSR &= ~(1 << 13);  // disable rising edge

}

void A0_init(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	    GPIOA->MODER |= (3U << 0);
}

void led_init(void){
	//internal led (P5) initialization
	  GPIOA->MODER &= ~(3 << (5 * 2));   // clear
	    GPIOA->MODER |=  (1 << (5 * 2));   // output mode
}
// ---------------- MAIN ----------------
int main()
{
    clock_init();
    TRGO_init();
    ADC_init();
    DMA_init();
    UART_init();
    A0_init();
    led_init();
    P13_init();

    while (1){
    	if(button_pressed){
    		 button_pressed = 0;
    		 GPIOA->ODR ^= (1 << 5);  // toggle LED
    	}
    }
}
