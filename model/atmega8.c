/*
 * kurs_atmega8.c
 *
 * Created: 09.03.2023 20:07:18
 * Author : genyl
 */ 
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define NUMBERS PORTD

int digits[4] = {0, 0, 0, 0};		// Массив для разрядов
int sec = 0;	// Количество секунд
int lap = 0;	// Режим круга в секундомере

unsigned int nums[10] = {
	0x3F,		// 0
	0x6,		// 1
	0x5B,		// 2
	0x4F,		// 3
	0x66,		// 4
	0x6D,		// 5
	0x7D,		// 6
	0x7,		// 7
	0x7F,		// 8
	0x6F
};

void format_time(int num);

void display_num(void);

void start(void);

void pause(void);

void set_lap(void);

ISR(TIMER1_COMPA_vect){
	sec++;
	if(lap == 0){		// Если не режим круга включен
		//convert_num(sec);
		format_time(sec);
	}else{
		// показывать запомненное время
		
	}
	/*
	поработать если включен режим круга
	*/
}

int main(void){
	
	// Настраиваем направления регистров
	DDRD = 0b11111111;			// Индикаторы на выход
	DDRB = 0b00111111;			// Разряды на выход
    
	DDRC = 0b0000000;			// Настроим все на вход
	
	PORTB = 0b00001111;
	PORTC = 0b1111111;		// Подтягиваем потенциалы на входах
	sei();					// Разрешить глобальные прерывания
	
    while (1){
		display_num();
		
		// Старт
		if(~PINC & (1<<0)){
			start();
			_delay_ms(200);
		}
		
		// Пауза
		if(~PINC & (1<<1)){
			pause();
			_delay_ms(200);
		}
		
		if(~PINC & (1<<2)){
			set_lap();
			_delay_ms(200);
		}
    }
}

void start(void){
	if(lap)
		return;
	
	PORTB &= ~(1<<4);
	
	// Настраиваем предделителя частоты
	
	// Поделим частоту на 256
	TCCR1B |= (1<<CS12);
	TCCR1B &= ~((1<<CS11) | (1<<CS10));
	
	/*
	TCCR1B &= ~(1<<CS12);
	TCCR1B |= ((1<<CS10) | (1<<CS11));		// Делим частоту на 64		// а если на 256*/
	
	TIMSK |= (1<<OCIE1A);		// Прерывания по совпадению
		
	// Сравниваем по значению 31 250
	OCR1AH = 0b01111010;
	OCR1AL = 0b00010010;
	
	//TCNT1 = 0;		// Счетчик в нуле // нам не нужно обнулять каждый раз
	
	TCCR1B |= (1<<WGM12);		// Сброс при совпадении
}

void pause(void){
	TCCR1B &= ~((1<<0) | (1<<1) | (1<<2));		// останавливаем счетчик
	
	// Можно зажечь светодиод
	PORTB |= (1<<4);
	
}

void set_lap(void){
	lap = 1;		// Врубили режим круга	
	// Зажигаем светодиод
	PORTB |= (1<<5);
}

void display_num(void){
	int time_ = 50;
	
	NUMBERS = nums[digits[0]];
	PORTB &= ~(1<<0);
	_delay_ms(time_);
	PORTB |= (1<<0);
	
	NUMBERS = nums[digits[1]];
	NUMBERS |= (1<<7);
	PORTB &= ~(1<<1);
	_delay_ms(time_);
	PORTB |= (1<<1);
	
	NUMBERS = nums[digits[2]];
	PORTB &= ~(1<<2);
	_delay_ms(time_);
	PORTB |= (1<<2);
	
	NUMBERS = nums[digits[3]];
	PORTB &= ~(1<<3);
	_delay_ms(time_);
	PORTB |= (1<<3);
	
}

// Время в нужном формате
void format_time(int num){
	
	if(num >= 3599){		// если будет 59:59, то не считает дальше
		digits[3] = 9;
		digits[2] = 5;
		digits[1] = 9;
		digits[0] = 5;
		}else{
		// Форматирование секунд в 60-минутный формат
		
		int sec = num % 60;
		int minutes = num / 60;
		
		// Записываем секунды:
		digits[3] = sec % 10;
		digits[2] = sec / 10;
		
		// Записываем минуты
		digits[1] = minutes % 10;
		digits[0] = minutes / 10;
	}
}
