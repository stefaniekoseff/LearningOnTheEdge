



// If your target is limited in memory remove this macro to save 10K RAM
#define EIDSP_QUANTIZE_FILTERBANK   0

/**
 * Define the number of slices per model window. E.g. a model window of 1000 ms
 * with slices per model window set to 4. Results in a slice size of 250 ms.
 * For more info: https://docs.edgeimpulse.com/docs/continuous-audio-sampling
 */
#define EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW 3


/* Includes ---------------------------------------------------------------- */
#include <PDM.h>
#include <MomRobot_2_inferencing.h>

#include <SparkFunDS1307RTC.h>
#include <Wire.h>

int lastPrediction =  1;  // room tone 
int numSnacks = 0;
int numBlox = 0;
int numWatch = 0;

int s = rtc.second();
int m = rtc.minute();
int h = rtc.hour();

int RED = 22;
int GREEN = 23;


/** Audio buffers, pointers and selectors */
typedef struct {
    signed short *buffers[2];
    unsigned char buf_select;
    unsigned char buf_ready;
    unsigned int buf_count;
    unsigned int n_samples;
} inference_t;

static inference_t inference;
static bool record_ready = false;
static signed short *sampleBuffer;
static bool debug_nn = false; // Set this to true to see e.g. features generated from the raw signal
static int print_results = -(EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW);

/**
 * @brief      Arduino setup function
 */
void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);

    rtc.begin();  // initialize RTC library    
   rtc.autoTime(); // set the time 
  //rtc.set12Hour(); // Use rtc.set12Hour to set to 12-hour mode

    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
      

    Serial.println("Edge Impulse Inferencing Demo");

    // summary of inferencing settings (from model_metadata.h)
    ei_printf("Inferencing settings:\n");
    ei_printf("\tInterval: %.2f ms.\n", (float)EI_CLASSIFIER_INTERVAL_MS);
    ei_printf("\tFrame size: %d\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
    ei_printf("\tSample length: %d ms.\n", EI_CLASSIFIER_RAW_SAMPLE_COUNT / 16);
    ei_printf("\tNo. of classes: %d\n", sizeof(ei_classifier_inferencing_categories) /
                                            sizeof(ei_classifier_inferencing_categories[0]));

    run_classifier_init();
    if (microphone_inference_start(EI_CLASSIFIER_SLICE_SIZE) == false) {
        ei_printf("ERR: Failed to setup audio sampling\r\n");
        return;
    }
}

/**
 * @brief      Arduino main function. Runs the inferencing loop.
 */
void loop()                                                       //////////////Looop 
{
    bool m = microphone_inference_record();
    if (!m) {
        ei_printf("ERR: Failed to record audio...\n");
        return;
    }

    signal_t signal;
    signal.total_length = EI_CLASSIFIER_SLICE_SIZE;
    signal.get_data = &microphone_audio_signal_get_data;
    ei_impulse_result_t result = {0};

    EI_IMPULSE_ERROR r = run_classifier_continuous(&signal, &result, debug_nn);
    if (r != EI_IMPULSE_OK) {
        ei_printf("ERR: Failed to run classifier (%d)\n", r);
        return;
    }
      static int8_t lastSecond = -1;
      rtc.update();
      // if (rtc.second() != lastSecond) // If the second has changed
      // {
      // printTime(); // Print the new time
      // lastSecond = rtc.second(); // Update lastSecond value
      // }

    
    if (++print_results >= (EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW)) {
       // print the predictions
      ei_printf("Predictions ");
        ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
                  result.timing.dsp, result.timing.classification, result.timing.anomaly);
        ei_printf(": \n");
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            ei_printf("    %s: %.5f\n", result.classification[ix].label,
                      result.classification[ix].value);
        }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
        ei_printf("    anomaly score: %.3f\n", result.anomaly);
#endif

        print_results = 0;
    }

      /// DO STUFF HERE ///
     int currentWinner;
     
      // can i play roblox 

      if (result.classification[0].value >= 0.65) {   
              currentWinner = 0;
              triggerProperIncrement(0);
              rtc.update();
              int h = rtc.hour();         // need this each time ? def helps get hour!
                if (numBlox <= 2) {
                    if (h >= 15 && h <= 18)
                    {
                      printTime();
                      Serial.println("ROBLOX OK");
                      digitalWrite(GREEN, LOW);
                      digitalWrite(RED, HIGH);
                    }
                    else  {
                      printTime();
                      Serial.println("ROBLOX Nope");
                      digitalWrite(RED, LOW);
                      digitalWrite(GREEN, HIGH);
                }
                } else  {
                      Serial.print("You have asked to play Roblox  ");      
                      Serial.println(numBlox);  
                }   
                        
              }
      else if (result.classification[3].value >= 0.65) {          // Can I watch TV
                currentWinner = 3;
                triggerProperIncrement(3);
                rtc.update();
                int h = rtc.hour();         // need this each time ? def helps get hour! 
                  if (numWatch <= 2) {
                    if (h >= 14 && h <= 18) {
                    printTime();
                    Serial.println("TV OK");
                    digitalWrite(GREEN, LOW);
                    digitalWrite(RED, HIGH);
                  }
                   else {
                    printTime();
                    Serial.println("TV Nope");
                    digitalWrite(RED, LOW);
                    digitalWrite(GREEN, HIGH);
                  }
                  } else {
                  Serial.print("You have asked to watch TV  ");      
                  Serial.println(numWatch);  
              }
              
      }
      else if (result.classification[2].value >= 0.65) {  // Can I have a snack
              currentWinner = 2;
              triggerProperIncrement(2);
              rtc.update();               
              int h = rtc.hour();          
              if (numSnacks <= 2) {
                if (h >= 8 && h <= 11 || h >= 14 && h <= 17) {
                printTime();
                Serial.println("SNACK OK");
                digitalWrite(GREEN, LOW);
                digitalWrite(RED, HIGH);  
              }
                else {
                printTime();
                Serial.println("SNACK Nope");
                digitalWrite(RED, LOW);
                digitalWrite(GREEN, HIGH);
                }
              }  else {
              Serial.print("You have asked for a snack  ");      
              Serial.println(numSnacks); 
        } 
     
      }

      if (currentWinner != lastPrediction) {
        triggerProperIncrement(currentWinner);
      } 
          if (numSnacks == 3) {
            numSnacks = 0;      
            }

             if (numWatch == 3) {
                  numWatch = 0;
              }

               if (numBlox == 3) {
                  numBlox = 0;
                } 

 lastPrediction = currentWinner;
 
}                                                         ////// end loop

void triggerProperIncrement(int value) {
        if (value == 0) {
        Serial.println("ROBLOX ASK");
        numBlox++; 
      } else if (value == 3 ) {
        Serial.println("TV ASK");
        numWatch++;
      } else if (value == 2) {
        Serial.println("SNACK ASK");
        numSnacks++;
      }
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

/**
 * @brief      PDM buffer full callback
 *             Get data and call audio thread callback
 */
static void pdm_data_ready_inference_callback(void)
{
    int bytesAvailable = PDM.available();

    // read into the sample buffer
    int bytesRead = PDM.read((char *)&sampleBuffer[0], bytesAvailable);

    if (record_ready == true) {
        for (int i = 0; i<bytesRead>> 1; i++) {
            inference.buffers[inference.buf_select][inference.buf_count++] = sampleBuffer[i];

            if (inference.buf_count >= inference.n_samples) {
                inference.buf_select ^= 1;
                inference.buf_count = 0;
                inference.buf_ready = 1;
            }
        }
    }
}

/**
 * @brief      Init inferencing struct and setup/start PDM
 *
 * @param[in]  n_samples  The n samples
 *
 * @return     { description_of_the_return_value }
 */
static bool microphone_inference_start(uint32_t n_samples)
{
    inference.buffers[0] = (signed short *)malloc(n_samples * sizeof(signed short));

    if (inference.buffers[0] == NULL) {
        return false;
    }

    inference.buffers[1] = (signed short *)malloc(n_samples * sizeof(signed short));

    if (inference.buffers[1] == NULL) {
        free(inference.buffers[0]);
        return false;
    }

    sampleBuffer = (signed short *)malloc((n_samples >> 1) * sizeof(signed short));

    if (sampleBuffer == NULL) {
        free(inference.buffers[0]);
        free(inference.buffers[1]);
        return false;
    }

    inference.buf_select = 0;
    inference.buf_count = 0;
    inference.n_samples = n_samples;
    inference.buf_ready = 0;

    // configure the data receive callback
    PDM.onReceive(&pdm_data_ready_inference_callback);

    PDM.setBufferSize((n_samples >> 1) * sizeof(int16_t));

    // initialize PDM with:
    // - one channel (mono mode)
    // - a 16 kHz sample rate
    if (!PDM.begin(1, EI_CLASSIFIER_FREQUENCY)) {
        ei_printf("Failed to start PDM!");
    }

    // set the gain, defaults to 20
    PDM.setGain(127);

    record_ready = true;

    return true;
}

/**
 * @brief      Wait on new data
 *
 * @return     True when finished
 */
static bool microphone_inference_record(void)
{
    bool ret = true;

    if (inference.buf_ready == 1) {
        ei_printf(
            "Error sample buffer overrun. Decrease the number of slices per model window "
            "(EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW)\n");
        ret = false;
    }

    while (inference.buf_ready == 0) {
        delay(1);
    }

    inference.buf_ready = 0;

    return ret;
}

/**
 * Get raw audio signal data
 */
static int microphone_audio_signal_get_data(size_t offset, size_t length, float *out_ptr)
{
    numpy::int16_to_float(&inference.buffers[inference.buf_select ^ 1][offset], out_ptr, length);

    return 0;
}

/**
 * @brief      Stop PDM and release buffers
 */
static void microphone_inference_end(void)
{
    PDM.end();
    free(inference.buffers[0]);
    free(inference.buffers[1]);
    free(sampleBuffer);
}

#if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_MICROPHONE
#error "Invalid model for current sensor."
#endif
