void initPins(void)
{
  DDRD &= B11111011;

  DDRD &= B11110111;
  PORTD |= B00001000;

  DDRD |= B00010000;
  PORTD &= B11101111;

  DDRD &= B11011111;

  DDRD &= B10111111;

  DDRD |= B10000000;
  PORTD &= B01111111;

  DDRB |= B00000001;
  PORTB |= B00000001;

  DDRB |= B00000010;

  DDRB |= B00000100;

  DDRB |= B10001000;
  PORTB |= B00001000;

  DDRB &= B11101111;

  DDRB |= B00100000;

  DDRC &= B11111110;

  DDRC |= B00000010;
  PORTC &= B11111101;
}

void initTimers(void)
{
  cli();

  TCCR1B = _BV(WGM13);
  TCCR1A = 0;

  TCCR2A = 0;
  TCCR2B = 0;

  ICR1 =   (F_CPU / 2000000) * 10000;
  TCCR1B = _BV(WGM13) | _BV(CS10);

  OCR2A = 152;
  OCR2B = 152;

  TCCR2B |= (1 << WGM22);
  TCCR2B |= (1 << CS20);
  TCCR2B |= (1 << CS21);
  TCCR2B |= (1 << CS22);

  TIMSK1 = _BV(TOIE1);

  TIMSK2 |= (1 << OCIE2A);
  TIMSK2 |= (1 << OCIE2B);
  sei();
}
