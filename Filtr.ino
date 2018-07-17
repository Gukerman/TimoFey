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

    Serial.printf("port LOW, Summ =  %u\n", Summ1);
    saveConfigSetup();
   
    publishMQTT(pubTopic+mqttUser+"/filtrVreal", filtrVreal);
    publishMQTT(pubTopic+mqttUser+"/filtrP", String(filtrP));

    Serial.println("GetDate - "+GetDate());
  }
 
  if ((digitalRead(pinIn)== HIGH )&&(flag==true)&&(ad < millis()- millis_ad1))   flag=false;

}

void Led(void) {


    Serial.printf("filtrP  %u\n", filtrP);
    
   lcd.home();                // At column=0, row=0
   lcd.print("OCTATOK: "+String(filtrP)+"%            ");   
   lcd.setCursor(0, 1);
   if (filtrP<1) 
     {
   lcd.setCursor(0, 1);

      lcd.print("3AMEH");lcd.write(0);      lcd.print("TE ");
      lcd.write(1); lcd.write(0);lcd.write(2);lcd.write(3); lcd.print("TP");
     }
    else 
    {
     for (int i=0; i <= filtrP/6.25; i++){
      lcd.print(char(255));
     }
      lcd.print("                          ");
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


