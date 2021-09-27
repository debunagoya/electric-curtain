#include <DS3232RTC.h>
#include <Stepper.h>

#define MOTOR_1   (8)   // blue
#define MOTOR_2   (9)   // pink
#define MOTOR_3   (10)   // yellow
#define MOTOR_4   (11)   // orange
#define MOTOR_STEPS (2048)
#define SWITCH_PIN (7)

Stepper stepper(MOTOR_STEPS, MOTOR_1, MOTOR_3, MOTOR_2, MOTOR_4);

String status;

void setup() {
  /* Switch */
  pinMode(SWITCH_PIN, INPUT);

  /* RTC */
//  setTime(5, 35, 00, 21, 8, 2021);  // 時,分,秒,日,月,年の順となっています
//  RTC.set(now());                   // 時刻情報を書き込む

  setSyncProvider(RTC.get);         // RTCから時刻を取得する関数

  /* Stepping moter */
  stepper.setSpeed(15);

  Serial.begin(9600);
}

void loop() {
  int ct = 0 ;
  unsigned long UNIXTIME1 = gettime() ;
  unsigned long UNIXTIME2 = gettime() ;
  unsigned long UNIXTIME3 = gettime() ;
  
  /* 開く */
  if ( hour() >= 6 and hour() < 18 and status != "open" ) {
    while ( true ) {
      stepper.step( -1 * MOTOR_STEPS * 0.1 ); // 0.1周
      if ( digitalRead(SWITCH_PIN) == 1 ) {
        if ( ct == 10 ) {
          UNIXTIME2 = gettime() - UNIXTIME1;
          stopMotor();
          status = "open";
          break;
        }
        ct += 1;
      } else if ( digitalRead(SWITCH_PIN) == 0 ) {
        ct = 0;
      }
    }
  } 
  /* 閉じる */
  else if ( hour() < 6 and hour() >= 18 and status == "open" ) {
    while ( UNIXTIME3 - UNIXTIME1 < UNIXTIME2 ) {
      UNIXTIME3 = gettime() ;
      stepper.step( 1 * MOTOR_STEPS * 0.1 ); // 0.1周
    }
    stopMotor();
    status = "close";
  }
}

unsigned long gettime() {
  return ((365*year()+(year()/4)-(year()/100)+(year()/400)+(306*(month()+1)/10)-428+day())-719161)*86400+(hour()*3600)+(minute()*60)+second() ;
}

/* モーターへの電流を止める */
void stopMotor() {
  digitalWrite(MOTOR_1, LOW);
  digitalWrite(MOTOR_2, LOW);
  digitalWrite(MOTOR_3, LOW);
  digitalWrite(MOTOR_4, LOW);
}
