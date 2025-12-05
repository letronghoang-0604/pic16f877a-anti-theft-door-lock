#include <16F877A.h>
#include <string.h>
#FUSES NOWDT, HS, NOLVP, NOBROWNOUT
#use delay(crystal=20000000)
#use FIXED_IO(B_outputs=PIN_B2, PIN_B3, PIN_B4)


#define MOCUA PIN_E2
#define BUZZER PIN_A2
#define KHOACUA PIN_A1
#define SENDER PIN_A0

#define LCD_ENABLE_PIN PIN_B2
#define LCD_RS_PIN PIN_B4
#define LCD_RW_PIN PIN_B3
#define LCD_DATA4 PIN_D7
#define LCD_DATA5 PIN_D6
#define LCD_DATA6 PIN_D5
#define LCD_DATA7 PIN_D4
#include <lcd.c>

const unsigned char codekeypad[12] = {
   '1','2','3',
   '4','5','6',
   '7','8','9',
   '*','0','#'
};
unsigned int8  = 12;
unsigned char input_buffer[5] = "";
unsigned int8 input_index = 0;
unsigned int8 wrong_attempts = 0;
int1 lockout = 0;



void () {
    key = 12;  

    output_high(pin_D2);
    output_high(pin_D1);
    output_high(pin_D0);
    output_high(pin_C3);


    output_low(pin_D2); 
    delay_us(20);  
    if (!input(pin_C2)) key = 0;  // '1'
    if (!input(pin_C1)) key = 1;  // '2'
    if (!input(pin_C0)) key = 2;  // '3'
    output_high(pin_D2);  


    output_low(pin_D1);
    delay_us(20);
    if (!input(pin_C2)) key = 3;  // '4'
    if (!input(pin_C1)) key = 4;  // '5'
    if (!input(pin_C0)) key = 5;  // '6'
    output_high(pin_D1);

    output_low(pin_D0);
    delay_us(20);
    if (!input(pin_C2)) key = 6;  // '7'
    if (!input(pin_C1)) key = 7;  // '8'
    if (!input(pin_C0)) key = 8;  // '9'
    output_high(pin_D0);

    output_low(pin_C3);
    delay_us(20);
    if (!input(pin_C2)) key = 9;  // '*'
    if (!input(pin_C1)) key = 10; // '0'
    if (!input(pin_C0)) key = 11; // '#'
    output_high(pin_C3);
}




void lockout_countdown() {
unsigned int8 i;
    for (i = 60; i > 0; i--) {
        lcd_putc('\f');
        lcd_gotoxy(1, 1);
        lcd_putc("THU LAI SAU");
        lcd_gotoxy(1, 2);
        printf(lcd_putc, "%02d GIAY", i);
        delay_ms(1000);
        output_high(BUZZER);
        delay_ms(300);
        output_low(BUZZER);
    }
    lockout = 0;
    wrong_attempts = 0;
    lcd_putc('\f');
    lcd_gotoxy(1, 1);
    lcd_putc("NHAP MAT KHAU");
}

void main() {
    set_tris_a(0b11111000);  // RA0, RA1, RA2 (OUTPUT - SENDER, KHOACUA, BUZZER), RA3, RA4, RA5 (INPUT)
    set_tris_c(0b11110000);  // RC0, RC1, RC2 (OUTPUT - Keypad column), RC3-RC7 (INPUT - Keypad rows)
   set_tris_d(0b00000111);  // RD0, RD1, RD2  INPUT keypad, LCD (output)
    set_tris_e(0b00000101);  // RE1 (OUTPUT - MOCUA), RE0, RE2 (INPUT - Button)
    lcd_init(); 
    lcd_putc('\f');
    lcd_gotoxy(1, 1);
    lcd_putc("CUA DA KHOA");
    output_high(KHOACUA);
    output_low(BUZZER);
    output_low(SENDER);
    while(TRUE) {
        if (lockout) {
            lockout_countdown();
        }
        
        keypad();

        if(input(MOCUA) == 0) {
            output_low(KHOACUA);
            output_low(BUZZER);
            lcd_putc('\f');
            lcd_gotoxy(1, 1);
            lcd_putc("CUA DA MO");
            delay_ms(5000);
            output_high(KHOACUA);
            output_low(BUZZER);
            lcd_putc('\f');
            lcd_gotoxy(1, 1);
            lcd_putc("CUA DA KHOA");
        }

        if(key != 12) {
            lcd_gotoxy(input_index + 1, 2);
            lcd_putc(codekeypad[key]); 
            input_buffer[input_index] = codekeypad[key];
            input_index++;

            if(input_index == 4) {
                input_buffer[4] = '\0';
                int1 correct = 1;
                char correct_pass[4] = {'1', '2', '3', '#'};
                for (int i = 0; i < 4; i++) {
                    if (input_buffer[i] != correct_pass[i]) {
                        correct = 0;
                        break;
                    }
                }
                if (correct) {
                    lcd_putc('\f');
                    lcd_gotoxy(1, 1);
                    lcd_putc("CUA DA MO");
                    output_low(BUZZER);
                    output_low(KHOACUA);
                    delay_ms(5000);
                    output_high(KHOACUA);
                    wrong_attempts = 0;
                } else {
                    lcd_putc('\f');
                    lcd_gotoxy(1, 1);
                    lcd_putc("MAT KHAU SAI");
                    delay_ms(2000);
                    wrong_attempts++;
                    if (wrong_attempts >= 3) {
                        lockout = 1;
                        output_high(BUZZER);
                        delay_ms(5000);
                        output_low(BUZZER);
                        
                        output_high(SENDER);
                        delay_ms(500);
                        output_low(SENDER);
                        
                    }
                }
                lcd_putc('\f');
                lcd_gotoxy(1, 1);
                lcd_putc("CUA DA KHOA");
                input_index = 0;
                memset(input_buffer, 0, sizeof(input_buffer));
            }
            key = 12;
            delay_ms(500);
        }
    }
}
