

/*
    Copyright 2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

#include <Porcupine_EN.h>

#include "params.h"
#include <Servo.h>
#include <SparkFunDS1307RTC.h>
#include <Wire.h>

#define MEMORY_BUFFER_SIZE (70 * 1024)


static const char* ACCESS_KEY = "TcnJthS9uLS+Bd0DZ8uqgvfqboGmXGvD9R3TyGpLSreamRIvlg7Bhw=="; //AccessKey string obtained from Picovoice Console (https://picovoice.ai/console/)

static int8_t memory_buffer[MEMORY_BUFFER_SIZE] __attribute__((aligned(16)));

static pv_porcupine_t *handle = NULL;

static const int32_t NUM_KEYWORDS = 3;
static const int32_t KEYWORD_MODEL_SIZES[] = {
        sizeof(EAT_KEYWORD_ARRAY),
        sizeof(ROW_KEYWORD_ARRAY),
        sizeof(WATCH_KEYWORD_ARRAY)
};
static const void *KEYWORD_MODELS[] = {
        EAT_KEYWORD_ARRAY,
        ROW_KEYWORD_ARRAY,
        WATCH_KEYWORD_ARRAY
};
static const float SENSITIVITIES[] = {
        0.8f,
        0.8f,
        0.8f
};

static const char *KEYWORDS_NAME[] = {
        "something to eat",
        "play row blocks",
        "watch tv"
};

 
Servo myservo1;
Servo myservo2;

int numSnacks = 0;
int numBlox = 0;
int numWatch = 0;

int s = rtc.second();
int m = rtc.minute();
int h = rtc.hour();

int RED = 22;
int GREEN = 23;

// here's where the code gets changed to do something based on a particular word

static void wake_word_callback(int32_t keyword_index) {
   Serial.print("Wake word detected: ");
   Serial.println(KEYWORDS_NAME[keyword_index]);
   rtc.update();
  // printTime();       // each time keyword detected, time is printed 
  int currentWinner;
///////////////////////////////////////////////////////////////

  
  if (KEYWORDS_NAME[keyword_index] == "something to eat") {
    numSnacks++;
    currentWinner = 0;
    rtc.update();               
    int h = rtc.hour();        
        
        if (numSnacks <= 2) {
            if (h >= 8 && h <= 11 && h >= 14 && h <= 22){
                printTime();
                Serial.println("OK");
                digitalWrite(GREEN, LOW);
                digitalWrite(RED, HIGH);
                servoUpYes();
                
            }
            else {
                printTime();
                Serial.println("Nope");
                digitalWrite(RED, LOW);
                digitalWrite(GREEN, HIGH);
                servoUpNo();
            }
        } else if (numSnacks == 3) {
                printTime();
                Serial.println("No more asking!");
                digitalWrite(RED, LOW);
                digitalWrite(GREEN, HIGH);
                servoUpNo();
          }
          if (numSnacks == 3) {
            numSnacks = 0;
          }
      }
 
 if (KEYWORDS_NAME[keyword_index] == "watch tv") {
    numWatch++;      
    currentWinner = 1;
    int h = rtc.hour();          
         if (numWatch <= 2) {
            if (h >= 11 && h <= 22){
                printTime();
                Serial.println("OK");
                digitalWrite(GREEN, LOW);
                digitalWrite(RED, HIGH);
                servoUpYes();
            }
            else {
              printTime();
                Serial.println("Nope");
                digitalWrite(RED, LOW);
                digitalWrite(GREEN, HIGH);
                servoUpNo();
            }
        }   else if  (numWatch == 3) {
                  printTime();
                  Serial.println("No more asking!");
                  digitalWrite(RED, LOW);
                  digitalWrite(GREEN, HIGH);
                  servoUpNo();
          }
          if (numWatch == 3) {
            numWatch = 0;
          }
        }
  
    
 if (KEYWORDS_NAME[keyword_index] == "play row blocks") {
    numBlox++;  
    currentWinner = 2;         
    int h = rtc.hour();         // need this each time ? def helps get hour! 

         if (numBlox <= 2) {
            if (h >= 15 && h <= 22) {
                printTime();
                Serial.println("OK");
                digitalWrite(GREEN, LOW);
                digitalWrite(RED, HIGH);
                servoUpYes();
                
            }
            else {
                printTime();
                Serial.println("Nope");
                digitalWrite(RED, LOW);
                digitalWrite(GREEN, HIGH);
                servoUpNo();
            }
        }   else if (numBlox == 3) {
                printTime();
                Serial.println("No more asking!");
                digitalWrite(RED, LOW);
                digitalWrite(GREEN, HIGH);
                servoUpNo();
        }
          if (numBlox == 3) {
            numBlox = 0;
          }
    }
    }

void setup() {

    Serial.begin(9600); 
    while (!Serial);

    rtc.begin();  // initialize RTC library    
 rtc.autoTime(); // set the time 
  //rtc.set12Hour(); // Use rtc.set12Hour to set to 12-hour mode
    
    pv_status_t status = pv_audio_rec_init();
    if (status != PV_STATUS_SUCCESS) {
        Serial.print("Audio init failed with ");
        Serial.println(pv_status_to_string(status));
        while (1);
    }

    status = pv_porcupine_init(
            ACCESS_KEY,
            MEMORY_BUFFER_SIZE,
            memory_buffer,
            NUM_KEYWORDS,
            KEYWORD_MODEL_SIZES,
            KEYWORD_MODELS,
            SENSITIVITIES,
            &handle);

    if (status != PV_STATUS_SUCCESS) {
        Serial.print("Picovoice init failed with ");
        Serial.println(pv_status_to_string(status));
        while (1);
    }

    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    myservo1.attach(2);
    myservo2.attach(3);
    Serial.println("The board is listening for 'play row blocks', 'something to eat', and 'watch tv'...");
}

void servoUpYes() {
  myservo1.write(180);
  delay(1000);
  myservo1.write(0);
}

void servoUpNo() {
  myservo2.write(0);
  delay(1000);
  myservo2.write(180);
}

void printTime()
{
  Serial.print(String(rtc.hour()) + ":"); // Print hour
  if (rtc.minute() < 10)
    Serial.print('0'); // Print leading '0' for minute
  Serial.print(String(rtc.minute()) + ":"); // Print minute
  if (rtc.second() < 10)
    Serial.print('0'); // Print leading '0' for second
  Serial.println(String(rtc.second())); // Print second
}  

void loop()
{
  static int8_t lastSecond = -1;
   rtc.update();
  // if (rtc.second() != lastSecond) // If the second has changed
  // {
  //   printTime(); // Print the new time
    
  //   lastSecond = rtc.second(); // Update lastSecond value
  // }
   

    const int16_t *buffer = pv_audio_rec_get_new_buffer();
    if (buffer) {
        int32_t keyword_index;
        const pv_status_t status = pv_porcupine_process(handle, buffer, &keyword_index);
        if (status != PV_STATUS_SUCCESS) {
            Serial.print("Picovoice process failed with ");
            Serial.println(pv_status_to_string(status));
            while(1);
        }
        if (keyword_index != -1) {
            wake_word_callback(keyword_index);
        }
    }
}