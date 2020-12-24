#include <LiquidCrystal.h>
#include <Keypad.h>
#include <DS3231.h>
#include <EEPROM.h>
#include<Math.h>
#include "RTClib.h"
int buzz=50;
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
DS3231  rtc(SDA, SCL);
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
String pin = "";
String c_info_pin = "";
String code = "";
String c_info_code = "";
String id = "";
String admin_pin = "1234";
String student_pin = "";
String submitted_id = "";
String submitted_pin = "";
int classRoom_address = 0, t2;
int deleted_registered_student_address = 0;
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'7', '8', '9', 'A'},
  {'4', '5', '6', 'B'},
  {'1', '2', '3', 'C'},
  {'S', '0', '=', 'D'}
};
byte rowPins[ROWS] = {22, 23, 24, 25}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {26, 27, 28, 29}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad cusKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
Time t;
int Screen = 2;
String s;
void setup() {
  pinMode(buzz,OUTPUT);
  pinMode(35,OUTPUT);
  pinMode(51,OUTPUT);
  
  
  rtc.begin(); // Initialize the rtc object
  lcd.begin(20, 4);
  lcd.clear();
  //The following lines can be uncommented to set the date and time
  rtc.setDOW(FRIDAY);     // Set Day-of-Week to SUNDAY
  rtc.setTime(20, 37, 0);     // Set the time to 12:00:00 (24hr format)
  rtc.setDate(04, 9, 2020);   // Set the date to January 1st, 2014
  t2 = t.year - 2000;
  Serial.begin(9600);
  //s = rtc.getMonthStr();

  //  Serial.print(t2);
  //  Serial.print(t.year, DEC);

  //Initialize_EEPROM();

  //store_multiple_student_info();
  read_student_info();
  read_classroom_info();
  //  insert_Classroom_info(5);
  //  insert_Classroom_info(18);
  //  insert_Classroom_info(31);
  //  insert_Classroom_info(44);
  //  insert_Classroom_info(57);
  //code_kam_kore_na();
}

void loop() {
  t = rtc.getTime();
  char key = cusKeypad.getKey();
  if (Screen == 2) {                //__________________Screen 2____________________
    lcd.clear();
    ShowDateTime();
    Display(2, 2, "Press A for menu");
    while(key != 'A'){
      key = cusKeypad.getKey();
      Screen = 3;
    }
  }
  else if (Screen == 3)           //__________________Screen 3____________________
  {
    //Screen = 3;
    lcd.clear();
    Display(1, 0, "A: Start Attendance");
    Display(1, 1, "B: Register");
    Display(1, 2, "C: Delete");
    Display(1, 3, "D: Home");
    Display(9, 3, "E:C_INFO");
    while (true) {
      key = cusKeypad.getKey();
      if (key == 'A') {
        Screen = 5;
        Serial.println("A");
        break;
      } else if (key == 'B') {
        Screen = 11;
        Serial.println("B");
        break;
      } else if (key == 'C') {
        Screen = 12;
        Serial.println("C");
        break;
      } else if (key == 'D') {
        Screen = 2;
        Serial.println("D");
        lcd.clear();
        break;
      }
      else if (key == '=') {
        Screen = 15;
        Serial.println("E");
        break;
      }
    }

  }
  else if (Screen == 5) {             //__________________Screen 5____________________
    lcd.clear();
    Display(0, 0, "Enter ClassCode:");
    Display(0, 1, "0:3211");
    Display(0, 2, "1:3213");
    Display(7, 1, "2:3215");
    Display(7, 2, "3:3216");
    Display(14, 1, "4:3223");
    Display(0, 4, "A:OK");
    Display(5, 4, "B:Clear C:Back");

    while (true) {
      key = cusKeypad.getKey();
      if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4') {
        code = (char)key;
        Display(17, 0, code);
      } else if (key == 'A' ) {
        if (code != "") {
          Screen = 6;
          break;
        }

      }

      else if (key == 'B') {
        code = " ";
        //Screen=5;
        Display(17, 0, code);


      } else if (key == 'C') {
        Screen = 3;
        break;
      }
    }
  }
  else if (Screen == 11) {          //__________________Screen 11____________________
    lcd.clear();
    Display(1, 0, "Register ID:");
    Display(1, 3, "A:OK B:Clear C:BACK");
    id = "";
    while (true) {
      key = cusKeypad.getKey();
      if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7'
          || key == '8' || key == '9') {
        id += (char)key;
        Display(14, 0, id);
      }
      else if (key == 'A') {
        if (check_duplicate_register_id(id.toInt())) {
          lcd.clear();
          Display(4, 1, "DUPLICATE ID");
          Display(7, 2, "FOUND");
          tone(buzz,1000);
          digitalWrite(35,HIGH);
          delay(1000);
          digitalWrite(35,LOW);
          noTone(buzz);
          
          break;
        } else {
          digitalWrite(51,HIGH);
          delay(1000);
          digitalWrite(51,LOW);
          Screen = 14;
          break;
        }

      }
      else if (key == 'B') {
        Screen = 11;
        break;
      } else if (key == 'C') {
        Screen = 3;
        break;
      }

    }
  }
  else if (Screen == 14) {            //__________________Screen 14____________________
    lcd.clear();
    Display(2, 0, "Enter PIN:");
    Display(1, 3, "A:OK B:Clear C:BACK");
    student_pin = "";
    while (true) {
      key = cusKeypad.getKey();

      if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7'
          || key == '8' || key == '9') {
        student_pin += (char)key;
        for (int i = 0; i < student_pin.length(); i++) {
          Display(12 + i, 0, "*");
        }

      }
      else if (key == 'A' &&  student_pin!="" ) {
        store_student_info(EEPROM.read(1001) * 5 + EEPROM.read(1003));
        Screen = 2;
        lcd.clear();
        Display(4, 1, "Succesfully");
        Display(5, 2, "Registered");
        digitalWrite(51,HIGH);
        delay(2000);
        digitalWrite(51,LOW);
        break;
      }
      else if (key == 'B') {
        Screen = 14;
        break;
      }
      else if (key == 'C') {
        Screen = 3;
        break;
      }

    }

  }
  else if (Screen == 12) {      //__________________Screen 12____________________
    lcd.clear();
    Display(1, 0, "ID: ");
    Display(1, 1, "A:SUBMIT B:CLEAR");
    Display(1, 2, "C:BACK");
    String submitted_delete_id = "";
    while (true) {
      key = cusKeypad.getKey();
      if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7'
          || key == '8' || key == '9') {

        submitted_delete_id += (char)key;
        Display(5, 0, submitted_delete_id);
      }
      else if (key == 'A') { //SUBMIT
        int starting_student_info_address = EEPROM.read(1001);
        bool isFound_ID = false;
        for (int i = 210; i < 210 + starting_student_info_address * 5; i += 5) {
          if (EEPROM.read(i) == submitted_delete_id.toInt()) {
            deleted_registered_student_address = i;
            isFound_ID  = true;
            break;
          }
        }
        if (isFound_ID) {
          Screen = 13;
          submitted_delete_id = "";
          break;
        } else {
          lcd.clear();
          Display(1, 1, "NOT FOUND");
          digitalWrite(35,HIGH);
          delay(2000);
          digitalWrite(35,LOW);
          break;
        }
      } else if (key == 'B') { //CLEAR
        submitted_delete_id = "    ";
        Display(5, 0, submitted_delete_id);
        submitted_delete_id = "";

      } else if (key == 'C') { //BACK
        Screen = 3;
        break;
      }
    }
  } else if (Screen == 13) {     //__________________Screen 13____________________
    lcd.clear();
    Display(1, 0, "ADMIN PIN: ");
    Display(1, 1, "A:SUBMIT B:CLEAR");
    Display(1, 2, "C:BACK");
    String submitted_admin_pin = "";
    while (true) {
      key = cusKeypad.getKey();

      if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7'
          || key == '8' || key == '9') {

        submitted_admin_pin += (char)key;
        Display(12, 0, submitted_admin_pin);
      }
      else if (key == 'A') { //SUBMIT
        if (submitted_admin_pin == admin_pin) {
          //id ta delete krte hbe j address a store kora ache,then porer sob gula info 5 ghor kore agay nite hbe, number of student
          //ek komate hbe[1001]
          delete_registered_student(deleted_registered_student_address);
          lcd.clear();
          Display(1, 1, "STUDENT DELETED");
          digitalWrite(51,HIGH);
          delay(2000);
          digitalWrite(51,LOW);
          Screen = 3;
        } else {
          lcd.clear();
          Display(1, 1, "WRONG ADMIN PIN");
          digitalWrite(35,HIGH);
          tone(buzz,1000);
          delay(2000);
          noTone(buzz);
          digitalWrite(35,LOW);
          Screen = 12;
        }

        break;

      } else if (key == 'B') { //CLEAR
        submitted_admin_pin = "    ";
        Display(12, 0, submitted_admin_pin);
        submitted_admin_pin = "";

      } else if (key == 'C') { //BACK
        Screen = 3;
        break;
      }
    }

  }  else if (Screen == 6) {     //__________________Screen 6____________________
    lcd.clear();
    Display(1, 0, "Enter PIN: ");
    Display(1, 1, "A: Start");
    Display(1, 2, "B: Cancel");

    while (true) {
      key = cusKeypad.getKey();
      if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7'
          || key == '8' || key == '9') {
        pin += (char)key;
        Display(11, 0, pin);
      } else if (key == 'A' && pin != "") {
        classRoom_address = code.toInt() * 13 + 1;
        insert_classroom_pin(classRoom_address, pin);
        insert_Classroom_info(classRoom_address + 4);
        Screen = 7;
        lcd.clear();
        Serial.print("A");
        break;
      }
      else if (key == 'B') {
        pin = "";
        Screen = 5;
        Serial.print("B");
        break;
      }
    }
  } else if (Screen == 7) {     //__________________Screen 7____________________
    lcd.clear();
    ShowDateTime();
    Display(2, 2, "Class in progress");
    Display(2, 3, "A: Join B: Finish");
    while (true) {
      key = cusKeypad.getKey();
      if (key == 'A') {
        Screen = 8;
        break;
      } else if (key == 'B') {
        Screen = 9;
        break;
      }
    }
    lcd.clear();
  } else if (Screen == 8) {       //__________________Screen 8____________________
    submitted_id = "";
    submitted_pin = "";
    Display(1, 0, "ID: ");
    Display(1, 1, "PIN: ");
    Display(1, 2, "A: NEXT B: CLEAR");
    Display(1, 3, "C: SUBMIT D: BACK" );
    lcd.setCursor(6, 0);
    bool isMatched = false;
    while (true) {
      key = cusKeypad.getKey();
      if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7'
          || key == '8' || key == '9') {
        submitted_id += (char)key;
        Display(6, 0, submitted_id);
      }
      else if (key == 'A') {
        lcd.setCursor(6, 1);
        //lcd.blink();

        while (key != 'C') {
          key = cusKeypad.getKey();
          if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7'
              || key == '8' || key == '9') {
            submitted_pin += (char)key;
            Display(6, 1, submitted_pin);
          }
          else if (key == 'B') {
            Display(6, 1, "    ");
            submitted_pin = "";
          } else if (key == 'D') {
            Screen = 7;
            break;
          }

        }

        for (int i = 210; i < 500; i += 5) {
          if (EEPROM.read(i) == submitted_id.toInt()) {
            String temp_pin = "";
            for (int j = i + 1; j < i + 5; j++) {
              temp_pin += EEPROM.read(j);
            }
            if (temp_pin == submitted_pin) {
              Serial.println("ID & PIN Matched");
              isMatched = true;
              break;
            }
          }
        }
        if (isMatched) {

          if (store_student_attendance(classRoom_address, submitted_id)) {
            lcd.clear();
            Display(1, 0, "Attendance Given of");
            Display(4, 1, "ID:");
            Display(7, 1, submitted_id);
            Screen = 7;
            delay(1000);
            break;
          } else {
            lcd.clear();
            Display(8, 1, "ERROR");
            Display(1, 2, "Already Attendent");
            Display(4, 3, "in the Class");
            delay(1000);
            Screen = 7;
            break;
          }
        } else {
          lcd.clear();
          Display(8, 1, "ERROR");
          Display(0, 1, "ID or PIN Mismatched");
          submitted_id = "";
          submitted_pin = "";
          delay(1000);
          Screen = 8;
          lcd.clear();
          break;

        }
      }
      else if (key == 'B') {
        submitted_id = "";
        Display(6, 0, "    ");
      }
      else if (key == 'D') {
        Screen = 7;
        break;
      }

    }


  } else if (Screen == 9) {               //__________________Screen 9____________________
    lcd.clear();
    Display(0, 1, "Enter PIN: ");
    Display(1, 2, "A: OK");
    Display(1, 3, "B: Back");
    String str = "";
    String pass = "";
    while (true) {
      key = cusKeypad.getKey();
      if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7'
          || key == '8' || key == '9') {
        pass += (char)key;
        Display(10, 1, pass);
      } else if (key == 'A') {
        lcd.clear();
        bool isMatched = false;
        Serial.println(get_classroom_pin(code.toInt()));
        if (get_classroom_pin(code.toInt()) == pass.toInt()) {
          isMatched = true;
        }
        if (isMatched) {
          while (true) {
            Display(1, 0, "Class has finished");
            Display(1, 1, "A: Home");
            key = cusKeypad.getKey();
            if (key == 'A') {
              Screen = 2;
              break;
            }
          }
          break;
        } else {
          while (true) {
            Display(1, 0, "Classcode or PIN");
            Display(1, 1, "was not matched");
            Display(1, 2, "A: OK");
            key = cusKeypad.getKey();
            if (key == 'A') {
              Screen = 7;
              break;
            }
          }
        }
        lcd.clear();
        break;
      } else if (key == 'B') {
        Screen = 7;
        break;
      }
    }

  }
  else if (Screen == 15) {             //__________________Screen 15____________________
    lcd.clear();
    Display(0, 0, "Enter ClassCode:");
    Display(0, 1, "0:3211");
    Display(0, 2, "1:3213");
    Display(7, 1, "2:3215");
    Display(7, 2, "3:3216");
    Display(14, 1, "4:3223");
    Display(0, 4, "A:OK");
    Display(5, 4, "B:Clear C:Back");
    //key = '';
    while (true) {
      key = cusKeypad.getKey();
      if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4') {
        c_info_code = (char)key;
        Display(17, 0, c_info_code);
      } else if (key == 'A' ) {

        if (c_info_code != "") {
          Screen = 16;
          break;
        }


      }

      else if (key == 'B') {
        c_info_code = " ";
        Screen = 15;
        Display(17, 0, c_info_code );


      } else if (key == 'C') {
        Screen = 3;
        break;
      }
    }
  }
  else if (Screen == 16) {     //__________________Screen 16____________________
    lcd.clear();
    Display(1, 0, "Enter PIN: ");
    Display(1, 1, "A: SUBMIT");
    Display(1, 2, "B: CLEAR");
    Display(10, 2, "C: BACK");
    while (true) {

      key = cusKeypad.getKey();
      if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7'
          || key == '8' || key == '9') {
        c_info_pin += (char)key;
        Display(11, 0, c_info_pin);
      } else if (key == 'A' &&  c_info_pin != "") {
        if (get_classroom_pin(c_info_code.toInt()) == c_info_pin.toInt()) {
          Screen = 17;
          c_info_pin = "";
          lcd.clear();
          break;
        }
        else {
          lcd.clear();
          Display(1, 2, "WRONG PIN");
          delay(2000);
          Screen = 16;
          break;

        }

      }
      else if (key == 'B') {
        Screen = 15;
        c_info_pin = "";
        break;
      }
      else if (key == 'C') {
        Screen = 3;
        break;
      }
    }
  }
  else if (Screen == 17) {        //__________________Screen 17____________________
    lcd.clear();
    Display( 0, 0, "Class Info Settings");
    Display(1, 1, "A:SHOW ID");
    Display(1, 2, "B:DELETE RECORD");
    Display(1, 3, "C: BACK");
    while (true) {
      key = cusKeypad.getKey();
      if (key == 'A') {   //show id
        Screen = 18;
        break;
      }
      else if (key == 'B') {    //delete record
        delete_student_attendance_details(c_info_code.toInt());
        lcd.clear();
        Display(1, 1, "Record Deleted");
        delay(2000);
        break;
      }
      else if (key == 'C') {    //back
        Screen = 3;
        break;
      }
    }


  }
  else if (Screen == 18)      //__________________Screen 18____________________
  {
    lcd.clear();
    read_student_attendance_details(c_info_code.toInt());
    Display(0, 3, "A:Next B:Prev C:Back");
    while (true) {
      key = cusKeypad.getKey();
      if (key == 'A') { //next

      } else if (key == 'B') { //prev

      } else if (key == 'C') { //back
        Screen = 17;
        break;
      }
    }
  }

}

void ShowDateTime()
{
  lcd.setCursor(2, 0);
  lcd.print("Time:  ");
  lcd.print(rtc.getTimeStr());

  lcd.setCursor(2, 1);
  lcd.print("Date: ");
  lcd.print(rtc.getDateStr());
}
void Display(int col, int row, String msg)
{
  lcd.setCursor(col, row);
  lcd.print(msg);
}

void Initialize_EEPROM()
{
  //System Info
  //EEPROM.write(1000,5);     //Number of Classroom
  EEPROM.write(1001, 0);    //Number of registered Student
  EEPROM.write(1002, 0);    //Classroom Code starting address
  EEPROM.write(1003, 210);  //Student info Starting address


  EEPROM.write(0, 11);    // 3211
  EEPROM.write(13, 13);    // 3213
  EEPROM.write(26, 15);    // 3215
  EEPROM.write(39, 16);    // 3216
  EEPROM.write(52, 23);    // 3223

  for (int i = 1000; i < 1004; i++) {
    Serial.println(EEPROM.read(i));
  }
}
void   insert_classroom_pin(int address, String pin) {
  Serial.print("32");
  Serial.println(EEPROM.read(address - 1));
  for (int i = address, j = 0; i < address + 4; i++, j++) {
    EEPROM.write(i, (pin[j] - 48));
    Serial.print(i);
    Serial.print("->");
    Serial.println(EEPROM.read(i));
  }

}
void insert_Classroom_info(int info_address)      //info address should be 0/13/26/39/52
{

  String s = rtc.getMonthStr();
  int monthDecimal = 0;
  if (s == "January")
    monthDecimal = 1;
  else if (s == "February")
    monthDecimal = 2;
  else if (s == "March")
    monthDecimal = 3;
  else if (s == "April")
    monthDecimal = 4;
  if (s == "May")
    monthDecimal = 5;
  else if (s == "June")
    monthDecimal = 6;
  else if (s == "July")
    monthDecimal = 7;
  else if (s == "August")
    monthDecimal = 8;
  if (s == "September")
    monthDecimal = 9;
  else if (s == "October")
    monthDecimal = 10;
  else if (s == "November")
    monthDecimal = 11;
  else if (s == "December")
    monthDecimal = 12;
  t2 = t.year - 2000;
  EEPROM.write(info_address, (t.date));
  EEPROM.write(info_address + 1, monthDecimal);
  //Serial.print(EEPROM.read(info_address+1));
  EEPROM.write(info_address + 2, t2);
  EEPROM.write(info_address + 3, (t.hour));
  EEPROM.write(info_address + 4, (t.min));
  EEPROM.write(info_address + 5, 1);
  EEPROM.write(info_address + 6, 0);
  //     EEPROM.write(,0);



}
bool check_duplicate_register_id(int id) {
  for (int i = 210; i < 210 + EEPROM.read(1001) * 5; i += 5) {
    if (EEPROM.read(i) == id) {
      return true;
    }
  }
  return false;
}
void store_student_info(int student_info_storing_address) {
  Serial.print("Student_Storing Address: ");
  Serial.println(student_info_storing_address);
  EEPROM.write(student_info_storing_address, id.toInt());
  Serial.print("Stored id : ");
  Serial.println(EEPROM.read(student_info_storing_address));
  for (int i = student_info_storing_address + 1, j = 0; i <= student_info_storing_address + 4; i++, j++) {
    EEPROM.write(i, (student_pin[j] - 48));
    Serial.print(i);
    Serial.print("->");
    Serial.println(EEPROM.read(i));
  }
  student_pin = "";
  id = "";
  EEPROM.write(1001, EEPROM.read(1001) + 1);
}

void read_student_info()
{
  int count = 1;
  int starting_address = EEPROM.read(1003);   //210
  //Serial.print(" id:------>");
  Serial.println(EEPROM.read(starting_address));
  int number_of_student = EEPROM.read(1001);  // 3
  Serial.print("Number OF Student: ");
  Serial.println((number_of_student));
  int last_address = number_of_student * 5 + 210 - 1;
  for (int i = starting_address; i <= last_address; i++) {
    if (i % 5 == 0) {
      Serial.print("Stored Id : ");
      Serial.print(i);
      Serial.print(" -> ");
      Serial.println(EEPROM.read(i));
    }
    else {
      Serial.print(i);
      Serial.print(" -> ");
      Serial.println(EEPROM.read(i));
    }


  }

}

void read_classroom_info()
{
  for (int i = 0; i < 5; i++) {
    String pin = "";
    String Date = "";
    String inserted_time = "";
    int no_of_attended_student;
    Serial.print("Classroom Code :");
    Serial.print(i * 13);
    Serial.print(" ->32");

    Serial.println(EEPROM.read(i * 13));
    for (int j = i * 13 + 1, k = 1; j < (i * 13) + 13; j++, k++) {
      if (k <= 4) {
        pin += EEPROM.read(j);
      } else if (k >= 5 && k <= 7) {
        Date += EEPROM.read(j);
        Date += '-';
      } else if (k == 8 || k == 9) {
        Date[Date.length() - 1] = ' ';
        //Date = Date[Date.size()-1];            //split last character
        //Date = Date.substr(0,Date.size()-1);
        inserted_time += EEPROM.read(j);
        inserted_time += ':';
      } else if (k == 10) {
        //inserted_time-=inserted_time[inserted_time.length()-1];       //split last character
        if (EEPROM.read(j) == 1 || EEPROM.read(j) == '1') {
          inserted_time += " AM";
        } else {
          inserted_time += " PM";
        }
      } else if (k == 11) {
        no_of_attended_student = EEPROM.read(j);
      }
    }
    //print everything in the console
    Serial.print("PIN : ");
    Serial.println(pin);
    Serial.print("Date : ");
    Serial.println(Date);
    Serial.print("Time : ");
    Serial.println(inserted_time);
    Serial.print("Number of attended student : ");
    Serial.println(no_of_attended_student);
    Serial.println();
  }
}

bool store_student_attendance(int classRoom_address, String submitted_id) {
  Serial.print("ClassRoom_address: ");
  Serial.println(classRoom_address);
  int attendance_address = classRoom_address + 10;
  int number_of_student_attendance = EEPROM.read(attendance_address);
  int classcode = EEPROM.read(classRoom_address - 1);
  Serial.print("Class Code : 32");
  Serial.println(classcode);
  int starting_attendance_address;
  if (classcode == 11) {
    starting_attendance_address = 500;
  } else if (classcode == 13) {
    starting_attendance_address = 550;
  } else if (classcode == 15) {
    starting_attendance_address = 600;
  } else if (classcode == 16) {
    starting_attendance_address = 650;
  } else if (classcode == 23) {
    starting_attendance_address = 700;
  }
  // have to check if the id already in the attendance list
  for (int i = starting_attendance_address; i <= starting_attendance_address + number_of_student_attendance; i++) {
    if (EEPROM.read(i) == submitted_id.toInt()) {
      Serial.println("Id has alreay in the database");
      return false;
    }
  }


  int storing_address = starting_attendance_address + number_of_student_attendance;
  EEPROM.write(storing_address, submitted_id.toInt());
  EEPROM.write(attendance_address, number_of_student_attendance + 1);
  Serial.print("Number Of Student attendance :");
  Serial.println(EEPROM.read(attendance_address));
  Serial.print("Storing Address :");
  Serial.println(storing_address);
  return true;
}

void read_student_attendance(int starting_address)
{
  for (int i = starting_address; i <= starting_address + 50; i++) {
    Serial.print(i);
    Serial.print(" -> ");
    Serial.println(EEPROM.read(i));
  }
}

void read_student_attendance_details(int code)      //  0/1/2/3/4
{
  int classCode = code * 13;
  int starting_address, no_of_attendent_student;
  if (classCode == 0) {
    starting_address = 500;
    no_of_attendent_student = EEPROM.read(classCode + 11);

  } else if (classCode == 13) {
    Serial.print("class selected: 13");
    Serial.println();

    starting_address = 550;

    no_of_attendent_student = EEPROM.read(classCode + 11);
  } else if (classCode == 26) {
    starting_address = 600;
    no_of_attendent_student = EEPROM.read(classCode + 11);
  } else if (classCode == 39) {
    starting_address = 650;
    no_of_attendent_student = EEPROM.read(classCode + 11);
  } else if (classCode == 52) {
    starting_address = 700;
    no_of_attendent_student = EEPROM.read(classCode + 11);
  }
  String ids = "";
  int j = 1, row = 0;
  lcd.clear();
  Serial.println(no_of_attendent_student);
  float temp = no_of_attendent_student;
  temp = ceil(temp / 5);

  for (int i = starting_address; i < starting_address + no_of_attendent_student; i++) {
    //500-506
    if (j <= 4) {
      ids += EEPROM.read(i); // 116,
      ids += ',';         //
      j++;
      Serial.print("If er j :");
      Serial.println(j);
    } else {
      Serial.println(ids);
      Serial.print("Else er j :");
      Serial.println(j);
      j = 1;
      Serial.print("Row :");
      Serial.println(row);
      ids += EEPROM.read(i);
      ids += ',';
      Display(0, row, ids); //109,110,112,114,115,
      row++;               //116,117,118
      Serial.print("new Row :");
      Serial.println(row);
      ids = "";


    }
  }
  if (j <= 5) {
    Display(0, row, ids);
  }
}



void delete_student_attendance_details(int code)
{
  int classCode = code * 13;
  EEPROM.write(classCode + 11, 0);
}

void code_kam_kore_na() {
  for (int i = 0; i < 65; i++) {
    Serial.print(i);
    Serial.print(" -> ");
    Serial.println(EEPROM.read(i));
  }

  for (int i = 210; i < 240; i++) {
    Serial.print(i);
    Serial.print(" -> ");
    Serial.println(EEPROM.read(i));
  }
}

void store_multiple_student_info()
{
  for (int starting_address = 210, id = 109; starting_address <= 270; starting_address += 5, id++) { //13 times [109-121]
    EEPROM.write(starting_address, id);
    Serial.print("ID : ");
    Serial.print(starting_address);
    Serial.print(" -> ");
    Serial.println(EEPROM.read(starting_address));
    String pin = String(id);
    for (int i = starting_address + 1, k = -1; i <= starting_address + 4; i++, k++) {
      if (k == -1) {
        EEPROM.write(i, 0);
        Serial.print(i);
        Serial.print("->");
        Serial.println(EEPROM.read(i));
      } else {
        EEPROM.write(i, (pin[k] - 48));
        Serial.print(i);
        Serial.print("->");
        Serial.println(EEPROM.read(i));
      }

    }
    Serial.println();
  }
  EEPROM.write(1001, 13);

}
int get_classroom_pin(int classroom_code)
{
  String pin = "";
  int pin_starting_address = classroom_code * 13 + 1;
  for (int i = pin_starting_address; i < pin_starting_address + 4; i++) {
    pin += EEPROM.read(i);
  }
  return pin.toInt();
}

void delete_registered_student(int starting_address)
{
  int number_of_student = EEPROM.read(1001);
  int last_registerd_address_id = 210 + number_of_student * 5 - 5;

  for (int i = starting_address, j = 0; i < starting_address + 5; i++, j++) {
    EEPROM.write(i, EEPROM.read(last_registerd_address_id + j));
  }
  EEPROM.write(1001, number_of_student - 1);
  read_student_info();
}
