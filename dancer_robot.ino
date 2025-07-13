#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVO_MIN 150  // Pulso mínimo (~0 graus)
#define SERVO_MAX 600  // Pulso máximo (~180 graus)
#define ANGLE_MAX 160
#define ANGLE_MIN 0

#define MAX_POSES 2

#define default_rh 0
#define default_re 0
#define default_prs 110
#define default_ors 0
#define default_ols 135
#define default_pls 60
#define default_le 120
#define default_lh 60
#define default_h 90

int current_angle[9] = { 0 };
int next_angle[9] = { 0 };
bool toggle = false;
String texto = "-1";

typedef struct move {
  int size;  //quantidade de poses
  int speed;
  int poses[MAX_POSES][9];
} Move;

Move happy;
Move neutral;
Move euphoric;
Move parado;
Move sad;
Move loving;

// Corrigido: posições alinhadas com o enum Servo
int right_up_left_down[9] = {
  default_rh + 90,  // RIGHT_HAND
  default_re,       // RIGHT_ELBOW
  default_prs,      // P_RIGHT_SHOULDER
  180,              // O_RIGHT_SHOULDER
  default_ols,      // O_LEFT_SHOULDER
  default_pls,      // P_LEFT_SHOULDER
  default_le,       // LEFT_ELBOW
  default_lh,       // LEFT_HAND
  default_h         // HEAD
};

int left_up_right_down[9] = {
  default_rh + 90,  // RIGHT_HAND
  default_re,       // RIGHT_ELBOW
  default_prs,      // P_RIGHT_SHOULDER
  default_ors,      // O_RIGHT_SHOULDER
  0,                // O_LEFT_SHOULDER
  default_pls,      // P_LEFT_SHOULDER
  default_le,       // LEFT_ELBOW
  default_lh,       // LEFT_HAND
  default_h         // HEAD
};

int neutral_right[9] = {
  default_rh,        // RIGHT_HAND
  default_re + 90,   // RIGHT_ELBOW
  default_prs - 60,  // P_RIGHT_SHOULDER
  default_ors - 10,  // O_RIGHT_SHOULDER
  default_ols,       // O_LEFT_SHOULDER
  default_pls,       // P_LEFT_SHOULDER
  default_le,        // LEFT_ELBOW
  default_lh,        // LEFT_HAND
  default_h          // HEAD
};

int neutral_left[9] = {
  default_rh,        // RIGHT_HAND
  default_re,        // RIGHT_ELBOW
  default_prs,       // P_RIGHT_SHOULDER
  default_ors,       // O_RIGHT_SHOULDER
  default_ols + 10,  // O_LEFT_SHOULDER
  default_pls + 60,  // P_LEFT_SHOULDER
  default_le - 90,   // LEFT_ELBOW
  ANGLE_MIN,         // LEFT_HAND
  default_h          // HEAD
};

int hands_up[9] = {
  default_rh + 90,  // RIGHT_HAND
  default_re + 30,  // RIGHT_ELBOW
  default_prs,      // P_RIGHT_SHOULDER
  90,               // O_RIGHT_SHOULDER
  50,               // O_LEFT_SHOULDER
  default_pls,      // P_LEFT_SHOULDER
  default_le - 30,  // LEFT_ELBOW
  default_lh,       // LEFT_HAND
  default_h         // HEAD
};

int hands_down[9] = {
  140,          // RIGHT_HAND
  70,           // RIGHT_ELBOW
  default_prs,  // P_RIGHT_SHOULDER
  default_ors,  // O_RIGHT_SHOULDER
  default_ols,  // O_LEFT_SHOULDER
  default_pls,  // P_LEFT_SHOULDER
  70,           // LEFT_ELBOW
  0,            // LEFT_HAND
  default_h     // HEAD
};

int parado_pose[9] = {
  default_rh,   // RIGHT_HAND
  default_re,   // RIGHT_ELBOW
  default_prs,  // P_RIGHT_SHOULDER
  default_ors,  // O_RIGHT_SHOULDER
  default_ols,  // O_LEFT_SHOULDER
  default_pls,  // P_LEFT_SHOULDER
  default_le,   // LEFT_ELBOW
  default_lh,   // LEFT_HAND
  default_h     // HEAD
};

int sad1[9] = {
  default_rh + 90,   // RIGHT_HAND **********
  default_re,        // RIGHT_ELBOW *********
  default_prs + 50,  // P_RIGHT_SHOULDER *******
  default_ors + 20,  // O_RIGHT_SHOULDER ************
  default_ols - 45,  // O_LEFT_SHOULDER
  default_pls - 75,  // P_LEFT_SHOULDER
  default_le - 40,   // LEFT_ELBOW
  default_lh - 60,   // LEFT_HAND
  default_h          // HEAD
};

int sad2[9] = {
  default_rh,   // RIGHT_HAND 
  default_re + 40,        // RIGHT_ELBOW 
  default_prs + 40,  // P_RIGHT_SHOULDER 
  default_ors + 60,  // O_RIGHT_SHOULDER 
  default_ols - 80,  // O_LEFT_SHOULDER
  default_pls + 75,  // P_LEFT_SHOULDER
  default_le - 60,   // LEFT_ELBOW
  default_lh + 40,   // LEFT_HAND
  default_h - 90,         // HEAD
};

int loving1[9] = {
  default_rh + 20,   // RIGHT_HAND 
  default_re + 40,        // RIGHT_ELBOW 
  default_prs + 60,  // P_RIGHT_SHOULDER 
  default_ors + 30,  // O_RIGHT_SHOULDER 
  default_ols - 90,  // O_LEFT_SHOULDER
  default_pls,  // P_LEFT_SHOULDER
  default_le ,   // LEFT_ELBOW
  default_lh ,   // LEFT_HAND
  default_h + 45,         // HEAD
};

int loving2[9] = {
  default_rh + 20,   // RIGHT_HAND 
  default_re + 40,        // RIGHT_ELBOW 
  default_prs + 60,  // P_RIGHT_SHOULDER 
  default_ors + 30,  // O_RIGHT_SHOULDER 
  default_ols - 90,  // O_LEFT_SHOULDER
  default_pls,  // P_LEFT_SHOULDER
  default_le ,   // LEFT_ELBOW
  default_lh ,   // LEFT_HAND
  default_h - 45,         // HEAD
};



enum Servo {
  RIGHT_HAND,        //0
  RIGHT_ELBOW,       //1
  P_RIGHT_SHOULDER,  //2
  O_RIGHT_SHOULDER,  //3
  O_LEFT_SHOULDER,   //4
  P_LEFT_SHOULDER,   //5
  LEFT_ELBOW,        //6
  LEFT_HAND,         //7
  HEAD               //8
};



Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void set_angle(int channel, int angle) {
  int pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
  pwm.setPWM(channel, 0, pulse);
}

void reset_servos() {
  set_angle(RIGHT_HAND, 80);
  next_angle[RIGHT_HAND] = 80;
  current_angle[RIGHT_HAND] = 80;

  set_angle(RIGHT_ELBOW, 0);
  next_angle[RIGHT_ELBOW] = 0;
  current_angle[RIGHT_ELBOW] = 0;

  set_angle(P_RIGHT_SHOULDER, 110);
  next_angle[P_RIGHT_SHOULDER] = 110;
  current_angle[P_RIGHT_SHOULDER] = 110;

  set_angle(O_RIGHT_SHOULDER, 0);
  next_angle[O_RIGHT_SHOULDER] = 0;
  current_angle[O_RIGHT_SHOULDER] = 0;

  set_angle(O_LEFT_SHOULDER, 135);
  next_angle[O_LEFT_SHOULDER] = 135;
  current_angle[O_LEFT_SHOULDER] = 135;

  set_angle(P_LEFT_SHOULDER, 60);
  next_angle[P_LEFT_SHOULDER] = 60;
  current_angle[P_LEFT_SHOULDER] = 60;

  set_angle(LEFT_ELBOW, 120);
  next_angle[LEFT_ELBOW] = 120;
  current_angle[LEFT_ELBOW] = 120;

  set_angle(LEFT_HAND, 60);
  next_angle[LEFT_HAND] = 60;
  current_angle[LEFT_HAND] = 60;

  set_angle(HEAD, 90);
  next_angle[HEAD] = 90;
  current_angle[HEAD] = 90;
}

void set_pose(int pose[9]) {
  next_angle[O_RIGHT_SHOULDER] = pose[O_RIGHT_SHOULDER];
  next_angle[P_RIGHT_SHOULDER] = pose[P_RIGHT_SHOULDER];
  next_angle[RIGHT_ELBOW] = pose[RIGHT_ELBOW];
  next_angle[RIGHT_HAND] = pose[RIGHT_HAND];
  next_angle[P_LEFT_SHOULDER] = pose[P_LEFT_SHOULDER];
  next_angle[O_LEFT_SHOULDER] = pose[O_LEFT_SHOULDER];
  next_angle[LEFT_ELBOW] = pose[LEFT_ELBOW];
  next_angle[LEFT_HAND] = pose[LEFT_HAND];
  next_angle[HEAD] = pose[HEAD];
}

void execute_move(Move move) {
  for (int step = 0; step < move.size; step++) {
    set_pose(move.poses[step]);

    bool done = false;
    while (!done) {
      done = true;
      for (int i = 0; i < 9; i++) {
        if (current_angle[i] < next_angle[i]) {
          current_angle[i] = min(current_angle[i] + move.speed, next_angle[i]);
          done = false;
        } else if (current_angle[i] > next_angle[i]) {
          current_angle[i] = max(current_angle[i] - move.speed, next_angle[i]);
          done = false;
        }
        set_angle(i, current_angle[i]);
      }
      delay(10);
    }

    delay(500);  // Pequena pausa entre poses
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Iniciando controle de servos...");

  pwm.begin();
  pwm.setPWMFreq(50);  // Frequência padrão de servo: 50Hz

  reset_servos();
  delay(500);

  happy.size = 2;
  happy.speed = 3;
  for (int i = 0; i < 9; i++) {
    happy.poses[0][i] = right_up_left_down[i];
    happy.poses[1][i] = left_up_right_down[i];
  }

  neutral.size = 2;
  neutral.speed = 2;
  for (int i = 0; i < 9; i++) {
    neutral.poses[0][i] = neutral_left[i];
    neutral.poses[1][i] = neutral_right[i];
  }

  euphoric.size = 2;
  euphoric.speed = 2;
  for (int i = 0; i < 9; i++) {
    euphoric.poses[0][i] = hands_up[i];
    euphoric.poses[1][i] = hands_down[i];
  }

  parado.size = 1;
  parado.speed = 1;
  for (int i = 0; i < 9; i++) {
    parado.poses[0][i] = parado_pose[i];
  }

  sad.size = 2;
  sad.speed = 1;
  for (int i = 0; i < 9; i++) {
    sad.poses[0][i] = sad1[i];
    sad.poses[1][i] = sad2[i];
  }

  loving.size = 2;
  loving.speed = 1;
  for (int i = 0; i < 9; i++) {
    loving.poses[0][i] = loving1[i];
    loving.poses[1][i] = loving2[i];
  }
}



void loop() {

  if (Serial.available() > 0) {
    texto = Serial.readStringUntil('\n');
    texto.trim();
    Serial.println(texto);
  }
  if (texto == "-1") texto = -1;
  else if (texto == "euphoric") execute_move(euphoric);
  else if (texto == "neutral") execute_move(neutral);
  else if (texto == "happy") execute_move(happy);
  else if (texto == "parado") execute_move(parado);
  else if (texto == "sad") execute_move(sad);
  else if (texto == "loving") execute_move(loving);
}