#include <at89c5131.h>
#include "lcd.h"
#include "serial.c"
		 
unsigned char bdata b0=7,b1,b2;
unsigned char num_map[16];
sbit b00 = b0^0;
sbit b03 = b0^3;
sbit b10 = b1^0;
sbit b13 = b1^3;
sbit b20 = b2^0;
sbit b23 = b2^3;
code unsigned char controls[]= "qwertyuiasdfghjk";
unsigned char move;
unsigned char mov_i;
unsigned char count;
unsigned char	j;
unsigned char uncvrd_ind[17];



void set_num (void){
	unsigned char pos;
	unsigned char num;
	pos=b0;
	b2=b0;
	for (num=1;num<9;num++)
	{
		num_map[pos]=num;
		
		b1=b2>>1;
	  b13=b23^b20;
		pos=(b1+b0)%16;
		num_map[pos]=num;
		if(num==8){break;}
		b2=b1>>1;
	  b23=b13^b10;
		pos=(b2+b0)%16;
	}
	
	b0=b2
}

void uncover(bit b, unsigned char c){
	unsigned char n=num_map[c];
	unsigned char add;
	if(c<8){add= (130+c);}
	else{add= (186+c);}

	if(b==0){
		lcd_cmd(add);
		lcd_write_char(n+48);
		msdelay(3000);
		lcd_cmd(add);
		lcd_write_char(42);
	}
	else{
		lcd_cmd(add);
		lcd_write_char(n+48);
	}
}


void print_game(unsigned char a){
	lcd_cmd(0x82);
	lcd_write_string("********  0 ");
	lcd_cmd(0xC2);
	if(a==0){
		lcd_write_string("********  L:XX");
	}
	else{
		lcd_write_string("********  L:");
		lcd_write_char(48+(a/10));
		lcd_write_char(48+(a%10));
	}	
}


void incr_count(){
	lcd_cmd(0x8C);
	lcd_write_char(48+(count/10));
	lcd_write_char(48+(count%10));
}


unsigned char ret_index(unsigned char mov){
	unsigned char i=0;
	while(i<16){
		if (controls[i]==mov)
		{
			break;
		}
		else{
			i++;
		}
	}

	return i;
}

unsigned char valid_move(void){
	unsigned char ind;
	while(1){
		move=receive_char();
		ind= ret_index(move);
		if(ind!=16){break;}
	}
	return ind;
}

bit repeat_check(unsigned char i ){
	unsigned char c;
	bit f=1;
	for (c=0; c<=j; c++)
	{
		if (uncvrd_ind[c]==i)
		{
			f=0; 
			break;
		}
	}
	return f;
}


void inp_move(void){
	bit k;
	while(1){
		mov_i=valid_move();
		k=repeat_check(mov_i);
		if (k==1){break;}
	}
}



void main(void){
	
	unsigned char L=100;
	unsigned char restart;
	unsigned char	num_active;
	
	lcd_init();
	uart_init();
	print_game(0);

  while(1){
  	transmit_string("****Matching Pair Game****\r\n");
		transmit_string("****Keyboard Controls****\r\n");
		transmit_string("****|qwertyui|****\r\n");
		transmit_string("****|asdfghjk|****\r\n");
		

		uncvrd_ind[0]=16; //or any no greater thn 15
	  j=0;
		count=0;
		set_num();
		
		while(j<16){
			inp_move();
			count++;
			incr_count();
			uncover(1,mov_i);
			num_active= num_map[mov_i];
			j++;
			uncvrd_ind[j]=mov_i;

			while(1){
				inp_move();
				count++;
				incr_count();
				if(num_map[mov_i]==num_active){
					uncover(1,mov_i);
					j++;
					uncvrd_ind[j]=mov_i;
					break;
				}
				else{
					uncover(0,mov_i);
				}
			}	
			
		}
		
		if (count<L){
			L=count;
		}
		else;
		
		transmit_string("*****Game Over*****\r\n");
		transmit_string("Press spacebar to restart\r\n");
	  while(1){
			restart=receive_char();
			if (restart== 32){break;}
		}
		
		print_game(L);
	
	}
}


