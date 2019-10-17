#include <RotaryEncoder.h>

#define SX1278ATTACHED

#ifdef SX1278ATTACHED
#include <RadioLib.h>
#endif

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h> // Hardware-specific library
#include <Fonts/FreeSans9pt7b.h>

#define _sclk 13
#define _miso 12
#define _mosi 11
#define _cs 10
#define _dc 9
#define _rst 8
Adafruit_ILI9341 tft = Adafruit_ILI9341(_cs, _dc, _rst);

#ifdef SX1278ATTACHED
SX1278 LORA = new Module(7, 2, 3);
#endif

RotaryEncoder encoder(A1, A2);

#define BLACK   ILI9341_BLACK
#define BLUE    0x001F
#define RED     ILI9341_RED
#define GREEN   ILI9341_GREEN
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   ILI9341_WHITE

uint16_t FONT_HEIGHT = 18;
uint16_t SCREEN_TOOLBAR_HEIGHT = 32;

float CARRIER_FREQUENCY = 434.0f;

/////////////////////////////////////
// Radio (Lora chip) configuration //
/////////////////////////////////////
// All configurations.
#define SYNC_WORD 0x0F

// Non ISM band LORA mode
#define BANDWIDTH 125.0f //62.5f // KHz low bandiwdth mode for communicating with ground stations
#define SPREADING_FACTOR 9
#define CODING_RATE 8 // can be 12.
#define OUTPUT_POWER 6 // dBm

// ISM band LORA mode.
#define ISM_CARRIER_FREQUENCY 435.0f
#define ISM_BANDWIDTH 125.0f 
#define ISM_SPREADING_FACTOR 9
#define ISM_CODING_RATE 8 // can be 12.
#define ISM_OUTPUT_POWER 10 // dBm

// Non ISM band FSK mode.
#define FSK_CARRIER_FREQUENCY 434.0f
#define FSK_CURRENT_LIMIT 10
#define FSK_FREQUENCY_DERIVATION 10.0
#define FSK_RX_BANDWIDTH 250.0
#define FSK_BIT_RATE 100.0
#define FSK_DATA_SHAPING 0.5
#define FSK_OUTPUT_POWER 10 // dBm

/*
 * @brief switches SX1278 modem mode to FSK.
 */
void Communication_SwitchFSK()
{
  int err, startupErr;
  
  startupErr = LORA.beginFSK();
  
  err = LORA.setFrequency(FSK_CARRIER_FREQUENCY);
  
  err = LORA.setBitRate(FSK_BIT_RATE);
  
  err = LORA.setFrequencyDeviation(FSK_FREQUENCY_DERIVATION);
  
  err = LORA.setRxBandwidth(FSK_RX_BANDWIDTH);
  
  err = LORA.setOutputPower(FSK_OUTPUT_POWER);
  
  err = LORA.setCurrentLimit(FSK_CURRENT_LIMIT);
  
  err = LORA.setDataShaping(FSK_DATA_SHAPING);
  
  uint8_t syncWord[] = { SYNC_WORD };
  err = LORA.setSyncWord(syncWord, 1);
}

void Communication_SwitchLORA()
{
  int err;
  err = LORA.begin(CARRIER_FREQUENCY, BANDWIDTH, SPREADING_FACTOR, CODING_RATE, SYNC_WORD, OUTPUT_POWER);
  Serial.println(err);
}

void Communication_SwitchLORA_ISM()
{
  int err;
  err = LORA.begin(ISM_CARRIER_FREQUENCY, ISM_BANDWIDTH, ISM_SPREADING_FACTOR, ISM_CODING_RATE, SYNC_WORD, ISM_OUTPUT_POWER);
}

class console
{
public:
  
  void RenderToolbar()
  {
    int ox, oy;
    ox = 0;
    oy = tft.height() - SCREEN_TOOLBAR_HEIGHT;
    
    tft.fillRect(ox, oy, tft.width(), SCREEN_TOOLBAR_HEIGHT, BLACK);
    
    tft.setTextColor(WHITE);

    tft.setCursor(10, tft.height() - 8);
    int16_t  x1, y1;
    uint16_t w, h;
    tft.getTextBounds(F("FOSSA"), 10, tft.height() - 8, &x1, &y1, &w, &h);
    tft.print("FOSSA");
    

    String freqText = String(F("OPERATING @ ")) + String(CARRIER_FREQUENCY);
    tft.getTextBounds(freqText, tft.width() / 4, tft.height() - 8, &x1, &y1, &w, &h);
    tft.setCursor(tft.width() - w - 10, tft.height() - 8);
    if (!IsRunning)
    {
      tft.setTextColor(RED);
    }
    else
    {
      tft.setTextColor(WHITE);
    }
    tft.print(freqText);

    tft.setTextColor(WHITE);
  }

  void Render()
  {
    // starting at the most recent index.
    int mostRecentIndex = _MessageListCurrentIndex;
    
    // loop through the lower _MessageListCount.
    for (int i = 1; i < _MessageListCount; i++)
    {
       int previousMessageIndex = mostRecentIndex - i;

       if (previousMessageIndex < 0)
       {
          previousMessageIndex = _MessageListCount - i;
       }

       String thisMessage = _Messages[previousMessageIndex];

        int y = tft.height() - SCREEN_TOOLBAR_HEIGHT;
        y -= 4;
  
    
        int numberOfPixelsInMessage = thisMessage.length() * 12;
        int numberOfYLines = 1 + floor(numberOfPixelsInMessage / tft.width());
        y -= ( FONT_HEIGHT * numberOfYLines ) * i;
   
        int x = 2;
  
        // render the most recent on at the bottom
        tft.setCursor(x, y);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print(String("> ") + thisMessage);
    }

    RenderToolbar();
  }
  
  void AddMessage(String pMessage)
  {
      for (byte i = 0; i < 64; i++)
      {
        _Messages[_MessageListCurrentIndex][i] = pMessage.charAt(i);
      }
      _Messages[_MessageListCurrentIndex][64] = '\0';
      
      _MessageListCurrentIndex++;
      
      if (_MessageListCurrentIndex >= _MessageListCount) // wrap around.
      {
        _MessageListCurrentIndex = 0;
      }
  }

  void Pause()
  {
      IsRunning = false;
  }
  
  void Play()
  {
     IsRunning = true;
  }

  void Error()
  {
    IsRunning = false;
    IsError = true;
  }

  void AssignMemory()
  {
    for (byte listIndex = 0; listIndex < 7; listIndex++)
    {
      for (byte charIndex = 0; charIndex < 63; charIndex++)
      {
        _Messages[listIndex][charIndex] = '1';
      }
    }
  }
  
  bool IsRunning = true;
  bool IsError = false;
private:
  char _Messages[8][64];
  
  byte _MessageListCurrentIndex = 3; // where we start rendering from at the bottom.
  
  byte _MessageListCount = 7; // how many messages we render upwards.
};

console c;

void setup()
{
    Serial.begin(9600);
    tft.begin();

    tft.setFont(&FreeSans9pt7b);
    
    tft.setRotation(3);
    
    tft.fillScreen(BLACK);
  
    tft.setCursor(0, 0);
    Serial.println("Running");

    c.AssignMemory();

    
#ifdef SX1278ATTACHED
    Communication_SwitchLORA();

    LORA.setDio0Action(setFlag);

    LORA.startReceive();
 #endif

  RedrawConsole();

  Serial.println("End of setup");
}



volatile bool receivedFlag = false;
volatile bool enableInterrupt = true;

void setFlag(void)
{
  Serial.println("DATA");
  if (!enableInterrupt)
  {
    return;
  }
  enableInterrupt = false;
  
  receivedFlag = true;
}




void RedrawConsole()
{
  tft.fillScreen(WHITE);
  c.Render();
}

unsigned long timestampLastTick = millis();
boolean carrierFrequencyChanged = false;

int pos = 0;
void loop()
{
  encoder.tick();
  int newPos = encoder.getPosition();
  if (pos != newPos)
  {
    Serial.println("Moved");
    carrierFrequencyChanged = true;
    
    CARRIER_FREQUENCY = 434.0f + (newPos * 0.5f);

    timestampLastTick = millis();
   
    pos = newPos;
  }

  if (carrierFrequencyChanged)
  {
      unsigned long timeSinceEncoderTick = millis() - timestampLastTick;
      if (timeSinceEncoderTick >= 250)
      {
        int state = LORA.setFrequency(CARRIER_FREQUENCY);
        Serial.print(F("Updated radio settings, status code "));
        Serial.println(state);
      
        RedrawConsole();

        carrierFrequencyChanged = false;
      }   
  }




  if (receivedFlag)
  {
    Serial.println("Recieved packet...");
    
    // process data.
    receivedFlag = false;
    
    if (c.IsRunning)
    {
      #ifdef SX1278ATTACHED
        String str;
        int state = LORA.readData(str);
    
        if (state == ERR_NONE)
        {
        
          // packet was successfully received
          Serial.println(F("success!"));
      
          // print the data of the packet
          Serial.print("[SX1278] Data:\t\t");
          Serial.println(str);
      
          // print the RSSI (Received Signal Strength Indicator)
          // of the last received packet
          Serial.print("[SX1278] RSSI:\t\t");
          Serial.print(LORA.getRSSI());
          Serial.println(" dBm");
      
          // print the SNR (Signal-to-Noise Ratio)
          // of the last received packet
          Serial.print("[SX1278] SNR:\t\t");
          Serial.print(LORA.getSNR());
          Serial.println(" dBm");
      
          // print frequency error
          // of the last received packet
          Serial.print("Frequency error:\t");
          Serial.print(LORA.getFrequencyError());
          Serial.println(" Hz");
  #endif

       c.AddMessage(str);
       RedrawConsole();
  
  #ifdef SX1278ATTACHED
        }
        else if (state == ERR_RX_TIMEOUT)
        {
          Serial.println(F("timeout!"));
        }
        else if (state == ERR_CRC_MISMATCH)
        {
          // packet was received, but is malformed
          Serial.println(F("CRC error!"));
      } 
  #endif
  
    }
    else
    {
      // do nothing
      Serial.println("Console not running.");
      delay(1000);
    }

    enableInterrupt = true;

    Serial.println("Done");
  }
}
