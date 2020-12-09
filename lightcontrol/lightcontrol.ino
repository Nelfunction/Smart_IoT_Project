#define SENSOR_1_PIN A0
#define SENSOR_1_OUTPUT 3


bool autolight= true;

String sCommand = "";


void setup() {

  Serial.begin(9600);
  pinMode(SENSOR_1_OUTPUT, OUTPUT);

}



void loop() {

  sCommand = "";
   while(Serial.available())
   {
     sCommand=Serial.readString();
   }
   
   if(sCommand != "" )
   {
    if(sCommand=="on"){
      autolight=true;
      }
      else if(sCommand=="off"){
        autolight=false;
        
        }
   
   }
  int SENSOR_1_VALUE = analogRead(SENSOR_1_PIN);

  

  
  if(autolight){
  if(SENSOR_1_VALUE>1000) //어둡다=라이트켜야됨

  {

    Serial.println("Light on " + String(SENSOR_1_VALUE));

    digitalWrite(SENSOR_1_OUTPUT, HIGH);

    }

  else  //밝음=라이트꺼야됨

  {

    Serial.println("Light off " + String(SENSOR_1_VALUE));

    digitalWrite(SENSOR_1_OUTPUT, LOW);

    }
    


  delay(1000);
  }
}
