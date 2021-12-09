#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <Servo.h>
Servo servo;

#include <Keypad.h>

const byte Rows = 4;
const byte Cols = 4;

char keys[Rows][Cols] = {
  {'1', '2', '3', 'A'},  // A 뒤로가기
  {'4', '5', '6', 'B'},  // B 비밀번호 설정
  {'7', '8', '9', 'C'},  // C 지우기(Backspace)
  {'*', '0', '#', 'D'}   // D 입력완료
};

byte RowPins[Rows] = {5,4,3,2};  // 키패드 핀번호
byte ColPins[Cols] = {6,7,8,9};

Keypad keypad = Keypad(makeKeymap(keys), RowPins, ColPins, Rows, Cols);

int touch = 10;  // 터치센서

String password = "1234";  // 초기 비밀번호 초기화
String pincode = "123456";  // PIN코드 초기화

String pass = "";  // 입력 값 저장 변수
String pin = "";  // 입력 값 저장 변수

int count = 0;  // 틀린 횟수
int lcount = 0;  // lock 횟수

int degree=0;  // 서보 모터의 각도

void setup() {
  pinMode(touch, INPUT);  // 터치센서 Set
  servo.attach(11);
  servo.write(degree); 
  Serial.begin(9600);
}

void loop() {
  lcd.init();  // LCD초기화
  lcd.clear();
  lcd.noBacklight();  // 백라이트 ON
  
  Serial.print("터치센서 : ");
  Serial.println(digitalRead(touch));
  delay(100);

  
  if(digitalRead(touch) == HIGH){// 터치센서 작동 시 LCD화면 ON
    Serial.print("터치센서 : ");
    Serial.println(digitalRead(touch));
    
    //lcd.init();  // LCD초기화
    lcd.backlight();  // 백라이트 ON

    lcd_password();

    while(1){
      char key = keypad.getKey();  // 키패드 get변수
      
      if(key){
        if(key != 'A' && key != 'B' && key != 'C' && key != 'D'){
          lcd.print(key);  // 비밀번호 입력받아 lcd에 출력
          Serial.print(key);
          Serial.print("   count : "); 
          Serial.print(count); 
          Serial.print("   pass길이 : "); 
          Serial.print(pass.length()); 
          Serial.print("   password길이 : "); 
          Serial.print(password.length()); 
          Serial.print("   count : "); 
          Serial.print(count); 
          Serial.print("   lcount : "); 
          Serial.println(lcount); 
          pass.concat(key);
        }
        //----------------(A - 뒤로가기)------------------
        if(key == 'A'){
          Serial.println("LCD OFF");
          lcd.clear();
          lcd.println("exits...");
          
          lcd_off();  // lcd off함수 실행
          break;
        }
        //----------------(B - 비밀번호 설정)------------------
        else if(key == 'B'){
          Serial.println("비밀번호 변경");
          lcd.clear();
          lcd.print("Change");
          lcd.setCursor(0,1);
          lcd.println("        PassWord");
          
          delay(2000);
          
          lcd_pin();

          while(1){
            char key = keypad.getKey();  // 키패드 get변수
            
            if(key){
              if(key != 'A' && key != 'B' && key != 'C' && key != 'D'){
                Serial.println(key);
                lcd.print("*");
                pin.concat(key);
              }
              else if(key == 'A'){
                Serial.println("back");

                count = 0;
                
                lcd_password();
                break; 
              }
              else if(key == 'C'){
                Serial.println("Erase");
                lcd_pin();
                continue;
              }
              if(key == 'D'){
                if(pincode.length() == pin.length()){ // pin코드 길이비교
                  if(pincode.equals(pin)){  // pin코드 비교
                    lcd_changepw();
                    
                    while(1){
                      char key = keypad.getKey();  // 키패드 get변수
                      
                      if(key){
                        if(key != 'A' && key != 'B' && key != 'C' && key != 'D'){
                          Serial.println(key);
                          lcd.print(key);
                          pass.concat(key);
                        }
                        else if(key == 'A'){
                          Serial.println("back");
                          lcd_password();
                          break;
                        }
                        else if(key == 'C'){
                          Serial.println("Erase");
                          lcd_changepw();
                          continue;
                        }
                        else if(key == 'D'){
                          password = pass;
                          
                          count = 0;  // 비번 바꾸면 락 카운트 초기화
                          lcount = 0;
                          
                          lcd_password();
                          break;
                        }
                      }
                    }
                    // 변경할 password입력
                    break;
                  }else{
                    Serial.println("pin코드가 틀림");
                    incorrect_pincode();
                    lcd_pin();
                    continue;
                  }
                }else{
                  Serial.println("pin코드 길이 다름");
                  incorrect_pincode();
                  lcd_pin();
                  continue;
                }
              }
            }
          }
        }
        //----------------(C - 입력 지우기)------------------
        else if(key == 'C'){
          Serial.println("Erase");
          pass = "";
          lcd_password();
          continue;
        }
        //----------------(D - 입력완료)------------------
        else if(key == 'D'){
          if(password.length() == pass.length()){ // 두 PW의 길이비교
            if(password.equals(pass)){
              lcd.clear();
              lcd.print("Unlock");
              lcd.setCursor(0,1);
              lcd.print("Password");

              count =0;
              
              for(degree = 0; degree <= 180; degree++){
                servo.write(degree);
                delay(10);
              }
              // 5초 후 다시 잠금
              delay(3000);  
              
              for(degree = 180; degree >= 0; degree--){
                servo.write(degree);
                delay(10);
              }
              lcd_off();
              break;
            }else{
              Serial.println("비밀번호가 틀림");
              count++;
              
              incorrect_password();
              lcd_password();
              continue;
            }
          }else{
            Serial.println("비밀번호 길이 다름");
            count++;
            
            incorrect_password();
            lcd_password();
            continue;
          }
        }
      }
      // -----(Lock count)-----
      if(count == 3){  // 3번 틀리면 lock
        lock();
        delay(5000);
        
        lcount++;
        
        lcd_password();
        continue;
      }
      if(lcount == 3){  // 3번 lock걸리면 pin코드 잠금
        lock();
        delay(2000);
        
        lcd_pin();

        while(1){
            char key = keypad.getKey();  // 키패드 get변수
            
            if(key){
              if(key != 'A' && key != 'B' && key != 'C' && key != 'D'){
                Serial.println(key);
                lcd.print("*");
                pin.concat(key);
              }
              else if(key == 'C'){
                Serial.println("Erase");
                lcd_pin();
                continue;
              }
              else if(key == 'D'){
                if(pincode.length() == pin.length()){ // 두 PW의 길이비교
                  if(pincode.equals(pin)){
                    lcd.clear();
                    lcd.print("Unlock");
                    delay(2000);
                    
                    lcount = 0;
                    lcd_password();
                    break;
                  }else{
                    Serial.println("pin코드가 틀림");
                    incorrect_pincode();
                    lcd_pin();
                    continue;
                  }
                }else{
                  Serial.println("pin코드 길이 다름");
                  incorrect_pincode();
                  lcd_pin();
                  continue;
                }  
              }
            }
        }  // end  lock while
        continue;
      }  // end lcount==3
    }  // end while
  }  // end 터치센서
}  // end loop

void lcd_password(){
  lcd.clear();
  
  pin = "";
  
  lcd.print("Enter PassWord");
  lcd.setCursor(0,1);
  lcd.print(": ");
}
void lcd_changepw(){
  lcd.clear();
  
  pin = "";
  
  lcd.print("Change PassWord");
  lcd.setCursor(0,1);
  lcd.print(": ");
}
void lcd_pin(){
  lcd.clear();  // 모든 내용 제거 후 커서 0,0으로 이동
  
  pin = "";
  
  lcd.print("Enter PIN code");
  lcd.setCursor(0,1);
  lcd.print(": ");
}
void lcd_off(){
  pass = "";  // pass 초기화
  pin = "";
  
  delay(2000);
  lcd.clear();  // lcd 모든 내용 제거
  lcd.noBacklight();  // 백라이트 off.1[
}

void incorrect_password(){  // 잘못된 비밀번호 입력
  pass = "";
  
  lcd.clear();
  lcd.print("Incorrect");
  lcd.setCursor(0,1);
  lcd.print("PassWord");
  
  delay(1000);
}

void incorrect_pincode(){  // 잘못된 비밀번호 입력
  lcd.clear();
  lcd.print("Incorrect");
  lcd.setCursor(0,1);
  lcd.print("PIN Code");
  
  delay(1000);
}

void lock(){
  count = 0;
  
  lcd.clear();
  Serial.print("Locked");
  lcd.print("Locked");
}
