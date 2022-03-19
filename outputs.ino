
ISR(TIMER1_OVF_vect)
{
  encoder->service();
}

ISR(TIMER2_COMPA_vect) 
{
  PORTB |= B00000010;
}

ISR(TIMER2_COMPB_vect) 
{
  PORTB |= B00000100;
}

void TopHeaterON(int dim) 
{
  OCR2B = 151 - dim;
}

void TopHeaterOFF(void) 
{
  OCR2B = 150;
}

void BottomHeaterON(int dim) 
{
  OCR2A = 151 - dim;
}

void BottomHeaterOFF(void) 
{
  OCR2A = 150;
}



void Dimming() 
{
  PORTB &= B11111101;
  PORTB &= B11111011;
  TCNT2 = 0;
  if (Status == 1) 
  {
    sprintf (buf, "OK%03d%03d%03d%03d%03d\r\n",
             P_U_Serial,
             P_D_Serial,
             tc1,
             tc2,
             currentProfile);
  }
  if (Status == 2) 
  {
    byte pdMap = map(P_D, 0, 150, 0, 100);
    sprintf (buf, "OK%03d%03d%03d%03d%03d\r\n",
             tunerAccu2,
             pdMap,
             tc1,
             tc2,
             Status);
  }
  if (Status == 4) 
  {
    byte puMap = map(P_U, 0, 150, 0, 100);
    sprintf (buf, "OK%03d%03d%03d%03d%03d\r\n",
             tunerAccu1,
             puMap,
             tc1,
             tc2,
             Status);
  }
}
