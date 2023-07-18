#include <Stepper.h>
// モーターの1回転ステップ数（ST-42BYH1004の場合）
int around_step = 400;
// ボタンを一回を押して進むステップ数
// 正転：10、反転：-10
int step = 10;
// 回転スピード（10：遅い、50：早い）
int speed_slow = 60;
int speed_fast = 130;

//回転角度
int rotate_step = 900;
int right_pin_up = 4;
int right_pin_down = 5;

// stepper インスタンス生成
Stepper stepper(around_step, 8, 9, 10, 11);

void up() {
  stepper.setSpeed(speed_fast);  // 1分間当たりの回転数を設定(rpm)
  stepper.step(-rotate_step);
  delay(100);
}
void down() {
  stepper.setSpeed(speed_fast);  // 1分間当たりの回転数を設定(rpm)
  // stepper.step(rotate_step); //. -------------------------------------本ばんはここをコメントアウト＆delayする
  digitalWrite(8, LOW);  // 出力を停止(モーターへの電流を止め発熱を防ぐ)
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  delay(1000);
  }
void check_sw(int right_up_sw, int right_down_sw) {
  Serial.print("right_up:");
  Serial.print(right_up_sw);
  Serial.print(", right_down:");
  Serial.println(right_down_sw);
}

void setup() {
  Serial.begin(9600);  // シリアルポートを初期化
  // pinMode(sw_pin, INPUT_PULLUP);
  pinMode(right_pin_up, INPUT);
  pinMode(right_pin_down, INPUT);
  Serial.println("Arduino start!");
  // up();
  // delay(1000);
  // down();
  Serial.println("Arduino end!");
}

int right_up_sw = 0;
int right_down_sw = 0;
int counts = 0;
int status = 1;
bool flg = true; //待機状態かどうか、true(待ち), false(終わった)

void loop() {
  right_up_sw = digitalRead(right_pin_up);
  right_down_sw = digitalRead(right_pin_down);
  Serial.print("status: ");
  Serial.print(status);
  Serial.print(", up_sw: ");
  Serial.print(right_up_sw);
  Serial.print(", down_sw: ");
  Serial.println(right_down_sw);

  //status: 1(wait), 2(スタンバイモードでしばく), 3(腕を下げる)
  switch (status) {
    case 1: //スタンバイ状態になるのを待つ
      if(right_up_sw){
        up();
        flg = true; //待機状態かどうか、true(待ち), false(終わった)
        status = 2;
      }
      break;
    case 2: //動かすモードになったら腕を動かす、
      if(right_down_sw){
        flg = false; //false(終わった) 待機状態じゃないぞい
        down();
        up();
      }else{
        if(!flg){ //終わったぞ
          status = 3;
        }
      }
      break;
    case 3: //スタンバイ状態になるのを待つ
      down();
      status = 1;
      break;
    default:
      Serial.print("right_up_sw: ");
      Serial.print(right_up_sw);
      Serial.print("right_down_sw: ");
      Serial.print(right_down_sw);
  }
}