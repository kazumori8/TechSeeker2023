#include <Stepper.h>

// MOTER
// モーターの1回転ステップ数（ST-42BYH1004の場合）
int around_step = 400;
// ボタンを一回を押して進むステップ数
int step = 1;         // 正転：10、反転：-10
int speed_slow = 80;  // 回転スピード（10：遅い、50：早い）
int speed_fast = 130;
int rotate_step = 1000;  //回転角度
// int rotate_step = 100; //回転角度

// PIN
int rasp_pin = 12;  // スイッチの入力Pin
int led_pin = 7;    // LEDの出力Pin
int vol_pin = 13;   //Keep High for 5v
int right_pin_up = 4; //これでStanbyかendか切り替える
int right_pin_down = 5; //動かせー！！
int test_pin = 2;

// stepper インスタンス生成
Stepper stepper_left(around_step, 8, 9, 10, 11);

//  ---------   Left -------------
void hit_fast_left_once() {
  stepper_left.setSpeed(speed_fast);  // 1分間当たりの回転数を設定(rpm)

  // stepper_left.step(rotate_step);// ageru

  stepper_left.step(-rotate_step);//下げる
  delay(500);
  stepper_left.step(rotate_step); //上げる
}
void hit_slow_left_once() {
  stepper_left.setSpeed(speed_slow);  // 1分間当たりの回転数を設定(rpm)
  stepper_left.step(-rotate_step); //下げる
  digitalWrite(8, LOW);  // 出力を停止(モーターへの電流を止め発熱を防ぐ)
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  delay(1000);
  stepper_left.step(rotate_step); //上げる
  delay(1000);
}

//----------  Debug -------------
void call_status(int rasp_sw, int counts) {
  Serial.print("loop end  sw:");
  Serial.print(rasp_sw);
  Serial.print(",  counts:");
  Serial.println(counts);
}

void standby() {                     //両手を上げる。
  digitalWrite(led_pin, HIGH);       //目を光らす
  digitalWrite(right_pin_up, HIGH);  //5V出す
  stepper_left.setSpeed(speed_slow);  // 1分間当たりの回転数を設定(rpm)
  stepper_left.step(rotate_step);    //左手を上げる。
}
void end() {                    //両手を下げる。
  stepper_left.setSpeed(speed_slow);  // 1分間当たりの回転数を設定(rpm)
  digitalWrite(right_pin_up, LOW);   //左手を下げる
  stepper_left.step(-rotate_step); //左手を下げる。
  digitalWrite(8, LOW);  // 出力を停止(モーターへの電流を止め発熱を防ぐ)
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  digitalWrite(led_pin, LOW);  //目を消す
}

//----------  main  -------------
void setup() {
  Serial.begin(115200);  // シリアルポートを初期
  pinMode(rasp_pin, INPUT);
  pinMode(test_pin, INPUT);
  pinMode(led_pin, OUTPUT);
  pinMode(right_pin_up, OUTPUT);
  pinMode(right_pin_down, OUTPUT);
  pinMode(vol_pin, OUTPUT);

  digitalWrite(right_pin_up, LOW);
  digitalWrite(right_pin_down, LOW);
  digitalWrite(led_pin, LOW);  //目を暗くする
  digitalWrite(vol_pin, HIGH);  //5V出す
  
  Serial.println("Arduino start!");
  // hit_fast_left_once();
  Serial.println("End test!");
}


int rasp_sw = 0;
int counts = 0;
int status = 1;

void loop() {
  rasp_sw = digitalRead(rasp_pin);
  // rasp_sw = digitalRead(test_pin);
  Serial.print("rasp sw:");
  Serial.print(rasp_sw);
  Serial.print(", status:");
  Serial.println(status);

  //status : 1(Nan), 2(StandBy), 3(Wake Up Now), 4(end Function)
  switch (status) {
    case 1:
      if(rasp_sw){
        standby();
        delay(1000);
        status = 2;
      }
      break;
    case 2: //腕が上がっている、ラズパイの１を待つ
      if(rasp_sw){
        status = 3;
      }
      break;
    case 3: //ラズパイから信号を来た！腕を動かす。
      if(rasp_sw){ //腕を下げ、上げする
        if (counts > 2) {
          hit_fast_left_once();
          //Right ArmのpinをHIGH
          digitalWrite(right_pin_down, HIGH);
        } else {
          hit_slow_left_once();
        }
        counts++;
      }else{ //アラームが終わったぞ
        //Right ArmのpinをLOW
        digitalWrite(right_pin_down, LOW);
        status = 4;
        counts = 0;
      }
      break;
    case 4: //アラームが終了したので腕を下ろす
      end();
      //腕を下ろしたら最初に戻る
      status = 1;
      break;  
    default:
      Serial.print("rasp sw:");
      Serial.print(rasp_sw);
      Serial.print(", status:");
      Serial.println(status);
  }
}