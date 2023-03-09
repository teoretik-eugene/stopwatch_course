/*
 * kurs_sec.c
 *
 * Created: 05.03.2023 16:21:26
 * Author : genyl
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define NUMBERS PORTB		// Порт для чисел

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
// Переводит число в разряды
void convert_num(int num){
	for(int i = 3; i >= 0; i--){
		digits[i] = num % 10;
		num /= 10;
	}	
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

void display_num(){	
	int time_ = 50;
	
	NUMBERS = nums[digits[0]];
	PORTD &= ~(1<<0);
	_delay_ms(time_);
	PORTD |= (1<<0);
	
	NUMBERS = nums[digits[1]];
	NUMBERS |= (1<<7);		// не забываем про точку
	PORTD &= ~(1<<1);
	_delay_ms(time_);
	PORTD |= (1<<1);
	
	NUMBERS = nums[digits[2]];
	PORTD &= ~(1<<2);
	_delay_ms(time_);
	PORTD |= (1<<2);
	
	NUMBERS = nums[digits[3]];
	PORTD &= ~(1<<3);
	_delay_ms(time_);
	PORTD |= (1<<3);
	/*
	NUMBERS = nums[digits[0]];
	PORTD = ~0b00000001;
	_delay_ms(time_);
	NUMBERS = nums[digits[1]];
	NUMBERS |= (1<<7);
	PORTD = ~0b00000010;
	_delay_ms(time_);
	NUMBERS = nums[digits[2]];
	PORTD = ~0b00000100;
	_delay_ms(time_);
	NUMBERS = nums[digits[3]];
	PORTD = ~0b00001000;
	_delay_ms(time_);
	*/
}

// Настройка счетчика
void start(void){
	// Настраиваем деление частоты
	
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
	
	TCNT1 = 0;		// Счетчик в нуле 
	
	TCCR1B |= (1<<WGM12);		// Сброс при совпадении
	
}

void pause(void){
	// Зажигаем светодиод, означающий, что поставили на паузу
	
}

ISR(TIMER1_COMPA_vect){
	sec++;
	if(lap == 0){		// Если не режим круга включен
		//convert_num(sec);
		format_time(sec);
	}
	/*
	поработать если включен режим круга
	*/
}

int main(void){
	
	/*Настраиваем регистры направления*/
	DDRB = 0b11111111;
	DDRD = 0b00011111;		// Кнопки на вход, индикаторы и светодиод на выход
	/*Настраиваем выходное напряжение*/
	PORTD = 0b1101111;
	
	sei();		// Разрешаем глобальные прерывания
	
    while (1){
		display_num();
		
		// Кнопка старта
		if(~PIND & (1<<6)){
			start();
			_delay_ms(200);
		}
		
		if(~PIND & (1<<5)){
			pause();
			_delay_ms(200);
		}
		
		// Кнопка паузы
		
		// Кнопка круга
    }
}

