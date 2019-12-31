//STEPPER BOX 2 Begun 04/October 2016 - current 20th May 2017
// Global variables to have capital letters
//#define ENCODER_OPTIMIZE_INTERUPTS // Use if time overhead is crucial
#include <Encoder.h>
#include <LiquidCrystal_I2C.h>
Encoder myencoder(2, 3); // both of these are interupt pins - gives highest fidelity
int Button_pin = 13;// pin that connects to encoder switch/button
int Upper_limit = 8; // initial max for encoder
int Lower_limit = 1; // initial min for encoder
int Increment = 10; // initial value for multilier re: durations, distance etc
int Direction; // intial direct rail will move 0 = forward
int Rail_speed = 400; // sets delay between driver pulses
long int Distance;
float microns_per_step = 5.0;
int Number_photos;
int Delay_1;// Time over which railmovement settles down
int Delay_2;// Exposure time during which rail halts
int Dir_pin = 11; // Set initial pins to those for Z axes
int Drive_pin = 12; // initial pin for Z stepper motion
int Enable_pin = 10; // initial pin to enable Z stepper motor
int Camera_pin = 15;// connected to camera remote shutter
int Pre_focus_pin = 16;// connected to camera remote prefocus
int Flash_pin = 17;// connects to flash socket - rename and initialise if using more than one flash - ie Flash_2_pin = 19
int Motor_selected = 3; //start with Z motor selected
int Number_Photos_hold;
int Direction_hold;
int Distance_hold;
int Delay_1_hold;
int Delay_2_hold;
int Delay_shutter_lag = 100; // sets period required to activate camera
byte Reuse_Z_values;
byte Button_flag; // flag for button press
byte Old_button_flag = HIGH; // carry old value of button forward

String Main_Menu_Title = "   --MAIN  MENU--   ";
//                 12345678901234567890
String Option_1 = "       Stack        ";
String Option_2 = "        Jog         ";
String Option_3 = "       Rotate       ";
String Option_4 = "       Shoot        ";
String Option_5 = "       Flash        ";
String Option_6 = "    Shoot & flash   ";
String Option_7 = " Motor -  X,Y,Z,A,B ";
String Option_8 = "Distance multiplier ";
String Explain_1 = "  (Create Z stack) ";
String Explain_2 = " (Jog back/forward)";
String Explain_3 = " (Control A,B Axes)";
String Explain_4 = " (Fire w/out flash)";
String Explain_5 = "    (Fire flash)   ";
String Explain_6 = "  (Fire and flash) ";
String Explain_7 = "   (Select motor)  ";
String Explain_8 = "(Click sensitivity)";
LiquidCrystal_I2C lcd(0x3F, 20, 4); // set address for LCD on I2C
void setup()
{
  pinMode(10, OUTPUT);// set all motor enable pins as outputs
  pinMode(20, OUTPUT);
  pinMode(23, OUTPUT);
  pinMode(29, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(52, OUTPUT);// set stepsize connected to MS1 on easydriver - not yet implemented for other axes
  pinMode(53, OUTPUT);// set stepsize connected to MS1 on easydriver
  digitalWrite(10, HIGH); // set all motor enable pins to off = HIGH
  digitalWrite(20, HIGH);
  digitalWrite(23, HIGH);
  digitalWrite(29, HIGH);
  digitalWrite(32, HIGH);
  digitalWrite(52, HIGH);//set 1/8th step on board B - not yet implemented for other axes
  digitalWrite(53, HIGH);//set 1/8th step on board B
  pinMode(15, OUTPUT);//configure current flash and camera pins as outputs
  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
  pinMode(18, OUTPUT);
  // others as one wishes
  pinMode (Button_pin, INPUT_PULLUP);
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  //         12345678901234567890
  lcd.print("  M A D B O F F I N ");
  lcd.setCursor(0, 1);
  lcd.print("       L A B S      ");
  lcd.setCursor(0, 2);
  lcd.print("Software version 1.5");
  lcd.setCursor(0, 3);
  lcd.print("   24th Dec 2019    ");
  delay(4000);
  lcd.clear();
}

void loop()

{
  lcd.setCursor(0, 0);
  lcd.print(Main_Menu_Title);
  int k = encode_return(Upper_limit, Lower_limit); // call subroutine with upper and lower limits, k holds value returned from encoder
  // Spell out menu options using encoder to select
  switch (k) {
    case 1:
      lcd.setCursor(0, 1);
      lcd.print(Option_1);
      lcd.setCursor(0, 2);
      lcd.print(Explain_1);
      break;
    case 2:
      lcd.setCursor(0, 1);
      lcd.print(Option_2);
      lcd.setCursor(0, 2);
      lcd.print(Explain_2);
      break;
    case 3:
      lcd.setCursor(0, 1);
      lcd.print(Option_3);
      lcd.setCursor(0, 2);
      lcd.print(Explain_3);
      break;
    case 4:
      lcd.setCursor(0, 1);
      lcd.print(Option_4);
      lcd.setCursor(0, 2);
      lcd.print(Explain_4);
      break;
    case 5:
      lcd.setCursor(0, 1);
      lcd.print(Option_5);
      lcd.setCursor(0, 2);
      lcd.print(Explain_5);
      break;
    case 6:
      lcd.setCursor(0, 1);
      lcd.print(Option_6);
      lcd.setCursor(0, 2);
      lcd.print(Explain_6);
      break;
    case 7:
      lcd.setCursor(0, 1);
      lcd.print(Option_7);
      lcd.setCursor(0, 2);
      lcd.print(Explain_7);
      break;
    case 8:
      lcd.setCursor(0, 1);
      lcd.print(Option_8);
      lcd.setCursor(0, 2);
      lcd.print(Explain_8);
      break;
    default:
      ;
  }
  Button_flag = digitalRead(Button_pin);
  if (Button_flag != Old_button_flag)
  {
    Old_button_flag = Button_flag;
    delay(100);
  }
  if (Button_flag == LOW && k == 1)
  {
    stacker();
  }
  else if (Button_flag == LOW && k == 2)
  {
    jogger();
  }
  else if (Button_flag == LOW && k == 3)
  {
    rotate();
  }
  else if (Button_flag == LOW && k == 4)
  {
    shoot();
  }
  else if (Button_flag == LOW && k == 5)
  {
    flash();
  }
  else if (Button_flag == LOW && k == 6)
  {
    shoot_flash();
  }
  else if (Button_flag == LOW && k == 7)
  {
    select_motor();
  }
  else if (Button_flag == LOW && k == 8)
  {
    click_Increment();
  }
}

//
// SUBROUTINE encode_return () is sent upper and lower limits and returns encoder_value
//

int encode_return (int Upper_limit, int Lower_limit) {
  int new_encoder_value = -999;
  int mult_enc;
  new_encoder_value = myencoder.read();
  new_encoder_value = new_encoder_value / 4; // because each click increases encoder value by 4
  // now test to see if limits exceeded
  if (new_encoder_value >= Upper_limit) {
    new_encoder_value = Upper_limit;
    mult_enc = (Upper_limit * 4); //not clear why "if" not blocking code here????? add a 2...take it out or not??!
    myencoder.write(mult_enc);
  }
  if (new_encoder_value <= Lower_limit) {
    new_encoder_value = Lower_limit;
    mult_enc = (Lower_limit * 4) + 2; //add 2 to stop "if" blocking code
    myencoder.write(mult_enc);
  }
  return new_encoder_value;
}

//
// SUBROUTINE stacker () creates a stack of photos
//

void stacker() {
  Direction = 1;
  Number_photos = 1;
  Distance = 0;
  Delay_1 = 0;
  Delay_2 = 0;
  byte Old_button_flag = HIGH;
  byte Button_flag = HIGH;
  if (Motor_selected > 3)
  {
    lcd.clear();
    //         01234567890123456789
    lcd.setCursor(0, 0);
    lcd.print("--------------------");
    lcd.setCursor(0, 1);
    lcd.print("Use X, Y or Z motor ");
    lcd.setCursor(0, 2);
    lcd.print("Choose when offered!");
    lcd.setCursor(0, 3);
    lcd.print("--------------------");
    delay(1500);
    lcd.clear();
    select_motor();
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  //         12345678901234567890
  lcd.print("--STACKER SUB-MENU--");
  //
  // do while loop here for direction to run
  //
  lcd.setCursor(0, 1);
  //         12345678901234567890
  lcd.print("    (Direction)     ");
  if (Reuse_Z_values == HIGH)
  {
    myencoder.write(Direction_hold * 4);
  }
  do
  {
    Direction = encode_return(2, 1);
    if (Direction == 1)
    {
      lcd.setCursor(0, 2);
      lcd.print("     **Forward**    ");
    }
    if (Direction == 2)
    {
      lcd.setCursor(0, 2);
      lcd.print("     **Reverse**    ");
    }
    Button_flag = digitalRead(Button_pin);
    if (Button_flag != Old_button_flag)
    {
      Old_button_flag = Button_flag;
      delay(30);
    }
  } while (Button_flag == HIGH);
  Direction_hold = Direction;
  myencoder.write(4); // return to a value of 1 for encoder - divide by four!
  delay(200); // inserting this delay ends problems with 'bouncing by' setting number of photos
  //         12345678901234567890
  // do while loop here for number of photos
  Old_button_flag = HIGH;
  Button_flag = HIGH;
  lcd.setCursor(0, 1);
  //         12345678901234567890
  lcd.print("  Number of Photos: ");
  lcd.setCursor(0, 2);
  lcd.print("                    ");
  if (Reuse_Z_values == HIGH)
  {
    myencoder.write(Number_Photos_hold * 4);
  }
  do
  {
    Number_photos = encode_return(1000, 1);
    lcd.setCursor(10, 2);
    lcd.print(Number_photos);
    lcd.print("    ");
    Button_flag = digitalRead(Button_pin);
    if (Button_flag != Old_button_flag)
    {
      Old_button_flag = Button_flag;
      delay(30);
    }
  } while (Button_flag == HIGH);
  Number_Photos_hold = Number_photos;
  myencoder.write(4); // return to a value of 1 for encoder - divide by four!
  delay(200); // inserting this delay ends problems with 'bouncing by' setting distance to run
  //
  // do while loop here for distance to run
  //
  lcd.setCursor(0, 1);
  //         12345678901234567890
  lcd.print("   Distance (um):   ");
  if (Reuse_Z_values == HIGH)
  {
    myencoder.write(Distance_hold * 4);
  }
  do
  {
    Distance = encode_return(10000, 1); // um could be a problem for longer distances!!
    Distance = Distance * Increment;
    //lcd.print("                    ");
    lcd.setCursor(9, 2);
    lcd.print(Distance);
    lcd.print("       ");
    Button_flag = digitalRead(Button_pin);
    if (Button_flag != Old_button_flag)
    {
      Old_button_flag = Button_flag;
      delay(30);
    }
  } while (Button_flag == HIGH);
  Distance_hold = Distance / Increment;
  myencoder.write(4);
  delay(200); // inserting this delay ends problems with 'bouncing by'
  //         12345678901234567890
  // do while loop here for delay to settle rail

  lcd.setCursor(0, 1);
  //         12345678901234567890
  lcd.print("Delay - settle (ms):");
  if (Reuse_Z_values == HIGH)
  {
    myencoder.write(Delay_1_hold * 4);
  }
  do
  {
    Delay_1 = encode_return(9999, 1);
    Delay_1 = Delay_1 * Increment;
    lcd.setCursor(9, 2);
    lcd.print(Delay_1);
    lcd.print("       ");
    Button_flag = digitalRead(Button_pin);
    if (Button_flag != Old_button_flag)
    {
      Old_button_flag = Button_flag;
      delay(30);
    }
  } while (Button_flag == HIGH);
  Delay_1_hold = Delay_1 / Increment;
  myencoder.write(4); // return to a value of 1 for encoder - divide by four!
  delay(200); // inserting this delay ends problems with 'bouncing by'

  //         12345678901234567890
  // do while loop here for delay for exposure i.e. after tripping shutter

  lcd.setCursor(0, 1);
  lcd.print("Delay - expose (ms):");
  if (Reuse_Z_values == HIGH)
  {
    myencoder.write(Delay_2_hold * 4);
  }
  do
  {
    Delay_2 = encode_return(9999, 1);
    Delay_2 = Delay_2 * Increment;
    lcd.setCursor(9, 2);
    lcd.print(Delay_2);
    lcd.print("       ");
    Button_flag = digitalRead(Button_pin);
    if (Button_flag != Old_button_flag)
    {
      Old_button_flag = Button_flag;
      delay(30);
    }
  } while (Button_flag == HIGH);
  Delay_2_hold = Delay_2 / Increment;
  myencoder.write(4); // return to a value of 1 for encoder - divide by four!
  lcd.clear();
  lcd.setCursor(0, 0);
  //         12345678901234567890
  lcd.print("Photos: ");
  lcd.print(Number_photos);
  lcd.setCursor(0, 1);
  lcd.print("Distance: ");
  lcd.print(Distance);
  lcd.setCursor(0, 2);
  lcd.print("Delay (settle): ");
  lcd.print(Delay_1);
  lcd.setCursor(0, 3);
  lcd.print("Delay (expose): ");
  lcd.print(Delay_2);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("--------------------");
  lcd.setCursor(0, 1);
  //         12345678901234567890
  lcd.print("   ACCEPT VALUES?   ");
  lcd.setCursor(0, 2);
  lcd.print("Press in 2 sec = YES");
  lcd.setCursor(0, 3);
  lcd.print("--------------------");
  Button_flag = HIGH;
  for (int i = 0; i <= 1000; i++) {
    Button_flag = digitalRead(Button_pin);
    delay(20);
    if (Button_flag == LOW)
    {
      break;
    }
  }
  if (Button_flag == LOW)
  {
    motor_driver();
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  //         01234567890123456789
  lcd.print("--------------------");
  lcd.setCursor(0, 1);
  lcd.print("  Finished Z stack! ");
  lcd.setCursor(0, 2);
  lcd.print("--------------------");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("--------------------");
  lcd.setCursor(0, 1);
  //         12345678901234567890
  lcd.print("REUSE STACK VALUES? ");
  lcd.setCursor(0, 2);
  lcd.print("Press in 2 sec = YES");
  lcd.setCursor(0, 3);
  lcd.print("--------------------");
  Reuse_Z_values = LOW;
  Button_flag = HIGH;
  for (int i = 0; i <= 200; i++) {
    Button_flag = digitalRead(Button_pin);
    delay(20);
    if (Button_flag == LOW)
    {
      Reuse_Z_values = HIGH;
      break;
    }
  }
  lcd.clear();
  return;
}

//
//
// SUBROUTINE motor_driver () moves the motor via EasyDriver and activates camera to produce a photo stack
//
//

int motor_driver()
{
  // This value is crucial for absolute accuracy and needs to be set by moving the rail and seeing how far it actually travels.
  long int steps_between_photos;
  float distance_between_photos;
  long int total_steps;
  int carry_direction;
  pinMode(Dir_pin, OUTPUT);
  pinMode(Drive_pin, OUTPUT);
  pinMode(Camera_pin, OUTPUT);
  pinMode(Pre_focus_pin, OUTPUT);
  pinMode(Flash_pin, OUTPUT);
  pinMode(Enable_pin, OUTPUT);
  digitalWrite(Dir_pin, LOW);
  digitalWrite(Drive_pin, LOW);
  digitalWrite(Enable_pin, LOW);
  distance_between_photos = float(Distance / (Number_photos - 1));
  steps_between_photos = int(distance_between_photos / microns_per_step); // check loss of precision!
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.setCursor(0, 0);
  //         12345678901234567890
  lcd.print("Caution motor active");
  lcd.setCursor(0, 1);
  lcd.print("Creating Z stack of:");
  lcd.setCursor(9, 2);
  lcd.print(Number_photos);
  lcd.setCursor(7, 3);
  lcd.print("Photos");
  delay(1500);
  //head off in the right direction!
  if (Direction == 2) {
    digitalWrite(Dir_pin, LOW);
  }
  if (Direction == 1) {
    digitalWrite(Dir_pin, HIGH);
  }
  //set Camera_pin high - first photo is where the rail is now
  //tell operator taking first photo
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Creating Z stack! ");
  lcd.setCursor(0, 1);
  lcd.print("Photo #: 1 ");
  lcd.setCursor(0, 2);
  lcd.print("In stack of:  ");
  lcd.print(Number_photos);
  digitalWrite(Pre_focus_pin, HIGH);
  digitalWrite(Camera_pin, HIGH);
  delay (Delay_shutter_lag);// wait for shutter to open plus a bit - 52ms for Nikon plus a copule of ms before flash
  digitalWrite(Flash_pin, HIGH);
  delay(10);// trigger flash
  digitalWrite(Camera_pin, LOW);
  digitalWrite(Pre_focus_pin, LOW);
  digitalWrite(Flash_pin, LOW);
  delay(Delay_2);
  for (int x_count_photos = 1; x_count_photos <= Number_photos - 1; x_count_photos++) {
    //move rail position by steps_between_photos
    //motor speed is set by delays....going slowly to avoid lost steps.....
    for (int y_count_steps = 1; y_count_steps <= steps_between_photos; y_count_steps++) {
      digitalWrite(Drive_pin, HIGH);
      delayMicroseconds(Rail_speed);
      digitalWrite(Drive_pin, LOW);
      delayMicroseconds(Rail_speed);
    }
    //Tell user which photo is being taken
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  Creating Z stack! ");
    lcd.setCursor(0, 1);
    //         12345678901234567890
    lcd.print("Photo #: ");
    lcd.print(x_count_photos + 1);
    lcd.setCursor(0, 2);
    lcd.print("In stack of: ");
    lcd.print(Number_photos);
    delay(Delay_1); // settle dealy to steady camera and rail
    // take the photo and set Flash_pin high with Camera_pin long enough to trigger camera
    digitalWrite(Pre_focus_pin, HIGH);
    digitalWrite(Camera_pin, HIGH);
    delay (Delay_shutter_lag);// delay for shutter to open - ~1/10th
    // Neeeds work here to set delay so flash occurs with correct timing - not relevant if camera triggers flashes
    digitalWrite(Flash_pin, HIGH);
    delay(10);// long enough pulse to trigger flash
    digitalWrite(Pre_focus_pin, LOW);
    digitalWrite(Camera_pin, LOW);
    digitalWrite(Flash_pin, LOW);
    delay(Delay_2); // allow for extended exposure times greater than 1 second provided by message delay
  }
  delay(1000);// so you can see the last value for photo number
  //
  //Rewind the slider to the starting point by reversing rail_direction and going back total number of steps in stack
  //
  //Warn that rewind about to take place..
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("--------------------");
  lcd.setCursor(0, 1);
  //         01234567890123456789
  lcd.print(" Caution rewinding! ");
  lcd.setCursor(0, 2);
  lcd.print("<<<<<<<<<<>>>>>>>>>>");
  lcd.setCursor(0, 3);
  lcd.print("--------------------");
  delay (1500);
  carry_direction = Direction;
  if (carry_direction == 2) {
    digitalWrite(Dir_pin, HIGH);
  }
  if (carry_direction == 1) {
    digitalWrite(Dir_pin, LOW);
  }
  //digitalWrite(Dir_pin, rail_direction);
  total_steps = (Number_photos - 1) * steps_between_photos;
  for (int x_count_rewind = 1; x_count_rewind <= total_steps; x_count_rewind ++) {
    digitalWrite(Drive_pin, LOW);
    delayMicroseconds(Rail_speed);
    digitalWrite(Drive_pin, HIGH);
    delayMicroseconds(Rail_speed);
  }

  digitalWrite(Enable_pin, HIGH);
  myencoder.write(4);
  lcd.clear();
}


//
/// SUBROUTINE jogger() jogs a motor via encoder
//

void jogger () {
  long int j = 0;
  int i = 0;
  int old_encoder_value;
  int step_size;
  int number_steps_jog;
  long int rail_position = 0; // to give movement relative to when jog mode is entered
  Old_button_flag = HIGH;
  Button_flag = HIGH;
  if (Motor_selected > 3)
  {
    lcd.clear();
    //         01234567890123456789
    lcd.setCursor(0, 0);
    lcd.print("--------------------");
    lcd.setCursor(0, 1);
    lcd.print("Use X, Y or Z motor ");
    lcd.setCursor(0, 2);
    lcd.print("Choose when offered!");
    lcd.setCursor(0, 3);
    lcd.print("--------------------");
    delay(1500);
    lcd.clear();
    select_motor();
  }
  pinMode(Dir_pin, OUTPUT);
  pinMode(Drive_pin, OUTPUT);
  pinMode(Enable_pin, OUTPUT);
  digitalWrite(Dir_pin, HIGH);
  digitalWrite(Drive_pin, LOW);
  digitalWrite(Enable_pin, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  //         01234567890123456789
  lcd.print(" ---JOG SUB-MENU--- ");
  lcd.setCursor(0, 1);
  lcd.print("Jog Increment (um): ");
  step_size = int(microns_per_step * Increment); // total distance in one jog  - scale step_size with value from Increment subroutine
  number_steps_jog = step_size / microns_per_step;
  lcd.setCursor(0, 2);
  lcd.print("         ");
  lcd.print(step_size);
  lcd.setCursor(0, 3);
  lcd.print("Position: ");
  lcd.print(rail_position);
  old_encoder_value = myencoder.read();
  do
  {
    j = myencoder.read();
    if (j > old_encoder_value + 4)
    {
      i = 1;
      do {// not at all clear why a for loop will not work...but it doesn't!
        digitalWrite(Dir_pin, HIGH);
        digitalWrite(Drive_pin, HIGH);
        delayMicroseconds(Rail_speed);
        digitalWrite(Drive_pin, LOW);
        delayMicroseconds(Rail_speed);
        i = i + 1;
      } while (i <= number_steps_jog);
      old_encoder_value = myencoder.read();
      rail_position = rail_position + step_size;
      lcd.setCursor(0, 3);
      lcd.print("Position: ");
      lcd.setCursor(10, 3);
      //         12345678901234567890
      lcd.print("        ");
      lcd.setCursor(10, 3);
      lcd.print(rail_position);
    }
    else if (j < old_encoder_value - 4)
    {
      i = 1;
      do {
        digitalWrite(Dir_pin, LOW);
        digitalWrite(Drive_pin, HIGH);
        delayMicroseconds(Rail_speed);;
        digitalWrite(Drive_pin, LOW);
        delayMicroseconds(Rail_speed);
        i = i + 1;
      } while (i <= number_steps_jog);
      old_encoder_value = myencoder.read();
      rail_position = rail_position - step_size;
      lcd.setCursor(0, 3);
      lcd.print("Position: ");
      lcd.setCursor(10, 3);
      lcd.print("        ");
      lcd.setCursor(10, 3);
      lcd.print(rail_position);
    }
    Button_flag = digitalRead(Button_pin);
    if (Old_button_flag != Button_flag)
    {
      Old_button_flag = Button_flag;
      delay(20);
    }
  } while (Button_flag == HIGH);
  digitalWrite(Enable_pin, HIGH);
  myencoder.write(0);
  lcd.clear();
  delay (500);
  return;
}

//
// SUBROUTINE rotate () rotates a rotary axis motor
//

void rotate () {
  //probably needs to check that a rotary axis motor has been selected in the 'select motor' subroutine
  //and then gets instructions re: clockwise or anticlockwise and the number of degrees (limits?)
  int turn_clock = 0;
  int turn_clock_old = 0;
  if (Motor_selected <= 3)
  {
    lcd.clear();
    //         01234567890123456789
    lcd.setCursor(0, 0);
    lcd.print("--------------------");
    lcd.setCursor(0, 1);
    lcd.print("  Use A or B motor  ");
    lcd.setCursor(0, 2);
    lcd.print("Choose when offered!");
    lcd.setCursor(0, 3);
    lcd.print("--------------------");
    delay(1500);
    lcd.clear();
    select_motor();
  }
  Old_button_flag = HIGH;
  Button_flag = HIGH;
  pinMode (Dir_pin, OUTPUT);
  pinMode(Drive_pin, OUTPUT);
  pinMode(Enable_pin, OUTPUT);
  digitalWrite(Enable_pin, LOW); // been problem when I defined enable pin as output?????
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("--ROTATE SUB-MENU---");
  //         12345678901234567890
  lcd.setCursor(0, 1);
  lcd.print("Jog step 0.22 degree");
  lcd.setCursor(0, 2);
  lcd.print("Degrees of rotation:");
  lcd.setCursor(0, 3);
  myencoder.write(0); // cahnged to zero -ok??
  do
  {
    turn_clock = myencoder.read();
    turn_clock = turn_clock / 4;
    if (turn_clock > turn_clock_old) {
      digitalWrite(Dir_pin, LOW);
      digitalWrite(Drive_pin , HIGH);
      delayMicroseconds(900);
      digitalWrite(Drive_pin, LOW);
      delayMicroseconds(900);
    }
    else if (turn_clock < turn_clock_old) {
      digitalWrite(Dir_pin, HIGH);
      digitalWrite(Drive_pin, HIGH);
      delayMicroseconds(900);
      digitalWrite(Drive_pin, LOW);
      delayMicroseconds(900);
    }
    turn_clock_old = turn_clock;
    lcd.setCursor(8, 3);
    lcd.print(float(turn_clock) * 0.225);
    lcd.print("    ");
    Button_flag = digitalRead(Button_pin);
    if (Old_button_flag != Button_flag)
    {
      delay(100);
      Button_flag = digitalRead(Button_pin);
      Old_button_flag = Button_flag;
    }
  } while (Button_flag == HIGH);
  myencoder.write(4);
  lcd.clear();
  digitalWrite(Enable_pin, HIGH);
}


//
// SUBROUTINE shoot () takes a picture without flash, just fires shutter on press of rotary encoder knob
//


void shoot () {
  // just fires shutter on press of rotary encoder knob
  int k;
  int j;
  byte Old_button_flag = HIGH;
  byte Button_flag = HIGH;
  lcd.clear();
  lcd.setCursor(0, 0);
  //         12345678901234567890
  lcd.print("---SHOOT SUB-MENU---");
  lcd.setCursor(0, 1);
  lcd.print("Click to take photo!");
  //         12345678901234567890
  // need to get clockwise or anticlockwise and degrees
  while (Button_flag == HIGH)
  {
    Button_flag = digitalRead(Button_pin);
    if (Old_button_flag != Button_flag)
    {
      delay(30);
      Button_flag = digitalRead(Button_pin);
      Old_button_flag = Button_flag;
    }
    if (Button_flag == LOW)
    {
      digitalWrite(Pre_focus_pin, HIGH);
      digitalWrite(Camera_pin, HIGH);
      delay(Delay_shutter_lag);// no need for lag here
      digitalWrite(Pre_focus_pin, LOW);
      digitalWrite(Camera_pin, LOW);
      delay(Delay_2);
      //         1234567890123456789
      lcd.setCursor(0, 2);
      lcd.print("    Photo taken!   ");
      delay(500);
      lcd.clear();
      return;
    }
  }
}

//
// SUBROUTINE flash () just fires the flashes on press of rotary encoder knob
//

void flash () {
  // just fires flashes on press of rotary encoder knob
  int k;
  int j;
  byte Old_button_flag = HIGH;
  byte Button_flag = HIGH;
  lcd.clear();
  lcd.setCursor(0, 0);
  //         12345678901234567890
  lcd.print("---FLASH SUB-MENU---");
  lcd.setCursor(0, 1);
  lcd.print("Click to fire flash ");
  //         12345678901234567890
  // need to get clockwise or anticlockwise and degrees
  while (Button_flag == HIGH)
  {
    Button_flag = digitalRead(Button_pin);
    if (Old_button_flag != Button_flag)
    {
      delay(30);
      Button_flag = digitalRead(Button_pin);
      Old_button_flag = Button_flag;
    }
    if (Button_flag == LOW)
    {
      digitalWrite(Flash_pin, HIGH);
      delay(10);
      digitalWrite(Flash_pin, LOW);
      delay(Delay_2);
      //         1234567890123456789
      lcd.setCursor(0, 2);
      lcd.print("    Flash fired!   ");
      delay(500);
      lcd.clear();
      return;
    }
  }
}


//
// SUBROUTINE shoot_flash () just takes a single picture with flash on press of rotary encoder knob
// this and the routines shoot() & shoot_flash() could all be choices within one subroutine??
//


void shoot_flash () {
  // just fires flashes on press of rotary encoder knob
  int k;
  int j;
  byte Old_button_flag = HIGH;
  byte Button_flag = HIGH;
  lcd.clear();
  lcd.setCursor(0, 0);
  //        12345678901234567890
  lcd.print("SHOOT&FLASH SUB-MENU");
  lcd.setCursor(0, 1);
  lcd.print("Click = flash photo ");
  //         12345678901234567890
  // need to get clockwise or anticlockwise and degrees
  while (Button_flag == HIGH)
  {
    Button_flag = digitalRead(Button_pin);
    if (Old_button_flag != Button_flag)
    {
      delay(30);
      Button_flag = digitalRead(Button_pin);
      Old_button_flag = Button_flag;
    }
    if (Button_flag == LOW)
    {
      digitalWrite(Pre_focus_pin, HIGH);
      digitalWrite(Camera_pin, HIGH);
      delay(Delay_shutter_lag);
      digitalWrite(Flash_pin, HIGH);
      delay(10);
      digitalWrite(Pre_focus_pin, LOW);
      digitalWrite(Camera_pin, LOW);
      digitalWrite(Flash_pin, LOW);
      delay(Delay_2);
      //         1234567890123456789
      lcd.setCursor(0, 2);
      lcd.print(" Flash photo taken!");
      delay(500);
      lcd.clear();
      return;
    }
  }
}

//
// SUBROUTINE select_motor() allows choice of which motor to control
//

int select_motor () {
  char motor = 'Z';
  Old_button_flag = HIGH;
  Button_flag = HIGH;
  myencoder.write(12);
  lcd.clear();
  lcd.setCursor(0, 0);
  //        12345678901234567890
  lcd.print("---MOTOR SUB-MENU---");
  //         12345678901234567890
  lcd.setCursor(0, 1);
  lcd.print(" A, B for rotations!");
  lcd.setCursor(0, 2);
  lcd.print("Current motor: ");
  lcd.print(motor);
  while (Button_flag == HIGH)
  {
    Motor_selected = encode_return(5, 1);
    switch (Motor_selected) {
      case 1:
        motor = 'X';
        break;
      case 2:
        motor = 'Y';
        break;
      case 3:
        motor = 'Z';
        break;
      case 4:
        motor = 'A';
        break;
      case 5:
        motor = 'B';
        break;
      default:
        ;
    }
    lcd.setCursor(0, 2);
    lcd.print("  Current motor: ");
    lcd.print(motor);
    Button_flag = digitalRead(Button_pin);
    if (Old_button_flag != Button_flag)
    {
      Old_button_flag = Button_flag;
      delay(70);
    }
  }
  switch (motor) { //set pins up for the motor selected
    case 'X':
      Enable_pin = 20;
      Dir_pin = 21;
      Drive_pin = 22;
      break;
    case 'Y':
      Enable_pin = 23;
      Dir_pin = 24;
      Drive_pin = 25;
      break;
    case 'Z': // reset these to 26, 27, 28 when rewiring done!
      Enable_pin = 10;
      Dir_pin = 11;
      Drive_pin = 12;
      break;
    case 'A':
      Enable_pin = 29;
      Dir_pin = 30;
      Drive_pin = 31;
      break;
    case 'B':
      Enable_pin = 32;
      Dir_pin = 33;
      Drive_pin = 34;
      break;
    default:
      ;
  }
  myencoder.write(4);
  //return; //Motor_selected......no need to return this variable it's global
}

//
// SUBROUTINE click_Increment() adjusts the 'sensitivity' of the rotary encoder making
// it easier to dial up big numbers
//

int click_Increment() {
  byte Old_button_flag = HIGH;
  byte Button_flag = HIGH;
  myencoder.write(40);
  lcd.clear();
  lcd.setCursor(0, 0);
  //         12345678901234567890
  lcd.print("-CLICK INC SUB-MENU-");
  lcd.setCursor(0, 1);
  lcd.print("  For each encoder  ");
  lcd.setCursor(0, 2);
  lcd.print(" click inc will be: ");
  //         12345678901234567890
  while (Button_flag == HIGH)
  {
    Increment = encode_return(50, 1);
    lcd.setCursor(9, 3);
    lcd.print(Increment);
    //         1234567890
    lcd.print("         ");
    Button_flag = digitalRead(Button_pin);
    if (Old_button_flag != Button_flag)
    {
      delay(100);
      Button_flag = digitalRead(Button_pin);
      Old_button_flag = Button_flag;
    }
    if (Button_flag == LOW)
    {
      lcd.clear();
      myencoder.write(4); // return encoder value to 1
      return Increment;
    }
  }
}
