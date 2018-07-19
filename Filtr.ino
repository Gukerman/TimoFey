void filtr(void) {
  filtrP = 100-(100*Summ1 / filtrV.toInt());
  filtrVreal = Summ1;

  if ((digitalRead(pinIn)== LOW )&&( flag ==false))  
  {
 
    flag=true;
    Summ1=Summ1+vodomerIn.toInt();  

    Led();

    if (filtrV.toInt()<Summ1) allarm(filtrV, String(Summ1));
    else 
    {
    Status = "Ok";
    }
    
    millis_ad1 = millis();

//    Serial.printf("port LOW, Summ =  %u\n", Summ1);
    saveConfigSetup();
   
    publishMQTT(pubTopic+mqttUser+"/filtrVreal", filtrVreal);
    publishMQTT(pubTopic+mqttUser+"/filtrP", String(filtrP));
 
if (Month_i <= 2) { --Year_i;  Month_i = Month_i + 12; }

long tt = (365*Year_i + Year_i/4 - Year_i/100 + Year_i/400 + 3*(Month_i+1)/5 + 30*Month_i + Day_i - 719561) * 86400;  //+ 3600 * h + 60 * mi + s

//  Serial.println(now_i);

if (now_i>1525132800)
{
ost_i = filtrT.toInt() - ((now_i - tt)/24/60/60);

if ( ost_i <= 0) allarm(filtrV, String(Summ1));

/*  
  Serial.println(ost_i);
  Serial.print(" ");
  Serial.print(Day_i);
  Serial.print(".");
  Serial.print(Month_i);
  Serial.print(".");
  Serial.print(Year_i);
  Serial.println();
*/
  
  }
} 
  if ((digitalRead(pinIn)== HIGH )&&(flag==true)&&(ad < millis()- millis_ad1))   flag=false;

}

void Led(void) {

   lcd.clear();
   lcd.home();                // At column=0, row=0
   lcd.print("OCTATOK: "+String(filtrP)+"%    ");   
   lcd.setCursor(0, 1);
   if (filtrP<1) 
     {
   lcd.setCursor(0, 1);

      lcd.print("3AMEH");lcd.write(0);      lcd.print("TE ");
      lcd.write(1); lcd.write(0);lcd.write(2);lcd.write(3); lcd.print("TP");
     }
    else 
    {
     for (int i=0; i <= 16; i++){
      if (i <= filtrP/6.25) lcd.print(char(255));
      else
        {
        lcd.print(" ");
        }
      }
    }
    
}

void allarm(String filtrV, String Summ1)
{
  Serial.println("Allarm");
  Serial.print("filtrV = ");
  Serial.println(filtrV);
  Serial.print("Summ1 = ");
  Serial.println(Summ1);
  Status = "Allarm";
//  publishMQTT(pubTopic+mqttUser+"/Status", Status);

}


