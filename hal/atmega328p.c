
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>

#define UART_BAUD_RATE 57600
#define UART_UBRR_CALC(BAUD_,FREQ_) ((FREQ_)/((BAUD_)*16L)-1)

void uart_init(void)
{
    UCSR0B |= (1<<TXEN0) | (1<<RXEN0);
    UCSR0C |= (3<<UCSZ00);
    
    UBRR0H = (uint8_t)( UART_UBRR_CALC( UART_BAUD_RATE, F_CPU ) >> 8 );
    UBRR0L = (uint8_t)UART_UBRR_CALC( UART_BAUD_RATE, F_CPU );
}

void uart_putchar(char data) {
    
    if( data == '\n' )
        uart_putchar('\r');
    
    // Wait for empty transmit buffer
    while ( !(UCSR0A & (_BV(UDRE0))) );
    // Start transmission
    UDR0 = data; 
}

char uart_getchar(void) {
    // Wait for incoming data
    while ( !(UCSR0A & (_BV(RXC0))) );
    // Return the data
    return UDR0;
}

unsigned char uart_kbhit(void) {
    //return nonzero if char waiting polled version
    unsigned char b;
    b=0;
    if(UCSR0A & (1<<RXC0)) b=1;
    return b;
}
void uart_pstr(char *s) {
    // loop through entire string
    while (*s) { 
        uart_putchar(*s);
        s++;
    }
}

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE );
static FILE mystdin = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

void hal_hardwareinit() {
    uart_init();
    
    stdout = &mystdout;
    stdin = &mystdin;
}


void hal_delay(uint16_t ms) {
    _delay_ms(ms); 
}
