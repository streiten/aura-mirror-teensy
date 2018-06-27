#include <OctoWS2811.h>

const int ledsPerStrip = 190;
const int TeensyLEDPin = 13;
 
DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];
const int config = WS2811_GRB | WS2811_800kHz;
OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

struct color
{
   char r;
   char g;
   char b;
};

color pixel_strip_data[ledsPerStrip];
color pixel_matrix_data[10][10];

bool display_on = true;
char inbuffer[300];
char inbuffer_sys[1];

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.print(5);   // send enquiry
    delay(500);
  }
  // Serial.println("Feels so good to be heard!");
}

void setup() {
  
  // Setting up the Serial
  Serial.begin(115200);
 
  pinMode(TeensyLEDPin, OUTPUT);
  
  displayClear();
  
  establishContact();
  generateMatrixData();
  //debugMatrixData();
  generateStripData();
  //debugStripData();

  leds.begin();
  renderStrip();

  
}


void loop() {
    
    int startChar;
    startChar = Serial.read();

    if(startChar == '$') {
      int count;
      count = Serial.readBytes((char *) inbuffer, sizeof(inbuffer) );
      if(count == sizeof(inbuffer)) {
        Serial.println("Thx for that frame!");
        generateMatrixDataFromSerial();
        //debugMatrixData();
        generateStripData();
        //debugStripData();

        if(display_on) {
          renderStrip();
        } 

      } else {
        Serial.println("Err... something wrong with the DATA! I got:");
        Serial.println(inbuffer);
      }
      
    } else if( startChar == '@' ) {
       int count;
        count = Serial.readBytes((char *) inbuffer_sys,sizeof(inbuffer_sys));
        if(count == sizeof(inbuffer_sys)) {
          
          sysCmdSR(inbuffer_sys[0]);
          
        } else {
          Serial.println("Err... something wrong with the sys COMMAND!");
        }
    }

    else if (startChar >= 0) {
    // discard unknown characters
    }
 
}

void renderStrip() {
    for (int i=0; i < ledsPerStrip; i++) {
        color c = pixel_strip_data[i];
        leds.setPixel(i,c.r,c.g,c.b);
    }
    leds.show();
}

void generateStripData() {
    
    int k = 0;
    // Line by line to linear strip
    for(int i = 0; i < 10; i++){
        
        int m = 0;
        for(int j=0; j < 19;j++){
            
            // inverse order on line for every second line in matrix
            int mNew = m;
            if((i+1)%2 == 0) {
                mNew = 9 - m;
            }
            
            if( j%2 == 0 ) {
                pixel_strip_data[k] = pixel_matrix_data[i][mNew];
                m++;
            
            //every second pixel is blanks / skipped
            } else {
                pixel_strip_data[k].r = 0;
                pixel_strip_data[k].g = 0;
                pixel_strip_data[k].b = 0;
            }

            k++;
        }
    }
}

void generateMatrixData()  {
    int k = 0;
    for(int i = 0; i < 10; i++){
        for(int j=0; j < 10;j++){

            pixel_matrix_data[i][j].r = 32;
            pixel_matrix_data[i][j].g = 0;
            pixel_matrix_data[i][j].b = 0;

            if(k % 2 == 0) {

            pixel_matrix_data[i][j].r = 0;
            pixel_matrix_data[i][j].g = 32;
            pixel_matrix_data[i][j].b = 0;
              
              
            } 
            
            if(k % 3 == 0) {
            pixel_matrix_data[i][j].r = 0;
            pixel_matrix_data[i][j].g = 0;
            pixel_matrix_data[i][j].b = 32;
            }
            
            
            k++;   
        }
    }
}

void generateMatrixDataFromSerial() {
        
    int k = 0;
    for(int i = 0; i < 10; i++){
        for(int j=0; j < 10;j++){
            // k = 0; 
            pixel_matrix_data[i][j].r = inbuffer[3*k];
            pixel_matrix_data[i][j].g = inbuffer[3*k + 1];
            pixel_matrix_data[i][j].b = inbuffer[3*k + 2];            
            k++;
        }
    }
}

void displayClear() {
  for (int i=0; i < ledsPerStrip; i++) {
    leds.setPixel(i, 0);
  }
  leds.show();
}

void sysCmdSR(byte cmd) {
  if(cmd == 'X') {
      if(!display_on) {
          Serial.println("Display ON now");
          display_on = true;
          renderStrip();
        } else 
        {
          display_on = false;
          Serial.println("Display OFF now");
          displayClear();
          }
   }
      else {
      Serial.println("Unknown command!");
   }
}

void debugStripData(){
    for (int i=0; i < ledsPerStrip; i++) {
        color c = pixel_strip_data[i];
            Serial.println(String(i));
            Serial.print(c.r);
            Serial.print(c.g);
            Serial.print(c.b);
            Serial.println("");   
    }
}

void debugMatrixData() {
      for(int i = 0; i < 10; i++){
        for(int j=0; j < 10;j++){
            Serial.println(String(i) + ":" + String(j));
            Serial.print(pixel_matrix_data[i][j].r);
            Serial.print(pixel_matrix_data[i][j].g);
            Serial.print(pixel_matrix_data[i][j].b);
            Serial.println("");   
        }
    }

}

