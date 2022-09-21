//STEPPER BOX ver 2.4 Stack/stitch version - being ammended at 21/09/22
// Global variables to have capital letters - mostly!
//#define ENCODER_OPTIMIZE_INTERUPTS // Use if time overhead is crucial
#include <Encoder.h>
#include <LiquidCrystal_I2C.h>
Encoder myencoder(2, 3); // both 2 & 3 are interupt pins - gives highest fidelity
int Button_pin = 13;// pin that connects to encoder switch/button
int Upper_limit = 10; // initial max for encoder
int Lower_limit = 1; // initial min for encoder
int Increment = 10; // initial value for multilier re: durations, distance etc
int Direction; // intial direct rail will move 0 = forward
int Rail_speed = 400; // sets delay between driver pulses
long int Distance;
//
// This value is crucial for absolute accuracy and needs to be set by moving the rail and seeing how far it actually travels!
float microns_per_step = 5.0; // 5 um is the distance for an 8mm lead and 1600 steps per revolution
// change microns_per_step to match leadscrew!!!
//
int Number_photos; // photos in a stack
int Delay_1;// Time over which railmovement settles down
int Delay_2;// Exposure time during which rail halts
int Dir_pin = 11; // Set initial pins to those for Z axes
int Drive_pin = 12; // initial pin for Z stepper motion
int Enable_pin = 10; // initial pin to enable Z stepper motor
int Camera_pin = 15;// connected to camera remote shutter
int Pre_focus_pin = 16;// connected to camera remote prefocus
int Flash_pin = 17;// connects to flash socket - rename and initialise if using more than one flash - ie Flash_2_pin = 19
int Motor_selected = 3; //start with Z motor selected
int Number_Photos_hold; // remember last values used in a stack
int Direction_hold;//           "
int Distance_hold;//            "
int Delay_1_hold;//             "
int Delay_2_hold;//             "
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
String Option_9 = "  Z calc for stack  ";
String Option_10 = "  Stack and stitch  ";
String Explain_1 = "  (Create Z stack) ";
String Explain_2 = " (Jog back/forward)";
String Explain_3 = " (Control A,B Axes)";
String Explain_4 = " (Fire w/out flash)";
String Explain_5 = "    (Fire flash)   ";
String Explain_6 = "  (Fire and flash) ";
String Explain_7 = "   (Select motor)  ";
String Explain_8 = "(Click sensitivity)";
String Explain_9 = " (DoF (mm), steps) ";
String Explain_10 = " (Create Gigapixel)";
//
// Perhaps in a later version add an option here to allow leadscrews to be setup etc...
//
LiquidCrystal_I2C lcd(0x3F, 20, 4); // set address for LCD on I2C
void setup()
{
  // set all motor enable pins as outputs
  pinMode(10, OUTPUT);// Z enable pin
  pinMode(11, OUTPUT);// Z dir
  pinMode(12, OUTPUT);// Z drive
  pinMode(15, OUTPUT);// camera shutter drive pin - configure current flash and camera pins as outputs
  pinMode(16, OUTPUT);// camera prefocus drive pin
  pinMode(17, OUTPUT);// separate flash drive pin
  pinMode(18, OUTPUT);// not in use
  pinMode(20, OUTPUT);// not in use
  pinMode(23, OUTPUT);// not in use
  pinMode(29, OUTPUT);// X enable pin
  pinMode(30, OUTPUT);// X dir
  pinMode(31, OUTPUT);// X drive
  pinMode(32, OUTPUT);// A enable pin - just one rotary motor in this version
  pinMode(33, OUTPUT);// A dir
  pinMode(34, OUTPUT);// A drive
  pinMode(36, OUTPUT);// Y enable pin
  pinMode(37, OUTPUT);// Y dir
  pinMode(38, OUTPUT);// Y drive
  pinMode(48, OUTPUT);//set stepsize on Y
  pinMode(49, OUTPUT);//     ditto
  pinMode(50, OUTPUT);//set stepsize on X
  pinMode(51, OUTPUT);//     ditto
  pinMode(52, OUTPUT);// set stepsize on Z
  pinMode(53, OUTPUT);//     ditto
  // set all motor enable pins to off = HIGH
  digitalWrite(10, HIGH); //Z - off
  digitalWrite(20, HIGH);//not in use
  digitalWrite(23, HIGH);//not in use
  digitalWrite(29, HIGH); //X - off
  digitalWrite(32, HIGH);// A - off
  digitalWrite(36, HIGH);//Y - off
  digitalWrite(48, HIGH);//set 1/8th step on easydriver Y
  digitalWrite(49, HIGH);//    ditto
  digitalWrite(50, HIGH);//set 1/8th step on easydriver X
  digitalWrite(51, HIGH);//   ditto
  digitalWrite(52, HIGH);//set 1/8th step on easydriver Z
  digitalWrite(53, HIGH);//    ditto
  // others as one wishes
  pinMode (Button_pin, INPUT_PULLUP); // this is pin 13 used to detect a press of the encoder
  // sign on!
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  //         12345678901234567890
  lcd.print("  M A D B O F F I N ");
  lcd.setCursor(0, 1);
  lcd.print("       L A B S      ");
  lcd.setCursor(0, 2);
  lcd.print("Software version 2.6");
  lcd.setCursor(0, 3);
  lcd.print("   21st Sept 2022   ");
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
    case 9:
      lcd.setCursor(0, 1);
      lcd.print(Option_9);
      lcd.setCursor(0, 2);
      lcd.print(Explain_9);
      break;
    case 10:
      lcd.setCursor(0, 1);
      lcd.print(Option_10);
      lcd.setCursor(0, 2);
      lcd.print(Explain_10);
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
  else if (Button_flag == LOW && k == 9)
  {
    z_calculator();
  }
  else if (Button_flag == LOW && k == 10)
  {
    stack_stitch();
  }
}

//
// SUBROUTINE encode_return () is sent upper and lower limits and returns encoder_value
//

long encode_return (long Upper_limit, long Lower_limit) {
  long new_encoder_value = -999;
  long mult_enc;
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
  float delay_1_sec;
  float delay_2_sec;
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
  myencoder.write(4); // return to a value of 1 for encoder - divide by four - not sure it matters though!
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
  lcd.print("Delay - settle (s) :");
  if (Reuse_Z_values == HIGH)
  {
    myencoder.write(Delay_1_hold * 4);
  }
  do
  {
    Delay_1 = encode_return(9999, 1);
    Delay_1 = Delay_1 * 100; // decided not to use * by increment because short or incredibly long delays unlikely
    delay_1_sec = float(Delay_1) / 1000;
    lcd.setCursor(9, 2);
    lcd.print(delay_1_sec, 1);
    lcd.print("       ");
    Button_flag = digitalRead(Button_pin);
    if (Button_flag != Old_button_flag)
    {
      Old_button_flag = Button_flag;
      delay(30);
    }
  } while (Button_flag == HIGH);
  Delay_1_hold = Delay_1 / 100;
  myencoder.write(4); // return to a value of 1 for encoder - divide by four!
  delay(200); // inserting this delay ends problems with 'bouncing by'

  //         12345678901234567890
  // do while loop here for delay for exposure i.e. after tripping shutter

  lcd.setCursor(0, 1);
  lcd.print("Delay - expose (s) :");
  if (Reuse_Z_values == HIGH)
  {
    myencoder.write(Delay_2_hold * 4);
  }
  do
  {
    Delay_2 = encode_return(9999, 1);
    Delay_2 = Delay_2 * 100; // decided not to use * by increment because short or incredibly long delays unlikely
    delay_2_sec = float(Delay_2) / 1000;
    lcd.setCursor(9, 2);
    lcd.print(delay_2_sec, 2);
    lcd.print("       ");
    Button_flag = digitalRead(Button_pin);
    if (Button_flag != Old_button_flag)
    {
      Old_button_flag = Button_flag;
      delay(30);
    }
  } while (Button_flag == HIGH);
  Delay_2_hold = Delay_2 / 100;
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
  lcd.print(delay_1_sec, 2);
  lcd.setCursor(0, 3);
  lcd.print("Delay (expose): ");
  lcd.print(delay_2_sec, 2);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("--------------------");
  lcd.setCursor(0, 1);
  //         12345678901234567890
  lcd.print("   ACCEPT VALUES?   ");
  lcd.setCursor(0, 2);
  lcd.print("Press in 2 sec = YES"); //  maybe just  wait for a click?
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
  lcd.print("Press in 2 sec = YES"); // maybe just  wait for a click?
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
      delay(50);
    }
  }
  switch (motor) { //set pins up for the motor selected
    case 'X':
      Enable_pin = 29;
      Dir_pin = 30;
      Drive_pin = 31;
      break;
    case 'Y':
      Enable_pin = 36;
      Dir_pin = 37;
      Drive_pin = 38;
      break;
    case 'Z': // reset these to 26, 27, 28 when rewiring done!
      Enable_pin = 10;
      Dir_pin = 11;
      Drive_pin = 12;
      break;
    case 'A':
      Enable_pin = 32; // only allowing for one rotary motor!! So, A & B set to same pins for now
      Dir_pin = 33;
      Drive_pin = 34;
      break;
    case 'B':
      Enable_pin = 32; // only allowing for one rotary motor!!
      Dir_pin = 33;
      Drive_pin = 34;
      break;
    default:
      ;
  }
  myencoder.write(4);
  delay(500);
  lcd.clear();
  return; //Motor_selected......no need to return this variable it's global <---- working here!!!!
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
    Increment = encode_return(1000, 1);
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
//
// SUBROUTINE Calculate DoF and best Z interval
//

int z_calculator() {
  int z_depth;
  int select_CoC;
  float CoC;
  float focal_length;
  float aperture;
  float effective_aperture;
  float magnification;
  float sensor_size;
  float field_width;
  float f_number;
  float DoF;
  float far_DoF;
  float near_DoF;
  float n_steps;
  float hyperfocal_distance;
  float lens_to_object;
  // Ask for CoC - will replace with calculation of this from sensor diagonal or offer choice of 3!!
  byte Old_button_flag = HIGH;
  byte Button_flag = HIGH;
  lcd.clear();
  lcd.setCursor(0, 0);
  //         12345678901234567890
  lcd.print("CALC STACK SUB-MENU");
  lcd.setCursor(0, 1);
  lcd.print("   Select sensor:   ");
  //         12345678901234567890
   myencoder.write(4);
while (Button_flag == HIGH)
  {
    select_CoC = encode_return(3, 1);
    switch (select_CoC) {
      case 1:
      lcd.setCursor(0,2);
      //         12345678901234567890
      lcd.print("   CoC = m4/3rds    ");
      CoC = 0.015;
      sensor_size = 17.9;
      break;
      case 2:
      lcd.setCursor(0,2);
      lcd.print("       CoC = D     ");
      CoC = 0.02;
      sensor_size = 24;
      break;
      case 3:
      lcd.setCursor(0,2);
      lcd.print("      CoC = FF     ");
      CoC = 0.029;
      sensor_size = 36;
      break;
    }
    lcd.setCursor(7, 3);
    lcd.print(CoC, 3);
    //         1234567890
    //lcd.print("         ");
    Button_flag = digitalRead(Button_pin);
    if (Old_button_flag != Button_flag)
    {
      delay(100);
      Button_flag = digitalRead(Button_pin);
      Old_button_flag = Button_flag;
    }
  }
  myencoder.write(4); // return encoder value to 1
  Old_button_flag = HIGH;
  Button_flag = HIGH;
  // Get focal length
  lcd.clear();
  lcd.setCursor(0, 0);
  //         12345678901234567890
  lcd.print("CALC STACK SUB-MENU");
  lcd.setCursor(2, 1);
  lcd.print("Foc length in mm:");
  while (Button_flag == HIGH)
  {
    focal_length = float (encode_return(500, 10));
    lcd.setCursor(7, 2);
    lcd.print(focal_length);
    //         1234567890
    lcd.print("         ");
    Button_flag = digitalRead(Button_pin);
    if (Old_button_flag != Button_flag)
    {
      delay(100);
      Button_flag = digitalRead(Button_pin);
      Old_button_flag = Button_flag;
    }
  }
  myencoder.write(4); // return encoder value to 1
  Old_button_flag = HIGH;
  Button_flag = HIGH;
  // get nominal aperture
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CALC STACK SUB-MENU");
  lcd.setCursor(0, 1);
  lcd.print("   Input aperture   ");
  lcd.setCursor(0, 2);
  //         12345678901234567890
  lcd.print("     (f number):    ");
  while (Button_flag == HIGH)
  {
    aperture = float (encode_return(320, 12));
    aperture = aperture / 10;
    lcd.setCursor(8, 3);
    lcd.print(aperture);
    lcd.print("         ");
    Button_flag = digitalRead(Button_pin);
    if (Old_button_flag != Button_flag)
    {
      delay(100);
      Button_flag = digitalRead(Button_pin);
      Old_button_flag = Button_flag;
    }
  }
  myencoder.write(4); // return encoder value to 1
  Old_button_flag = HIGH;
  Button_flag = HIGH;
  // get the distance to the object
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CALC STACK SUB-MENU");
  lcd.setCursor(2, 1);
  //            12345678901234567890
  lcd.print("Obj distance(mm):");
  while (Button_flag == HIGH)
  {
    lens_to_object = float(encode_return(500, 1));
    lcd.setCursor(8, 2);
    lcd.print(lens_to_object);
    //         1234567890
    lcd.print("         ");
    Button_flag = digitalRead(Button_pin);
    if (Old_button_flag != Button_flag)
    {
      delay(100);
      Button_flag = digitalRead(Button_pin);
      Old_button_flag = Button_flag;
    }
  }
  myencoder.write(4); // return encoder value to 1
  Old_button_flag = HIGH;
  Button_flag = HIGH;
  //
  //
// get field width
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CALC STACK SUB-MENU");
  lcd.setCursor(0, 1);
  //         12345678901234567890
  lcd.print("  Field width (mm): ");
  while (Button_flag == HIGH)
  {
    field_width = float(encode_return(5000, 10));
    field_width = field_width/10.0;
    lcd.setCursor(8, 2);
    lcd.print(field_width,1);
    //         1234567890
    lcd.print("         ");
    Button_flag = digitalRead(Button_pin);
    if (Old_button_flag != Button_flag)
    {
      delay(100);
      Button_flag = digitalRead(Button_pin);
      Old_button_flag = Button_flag;
    }
  }
  myencoder.write(4); // return encoder value to 1
  Old_button_flag = HIGH;
  Button_flag = HIGH;
  // Calculate effective aperture
  magnification = field_width/sensor_size;
  effective_aperture = aperture * (1.0 + magnification);
  //
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CALC STACK SUB-MENU");
  lcd.setCursor(0,1);
  //         12345678901234567890
  lcd.print("Magnification:");
  lcd.setCursor(15,1);
  lcd.print(magnification,2);
  lcd.setCursor(0, 2);
  lcd.print("Effective f:");
  lcd.setCursor(15,2);
  lcd.print(effective_aperture,1);
  while (Button_flag == HIGH)
  {
    //do nothing -  wait for click
    Button_flag = digitalRead(Button_pin);
  }
  myencoder.write(4); // return encoder value to 1
  Old_button_flag = HIGH;
  Button_flag = HIGH;
  // get Z depth of object
  lcd.clear();
  lcd.setCursor(0, 0);
  //         12345678901234567890
  lcd.print("CALC STACK SUB-MENU");
  lcd.setCursor(0, 1);
  lcd.print("   Input Z depth   ");
  lcd.setCursor(0, 2);
  lcd.print("     in microns:   ");
  while (Button_flag == HIGH)
  {
    z_depth = encode_return(10000, 1); // working here!!!!!!!!!
    z_depth = z_depth * Increment;
    lcd.setCursor(8, 3);
    lcd.print(z_depth);
    //         1234567890
    lcd.print("         ");
    Button_flag = digitalRead(Button_pin);
    if (Old_button_flag != Button_flag)
    {
      delay(100);
      Button_flag = digitalRead(Button_pin);
      Old_button_flag = Button_flag;
    }
  }
  delay(1000);
  Old_button_flag = HIGH;
  Button_flag = HIGH;
  myencoder.write(0);
  //calculate hyperfocal distance
  hyperfocal_distance = sq(focal_length) / (aperture * CoC);
  //calculate near and far focus etc and from them DoF
  //near_DoF = (lens_to_object * hyperfocal_distance) / (hyperfocal_distance + (lens_to_object - focal_length));
  //far_DoF = (lens_to_object * hyperfocal_distance)/ (hyperfocal_distance - (lens_to_object- focal_length));
  //DoF = far_DoF - near_DoF;
  //alternative calculation from magnification and CoC
  DoF = ((2.0 * CoC * aperture) * (magnification + 1.0))/ sq(magnification);
  DoF = DoF * 0.75; // 0.75 is safety factor to give a reliable result for step size
  n_steps = float(z_depth / 1000) / DoF;
  lcd.clear();
  lcd.setCursor(0, 0);
  //         12345678901234567890
  lcd.print("CALC STACK SUB-MENU");
  lcd.setCursor(0, 1);
  lcd.print("  Click to return  ");
  lcd.setCursor(5, 2);
  lcd.print("DoF: ");
  lcd.print (DoF);
  lcd.print(" ");
  lcd.setCursor(5, 3);
  lcd.print("Steps: ");
  lcd.print(int(n_steps + 1)); // plus 1....gives correct answer because....truncation?
  while (Button_flag == HIGH)
  {
    //do nothing -  wait for click
    Button_flag = digitalRead(Button_pin);
  }
  lcd.clear();
  delay (500);
  return;
}
//
//
// Stack and stitch - automatically generate a stitched and stacked "Gigapixel" image
//
//
int stack_stitch() {
  int user_response;
  float field_width;
  float field_height;;
  float object_width;
  float object_height;
  float object_depth;
  float z_step;
  float n_steps;
  float x_shifts;
  float y_shifts;
  byte Old_button_flag = HIGH;
  byte Button_flag = HIGH;
  //
  // just in case set button to high = waiting for press
  Old_button_flag = HIGH;
  Button_flag = HIGH;

  // Check user is at lower right hand corner of object - bog off if not
  //
  //
  lcd.clear();
  lcd.setCursor(0, 0);
  //         12345678901234567890
  lcd.print("-STACK/STITCH MENU- ");
  lcd.setCursor(0, 1);
  lcd.print("Positioned at LRHC? ");
  //         1234567890
  while (Button_flag == HIGH)
  {
    user_response = encode_return(2, 1);
    switch (user_response) {
      case 1:
        lcd.setCursor(9, 2);
        lcd.print("Yes");
        break;
      case 2:
        lcd.setCursor(9, 2);
        lcd.print("No ");
        break;
    }
    Button_flag = digitalRead(Button_pin);
    delay(50);
    if (Old_button_flag != Button_flag)
    {
      Button_flag = digitalRead(Button_pin);
      Old_button_flag = Button_flag;
    }
  }
  switch (user_response) {
    case 1:
      lcd.clear();
      break;
    case 2:
      lcd.clear();
      return;
      break;
  }
  Old_button_flag = HIGH;
  Button_flag = HIGH;
  myencoder.write(0);
  delay (100);
  //
  //
  // What is the WIDTH of the field in a single image - user can get this from a ruler
  //
  lcd.clear();
  lcd.setCursor(0, 0);
  //         12345678901234567890
  lcd.print(" STACK/STITCH MENU  ");
  lcd.setCursor(0, 1);
  lcd.print("Field width using a ");
  lcd.setCursor(0, 2);
  lcd.print("    ruler (mm):     ");
  while (Button_flag == HIGH)
  {
    field_width = float(encode_return(200000, 100)); // allow for a field width of 1mm to 20cm
    field_width = (field_width / 1000.0) * Increment; // put it in mm
    lcd.setCursor(7, 3);
    lcd.print(field_width);
    //         1234567890
    lcd.print("        ");
    Button_flag = digitalRead(Button_pin);
    if (Old_button_flag != Button_flag)
    {
      delay(50);
      Button_flag = digitalRead(Button_pin);
      Old_button_flag = Button_flag;
    }
  }
  delay(100);
  Old_button_flag = HIGH;
  Button_flag = HIGH;
  myencoder.write(0);
  //
  //
  // Ditto for the height of a FIELD user can get this from a RULER
  //
  lcd.clear();
  lcd.setCursor(0, 0);
  //         12345678901234567890
  lcd.print(" STACK/STITCH MENU  ");
  lcd.setCursor(0, 1);
  lcd.print("Field height using  ");
  lcd.setCursor(0, 2);
  lcd.print("   a ruler (mm):    ");
  while (Button_flag == HIGH)
  {
    field_height = float(encode_return(200000, 100));// minimum 1mm - max 20cm
    field_height = (field_height / 1000.0) * Increment;
    lcd.setCursor(7, 3);
    lcd.print(field_height);
    //         1234567890
    lcd.print("        ");
    Button_flag = digitalRead(Button_pin);
    if (Old_button_flag != Button_flag)
    {
      delay(50);
      Button_flag = digitalRead(Button_pin);
      Old_button_flag = Button_flag;
    }
  }
  delay(100);
  Old_button_flag = HIGH;
  Button_flag = HIGH;
  myencoder.write(0);
  //
  //
  // What is the total WIDTH of the object to be photographed - user can get this from 'jog' with motor set to X
  //
  lcd.clear();
  lcd.setCursor(0, 0);
  //         12345678901234567890
  lcd.print(" STACK/STITCH MENU  ");
  lcd.setCursor(0, 1);
  lcd.print(" Object width (mm): ");
  while (Button_flag == HIGH)
  {
    object_width = float(encode_return(200000, 100));
    object_width = (object_width / 1000.0) * Increment;
    lcd.setCursor(7, 2);
    lcd.print(object_width);
    //         1234567890
    lcd.print("        ");
    Button_flag = digitalRead(Button_pin);
    if (Old_button_flag != Button_flag)
    {
      delay(50);
      Button_flag = digitalRead(Button_pin);
      Old_button_flag = Button_flag;
    }
  }
  delay(100);
  Old_button_flag = HIGH;
  Button_flag = HIGH;
  myencoder.write(0);
  //
  //
  // What is the total HEIGHT of the object to be photographed - user can get this from 'jog' with motor set to Y
  //
  lcd.clear();
  lcd.setCursor(0, 0);
  //         12345678901234567890
  lcd.print(" STACK/STITCH MENU  ");
  lcd.setCursor(0, 1);
  lcd.print("Object height (mm): ");
  while (Button_flag == HIGH)
  {
    object_height = float(encode_return(200000, 100));
    object_height = (object_height / 1000.0) * Increment;
    lcd.setCursor(7, 2);
    lcd.print(object_height);
    //         1234567890
    lcd.print("        ");
    Button_flag = digitalRead(Button_pin);
    if (Old_button_flag != Button_flag)
    {
      delay(100);
      Button_flag = digitalRead(Button_pin);
      Old_button_flag = Button_flag;
    }
  }
  delay(1000);
  Old_button_flag = HIGH;
  Button_flag = HIGH;
  myencoder.write(0);
  //
  //
  // What is the total DEPTH of the object to be photographed - user can get this from 'jog' with motor set to Z
  //
  lcd.clear();
  lcd.setCursor(0, 0);
  //         12345678901234567890
  lcd.print(" STACK/STITCH MENU  ");
  lcd.setCursor(0, 1);
  lcd.print(" Object depth (mm): ");
  while (Button_flag == HIGH)
  {
    object_depth = float(encode_return(200000, 10));
    object_depth = (object_depth / 1000.0) * Increment;
    lcd.setCursor(7, 2);
    lcd.print(object_depth);
    //         1234567890
    lcd.print("        ");
    Button_flag = digitalRead(Button_pin);
    if (Old_button_flag != Button_flag)
    {
      delay(100);
      Button_flag = digitalRead(Button_pin);
      Old_button_flag = Button_flag;
    }
  }
  delay(100);
  Old_button_flag = HIGH;
  Button_flag = HIGH;
  myencoder.write(0);
  //
  //
  // What is the Z STEP - user can get this from 'calc'
  //
  lcd.clear();
  lcd.setCursor(0, 0);
  //         12345678901234567890
  lcd.print(" STACK/STITCH MENU  ");
  lcd.setCursor(0, 1);
  lcd.print(" z step (um): ");
  while (Button_flag == HIGH)
  {
    z_step = float(encode_return(10000, 1)); // minimum 10 when * Increment because  minimum step with current 8mm lead of rail is 5um - 1.25um if swapped for 2mm lead
    z_step = z_step * Increment;
    lcd.setCursor(7, 2);
    lcd.print(z_step);
    //         1234567890
    lcd.print("        ");
    Button_flag = digitalRead(Button_pin);
    if (Old_button_flag != Button_flag)
    {
      delay(100);
      Button_flag = digitalRead(Button_pin);
      Old_button_flag = Button_flag;
    }
  }
  delay(100);
  Old_button_flag = HIGH;
  Button_flag = HIGH;
  myencoder.write(0);
  //
  //
  // Display the DATA from above and allow the user to COP OUT
  //
  lcd.clear();
  //lcd.setCursor(0, 0);
  //         12345678901234567890
  //lcd.print("  STACK/STITCH MENU ");
  lcd.setCursor(0, 0);
  lcd.print("FdW: ");
  lcd.print(field_width, 1);
  lcd.setCursor(10, 0);
  lcd.print("FdH: ");
  lcd.print(field_height, 1);
  lcd.setCursor(0, 1);
  lcd.print("ObW: ");
  lcd.print(object_width, 1);
  lcd.setCursor(10, 1);
  lcd.print("ObH: ");
  lcd.print(object_height, 1);
  lcd.setCursor(0, 2);
  lcd.print("ObZ: ");
  lcd.print(object_depth, 1);
  lcd.setCursor(10, 2);
  lcd.print("Stp: ");
  lcd.print(int(z_step));// int or otherwise it will overun edge of LCD for biggest number
  lcd.setCursor(0, 3);
  lcd.print("Begin? (Y/N): ");
  //         1234567890
  while (Button_flag == HIGH)
  {
    user_response = encode_return(2, 1);
    switch (user_response) {
      case 1:
        lcd.setCursor(14, 3);
        lcd.print("Y");
        break;
      case 2:
        lcd.setCursor(14, 3);
        lcd.print("N");
        break;
    }
    Button_flag = digitalRead(Button_pin);
    delay(50);
    if (Old_button_flag != Button_flag)
    {
      Button_flag = digitalRead(Button_pin);
      Old_button_flag = Button_flag;
    }
  }
  switch (user_response) {
    case 1: // DISPLAY the number of Z, X and Y movements needed to generate the image
      lcd.clear();
      lcd.setCursor (0, 0);
      //         12345678901234567890
      lcd.print("Cycle time fixed 10s");
      lcd.setCursor(0, 1);
      n_steps = object_depth / (float(z_step) / 1000.0);
      lcd.print("Z_steps: ");
      lcd.setCursor(11, 1);
      lcd.print(int(n_steps));
      lcd.setCursor(0, 2);
      x_shifts = object_width / field_width;
      x_shifts = ((x_shifts * 2.0) - 2.0) + 1.5; //NO -> 50% overlap and minus 2 because already at X1Y1 +0.5 TO ROUND UP on int conversion - it's add 1.5 to get frame taken at starting point
      lcd.print("X_shifts: ");
      lcd.setCursor(11, 2);
      lcd.print(int(x_shifts));
      lcd.setCursor(0, 3);
      y_shifts = object_height / field_height;
      y_shifts = ((y_shifts * 2.0) - 2.0) + 1.5; // 50% overlap and minus 2 because already at X1Y1 + 0.5 TO ROUND UP on int conversion - it's add 1.5 as above
      lcd.print("Y_shifts: ");
      lcd.setCursor(11, 3);
      lcd.print(int(y_shifts));
      delay(5000);
      lcd.clear(); // leave screen clear for return to menu
      break;
    case 2: //COP OUT!
      lcd.clear();
      return;
      break;
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("----Giga start----");
  delay(2000);
  giga_z_stack(n_steps, x_shifts, y_shifts, object_depth, field_width, field_height);
  delay(1000);
  return;
}

//
//
// SUB-ROUTINE giga_z_stack - makes a gigapixel image by stacking on Z axis and shifting X & Y axes currently with some fixed values
//
int giga_z_stack(int n_steps, int x_shifts, int y_shifts, int object_depth, int field_width, int field_height)
{
  long int number_steps_X_move;
  long int number_steps_Y_move;
  long int x_steps;
  long int y_steps;
  long int count_x_steps_rewind;
  long int count_y_steps_rewind;
  // delays need to be set to reasonable values for flashes to charge and camera to steady
  // will make these values user inputs in  later version
  Delay_1 = 1000;// Time over which railmovement settles down // debug set short
  Delay_2 = 100;// Exposure time during which rail halts // debug set short
  // set direction to forwards only
  Number_photos = n_steps;
  Distance = object_depth * 1000;
  x_steps = (field_width * 1000) / microns_per_step;
  x_steps = x_steps / 2 ;// for 50% overlap
  y_steps = (field_height * 1000) / microns_per_step;
  y_steps = y_steps / 2 ;// for 50% overlap
  for (int y_shift_count = 1; y_shift_count <= y_shifts; y_shift_count++) {
    for (int x_shift_count = 1; x_shift_count <= x_shifts; x_shift_count++) {
      // make z stack at current x
      Dir_pin = 11;// set pins for Z motor, they are not set by motor_driver()
      Drive_pin = 12;//
      Enable_pin = 10;//
      lcd.clear();
      //         01234567890123456789
      lcd.setCursor(0, 0);
      lcd.print("      Gigastack    ");
      lcd.setCursor(0, 1);
      lcd.print("Y field #: ");
      lcd.print(y_shift_count);
      lcd.setCursor(0, 2);
      lcd.print("X field #: ");
      lcd.print(x_shift_count);
      delay(2500);
      Direction = 2; // this will set Dir_pin to low within motor_driver()
      motor_driver();// do z stack
      Dir_pin = 30; // set pins for X
      Drive_pin = 31;//
      Enable_pin = 29;//
      digitalWrite(Enable_pin, LOW);// X motor ENabled
      digitalWrite(Dir_pin, HIGH); // set direction on X forward
      for ( long int x_step_count = 1; x_step_count <= x_steps; x_step_count++) { // make the x shift
        digitalWrite(Drive_pin, HIGH);
        delayMicroseconds(Rail_speed);
        digitalWrite(Drive_pin, LOW);
        delayMicroseconds(Rail_speed);
        count_x_steps_rewind = count_x_steps_rewind + 1; // count steps until Y shift then reset to 0
      }
      digitalWrite(Enable_pin, HIGH);// X motor DISabled
    }
    // rewind X
    //
    // debug
    lcd.clear();
    lcd.setCursor(0, 2);
    //         12345678901234567890
    lcd.print(" <<<<Rewind on X>>>>");
    delay(1500);
    lcd.clear();
    //debug
    digitalWrite(Enable_pin, LOW);// X motor ENabled
    digitalWrite(Dir_pin, LOW); // reverse direction for rewind
    for (long int rewind_x = 1; rewind_x <= count_x_steps_rewind; rewind_x ++) { // back to the start of x series
      digitalWrite(Drive_pin, HIGH);
      delayMicroseconds(Rail_speed);
      digitalWrite(Drive_pin, LOW);
      delayMicroseconds(Rail_speed);
    }
    // debug
    lcd.clear();
    lcd.setCursor(0, 2);
    //         12345678901234567890
    lcd.print("    Rewind X done!  ");
    delay(1500);
    lcd.clear();
    //debug
    digitalWrite(Enable_pin, HIGH); //X motor DISabled
    count_x_steps_rewind = 0;// reset steps counter for rewind to zero
    Dir_pin = 37;// set pins for y
    Drive_pin = 38;//
    Enable_pin = 36;//
    digitalWrite(Enable_pin, LOW);// Y motor ENabled
    digitalWrite(Dir_pin, HIGH); // set direction on Y forward
    for ( long int y_step_count = 1; y_step_count <= y_steps; y_step_count++) { // make the y shift
      digitalWrite(Drive_pin, HIGH);
      delayMicroseconds(Rail_speed);
      digitalWrite(Drive_pin, LOW);
      delayMicroseconds(Rail_speed);
      count_y_steps_rewind = count_y_steps_rewind + 1; // count how far Y has moved to use on rewind
    }
    digitalWrite(Enable_pin, HIGH);// Y motor disabled
  }
  // rewind Y this will park the Y drive at the TRHC
  //
  Dir_pin = 37; // set pins for Y
  Drive_pin = 38;
  Enable_pin = 36;
  digitalWrite(Enable_pin, LOW);//Y motor ENabled
  // debug
  lcd.clear();
  lcd.setCursor(0, 2);
  //         12345678901234567890
  lcd.print(" <<<<Rewind on Y>>>>");
  delay(1500);
  lcd.clear();
  //debug
  digitalWrite(Dir_pin, LOW); // reverse direction for rewind
  for (long int rewind_y = 1; rewind_y <= count_y_steps_rewind; rewind_y ++) { // back to the start of x series
    digitalWrite(Drive_pin, HIGH);
    delayMicroseconds(Rail_speed);
    digitalWrite(Drive_pin, LOW);
    delayMicroseconds(Rail_speed);
  }
  // debug
  lcd.clear();
  lcd.setCursor(0, 2);
  //         12345678901234567890
  lcd.print("   Rewind Y done!  ");
  delay(1500);
  lcd.clear();
  //debug
  count_y_steps_rewind = 0;
  digitalWrite(Enable_pin, HIGH); // Y motor off
  // gigastack is now finsished
  lcd.clear();
  lcd.setCursor(0, 0);
  //         01234567890123456789
  lcd.print("    Giga stack    ");
  lcd.setCursor(0, 1);
  lcd.print("     Finished!    ");
  delay(2000);
  lcd.clear();
  // set rail back to Z axis default
  Dir_pin = 11;// set pins for Z
  Drive_pin = 12;
  Enable_pin = 10;
  digitalWrite(Enable_pin, HIGH);
  return;
}
