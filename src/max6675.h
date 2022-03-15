float dt[] = {2.00, 2.00};

float readTemp_Max6675(byte p) 
{
  int a = 0;
  if (p == 1)
  {
    PORTB &= B11111110;
  }
  else if (p == 2)
  {
    PORTB &= B11110111;
  }

  __asm__ __volatile__ ("nop\n\t");

  for (int8_t i = 15; i >= 0; i--)
  {

    PORTB |= B00100000;

    __asm__ __volatile__ ("nop\n\t");

    if (((PINB & B00010000) >> 4))
    {
      a |= (1 << i);
    }

    PORTB &= B11011111;

    __asm__ __volatile__ ("nop\n\t");

  }

  if (p == 1)
  {
    PORTB |= B00000001;
  }
  else if (p == 2)
  {
    PORTB |= B00001000;
  }

  if (a & 0x4)
  {
    return 255;
  }
  else
  {
    return ((a >> 3) * 0.25);
  }
}

float getTempHuman(int8_t sel) 
{
  float t;
  t = readTemp_Max6675(sel);
  float tem = 0.0;
  for (int i = 0; i < 39; i++) {
    tem += t;
  }
  return tem / 40 - dt[sel - 1];
}
