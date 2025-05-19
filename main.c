#include <avr/io.h>
#define F_CPU 10000000UL
#include <util/delay.h>
#define LCDP PORTB
#define RS 0
#define RW 1
#define EN 2
#define AK 3
#define CGRAM 6
#define DDRAM 7

void send_command(unsigned char cmd){
	PORTB = 0; //Clear All Pin on PORTB
	PORTD = 0x0;//Clear RS/RW/E/AK on PORTD;
	PORTD |= 1<<EN;//Set E bit to send instruction
	PORTB |= cmd; //Set Pin
	PORTD = 0x8;//Clear RS/RW/E/AK;
	_delay_us(40);
}
void initial(){
	send_command(0x20); //clear DDRAM (Display Data RAM)
	send_command(0x01); //clear AC (Address Counter)
	send_command(0x02); // cursor returns home
	_delay_ms(2);
	send_command(0b00111000);//Set function:1; 8-bit mode:1; 2-line display:1; 5x8 font:0); //
	send_command(0b00001111);//Display on; cursor on; blink
	send_command(0b00000110);//Increment and shift cursor; don't shift display
	_delay_us(40);
}
void send_data(unsigned char data){
	PORTB = 0; //Clear All Pin on PORTB
	PORTD = 0x0;//Clear RS/RW/E/AK on PORTD;
	PORTD |= (1<<RS) | (1<<EN) ; //Set E bit to send instruction; Send data
	_delay_us(40);
	PORTB |= data; //Set Pin
	PORTD = 0x8;//Clear RS/RW/E/AK;
	_delay_us(40);
}
void create_char(uint8_t char_number, unsigned char *pattern){
	for(uint8_t i=0; i<8;i++){
		send_command((1<<CGRAM)| (char_number<<3)| i); //CGRAM
		_delay_us(40);
		send_data(pattern[i]);
		_delay_us(40);
	}
	send_command(0x02); // cursor returns home
	_delay_ms(2);
	
}
void send_string(uint8_t start_position, unsigned char* str){
	// Move cursor to the desired start position.
	send_command((1<<7)|start_position); //0: line0; 4: line1 ; colum: 0-A; Example: 4A line1-col11
	// Loop to display each character until null terminator '\0'
	while(*str != '\0'){
		send_data(*str);
		str++;
	}
}
void int_to_string(uint16_t n, unsigned char *str){
	unsigned char temp[10]={0};
	uint8_t i=0;
	if(n<10) {
		temp[i++] = n+'0';
		temp[i++] = 0+'0';
	}
	else {
		while(n>0){
			temp[i++] = n%10 + '0';
			n/=10;
		}
	}
	for(uint8_t j=0;j<i;j++){
		str[j]=temp[i-j-1];
	}
	str[i]='\0';
}
void create_symbol(){	
	uint8_t optimus1[8] = {
		0b10111,
		0b10111,
		0b10111,
		0b10111,
		0b10111,
		0b10000,
		0b10111
	}; create_char(0,optimus1); 
	
	uint8_t optimus2[8] = {
		0b11011,
		0b10101,
		0b10101,
		0b10101,
		0b11011,
		0b01110,
		0b10101
	};create_char(1,optimus2);
	
	uint8_t optimus3[8] = {
		0b11101,
		0b11101,
		0b11101,
		0b11101,
		0b11101,
		0b00001,
		0b11101
	}; create_char(2,optimus3);
	
	uint8_t optimus4[8] = {
		0b11011,
		0b11100,
		0b11101,
		0b11101,
		0b01101,
		0b01110,
		0b01111
	}; create_char(3,optimus4);
	
	uint8_t optimus5[8] = {
		0b10101,
		0b01110,
		0b00000,
		0b10001,
		0b01110,
		0b01110,
		0b00000
	}; create_char(4,optimus5);
	
	uint8_t optimus6[8] = {
		0b11011,
		0b00111,
		0b10111,
		0b10111,
		0b01110,
		0b01110,
		0b11110
	}; create_char(5,optimus6);
}
void setup(){
	create_symbol();//Draw Optimus prime
	send_data(0); 
	send_data(1); 
	send_data(2); 
	send_command((1<<7)|0x40);
	send_data(3); 
	send_data(4); 
	send_data(5); 
	
	unsigned char dot[7] = ":  :  .";
	send_string(0x46,dot);
	unsigned char title[12] = "h / m /s/ ms";
	send_string(0x04,title);
}
int main(void)
{
	DDRB = 0xff; //Set all pin on port B to output
	DDRD |= (1<<PD0) | (1<<PD1) | (1<<PD2) | (1<<PD3) ; // Set top 4 pins on port D to output

	initial(); //init LCD
	setup();
	
	uint8_t h=0,m=0,s=0;
	unsigned char buffer[10]={0};
	uint16_t i = 0;
	while (1) {
		int_to_string(i++,buffer); //ms
		send_string(0x4D,buffer);
		if(i==1000){
			i=0;
			s++;
		}
		int_to_string(s,buffer); //second
		send_string(0x4A,buffer);
		if(s==60){
			s=0;
			m++;
		}
		int_to_string(m,buffer); //minute
		send_string(0x47,buffer);
		if(m==60){
			m=0;
			h++;
		}
		int_to_string(h,buffer); //hour
		send_string(0x44,buffer);
		if(h==24){
			h=0;
		}
		_delay_ms(1);
	}
}
