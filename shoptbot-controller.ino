/* 
 * This is simple program that uses the Arduino Esplora
 * to control Pumping Station: One's (http://pumpingstationone.org/) 
 * ShopBot CNC machine.
 * The purpose of having an extra controller is to allow for the
 * ShopBot user to have the Esplora act as a remote keyboard so 
 * he or she can position the bit where it needs to be without dragging
 * the actual computer keyboard around.
 *
 * This version uses the Arduino Esplora (http://arduino.cc/en/Main/ArduinoBoardEsplora)
 * but presumably can be retrofitted to any hardware.
 *
 * Usage:
 *    1. Press the joystick button to turn on the keyboard. The light
 *       will turn white to indicate that the keyboard is on.
 *    2. The joystick is mapped to the arrow keys. Move the stick to the
 *       left to act as moving the left arrow key. Same deal with the other
 *       keys.
 *    3. The up and down buttons on the right side of the Esplora map to
 *       the page up and page down buttons, respectively.
 *
 *
 * Version 1.0: 1/3/2015 by Ron Olson
 */
 
/* All the Arduino calls other than Keyboard are Esplora specific */
#include <Esplora.h>

/* Hold onto the previous values */
int prevXValue = 0;
int prevYValue = 0;

/* 
 * The joystick position has a "flutter range" when it's sitting at zero,
 * so a determination was made by playing around that 11 was enough to 
 * handle touching the joystick, but won't affect movement
 */
const int ZEROISH_POS = 11;
const int ZEROISH_NEG = -11;

/* Turned on or off by the button state on the joystick */
int doWork = 0;

void setup() {
  // initialize the serial communication:
  Serial.begin(9600);
  
  /* And capture the keyboard */
  Keyboard.begin();
}

void loop()
{
  int xValue = Esplora.readJoystickX();        // read the joystick's X position
  int yValue = Esplora.readJoystickY();        // read the joystick's Y position
  int button = Esplora.readJoystickSwitch();   // read the joystick pushbutton

  /*
   * This section determines whether the keyboard is on.
   * Press the button to turn on the keyboard, which will
   * also turn the light on. Press it again to turn the
   * keyboard off, which will also turn off the light
   */
  if (button == 0)
  {
    if (doWork == 1)
    {
      Serial.print("Turning off the keyboard\n");
      doWork = 0;
      Esplora.writeRGB(0, 0, 0);
    }
    else
    {
      Serial.print("Turning on the keyboard\n");
      doWork = 1;  
      Esplora.writeRGB(255, 255, 255);
    }
    
    /*
     * There's a separate delay for the button to prevent
     * it interfering with the joystick position 
     */    
    delay(500);
  }

  /*
   * If the keyboard is on...
   */
  if (doWork == 1)
  {
    /*
     * This section is for the right-side of the controller,
     * which handles the page-up and page-down
     */
    int downButton = Esplora.readButton(SWITCH_1);
    int upButton = Esplora.readButton(SWITCH_3);

    /* Guard against both buttons being pressed */
    if (!downButton != !upButton)
    {
      /* 
       * he buttons are a straight-binary thing, either
       * 0 (pressed) or 1 (not pressed)
       */
      if (downButton == 0)
        Keyboard.write(KEY_PAGE_DOWN);
      else if (upButton == 0)
        Keyboard.write(KEY_PAGE_UP); 

      // write the red color to indicate page up and page down
      Esplora.writeRGB(255, 0, 0);
    }

    /*
     * Handle the left and right motion
     */
    if (xValue < ZEROISH_NEG || xValue > ZEROISH_POS)
    {
      if (xValue >= prevXValue && (xValue > ZEROISH_NEG))
      {
        Keyboard.write(KEY_LEFT_ARROW);
      }
      else if ((xValue <= prevXValue) && (xValue < ZEROISH_POS))
      {
        Keyboard.write(KEY_RIGHT_ARROW);
      }

      // Set green on the LED to indicate side-to-side motion
      Esplora.writeRGB(0, 255, 0);
    }

    /*
     * Now handle the up and down of the joystick
     */
    if (yValue < ZEROISH_NEG ||  yValue > ZEROISH_POS)
    { 
      if ((yValue <= prevYValue) && (yValue < ZEROISH_NEG))
      {
        Keyboard.write(KEY_UP_ARROW);
      }
      else if ((yValue >= prevYValue) && (yValue > ZEROISH_POS))   
      {
        Keyboard.write(KEY_DOWN_ARROW);
      }

      // Set the LED to blue to indicate up and down motion
      Esplora.writeRGB(0, 0, 255);
    }

    prevXValue= xValue;
    prevYValue = yValue;
  }

  // Put in delay of 300ms so as to not overwhelm the system
  delay(300);

  // And set the light back to white if we're doing work
  if (doWork == 1)
    Esplora.writeRGB(255, 255, 255);
}
