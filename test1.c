//#define F_CPU 8000000UL
#define UART_BAUD_RATE 57600


#include <ptthreads.h>
#include <ptstream.h>
#include <hal.h>

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>

/*void uart_init(void);

int uart_putchar(char c, FILE *stream);
int uart_getchar (FILE *stream);

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE );
static FILE mystdin = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

int uart_putchar( char c, FILE *stream )
{
    if( c == '\n' )
        uart_putchar( '\r', stream );
    
    loop_until_bit_is_set( UCSR0A, UDRE0 );
    UDR0 = c;
    return 0;
}

int uart_getchar (FILE *stream)
{
    char c;
    
    // echo 
    
    
    loop_until_bit_is_set( UCSR0A, RXC0 );
    c = UDR0;
    printf("%c", c);
    
    return c;
}

#define UART_UBRR_CALC(BAUD_,FREQ_) ((FREQ_)/((BAUD_)*16L)-1)

void uart_init(void)
{
    UCSR0B |= (1<<TXEN0) | (1<<RXEN0);    // UART TX und RX einschalten
    UCSR0C |= (3<<UCSZ00);    // Asynchron 8N1 
    
    UBRR0H = (uint8_t)( UART_UBRR_CALC( UART_BAUD_RATE, F_CPU ) >> 8 );
    UBRR0L = (uint8_t)UART_UBRR_CALC( UART_BAUD_RATE, F_CPU );
}
*/

uint8_t ledswitch = 0x0 | (1 << PB5);
uint8_t counter = 0;
uint8_t count_read = 0;
int8_t ret = 0;

int main (void) {
    
    hal_hardwareinit();
    
    DDRB  = 0xFF;
    DDRB &= ~( (1 << PB4) );
    
    //uart_init();
    
    //stdout = &mystdout;
    //stdin = &mystdin;
    
    
    ptstream_t teststream;
    ptstream_init(&teststream, 100);
    
    
    while(1) {
        
        hal_delay(1000);
        
        PORTB ^= ((1 << PB5) & ledswitch);
        printf("PORTB: %d\n", PORTB);
        
        if (PINB & (1 << PB4))  {
            ledswitch = ~ledswitch;
            printf("Setting switch to %d\n", ledswitch);
            scanf("%d", ret);
        }
        
        if ((1 << PB5) & ledswitch) {
            
            ret= ptstream_write(&teststream, &counter, 1);
            
            printf ("Writing %d with retvalue %d\n", counter, ret);
            
            counter++;
        }
        else {
            ret = ptstream_read(&teststream, &count_read, 1);
            printf("Read %d with retvalue %d\n", count_read, ret);
        }
        
        printf("Read Pointer: %d\n", teststream.read_p);
        printf("Write Pointer: %d\n", teststream.write_p);
        
        /*int i = 0;
        for (i = 0; i < 10; i++) {
            printf ("%d\n", teststream.address[i]);
        }*/
        
        
        
    }
    
    return 0;
}