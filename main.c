#include <msp430.h>
#include <stdio.h>

// Global değişken
volatile unsigned int adc_value = 0;

// Fonksiyon prototipleri
void clock_init(void);
void uart_init(void);
void adc_init(void);
void uart_send_char(char c);
void uart_send_string(const char *str);
void delay_ms(unsigned int ms);

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Watchdog Timer durduruldu
    clock_init();
    uart_init();
    adc_init();

    uart_send_string("UART başlatıldı\r\n");

    while (1)
    {
        ADC12CTL0 |= ADC12SC;           // Dönüşüm başlat
        while (ADC12CTL1 & ADC12BUSY);  // Bitene kadar bekle
        adc_value = ADC12MEM0;          // Sonucu oku

        char buffer[32];
        sprintf(buffer, "ADC: %u\r\n", adc_value);
        uart_send_string(buffer);

        delay_ms(100);  // 100 ms bekle (10 Hz okuma)
    }
}

// 1 MHz Saat Ayarı
void clock_init(void)
{
    UCSCTL3 = SELREF_2;      // REFO referansı
    UCSCTL4 |= SELA_2;       // ACLK = REFO
    __bis_SR_register(SCG0); // DCO kapat
    UCSCTL0 = 0x0000;        // DCO sıfırla
    UCSCTL1 = DCORSEL_2;     // 1 MHz seç
    UCSCTL2 = FLLD_1 + 30;   // DCO = 1 MHz
    __bic_SR_register(SCG0); // DCO aç
    __delay_cycles(375000);  // FLL stabilite beklemesi
}

// UART (USB: UCA1TXD / UCA1RXD) 9600 baud
void uart_init(void)
{
    P4SEL |= BIT4 + BIT5;       // P4.4 = UCA1TXD, P4.5 = UCA1RXD
    UCA1CTL1 |= UCSWRST;        // Yazma moduna geç
    UCA1CTL1 |= UCSSEL_2;       // SMCLK
    UCA1BR0 = 104;              // 1 MHz, 9600 baud
    UCA1BR1 = 0;
    UCA1MCTL |= UCBRS_1;        // Modülasyon
    UCA1CTL1 &= ~UCSWRST;       // UART başlat
}


void uart_send_char(char c)
{
    while (!(UCA1IFG & UCTXIFG));
    UCA1TXBUF = c;
}


void uart_send_string(const char *str)
{
    while (*str)
    {
        uart_send_char(*str++);
    }
}

// ADC P6.0 -> A0
void adc_init(void)
{
    P6SEL |= BIT0;             // P6.0 ADC işlevi
    ADC12CTL0 = ADC12SHT02 + ADC12ON;
    ADC12CTL1 = ADC12SHP;
    ADC12CTL2 = ADC12RES_2;    // 12-bit çözünürlük
    ADC12MCTL0 = ADC12INCH_0;  // A0 kanalı
    ADC12CTL0 |= ADC12ENC;     // ADC etkinleştir
}

// Yazılım gecikmesi (yaklaşık ms)
void delay_ms(unsigned int ms)
{
    while (ms--)
        __delay_cycles(1000); // 1 MHz = 1000 döngü/ms
}
