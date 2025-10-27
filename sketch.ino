//Theo Immanuel S.
//24/534368/TK/59227

#include <Wire.h> //requirement untuk menjalankan library MPU6050
#include <Adafruit_Sensor.h> //requirement untuk menjalankan library MPU6050
#include <ESP32Servo.h> //library untuk mengendalikan servo di ESP32
#include <Adafruit_MPU6050.h> //library untuk mengendalikan sensor MPU6050

Adafruit_MPU6050 mpu; //membuat objek sensor MPU
Servo servo1, servo2, servo3, servo4, servo5; //membuat objek servo

//menyebutkan pin penghubung servo
#define SERVO1_PIN 23
#define SERVO2_PIN 17
#define SERVO3_PIN 25
#define SERVO4_PIN 14
#define SERVO5_PIN 13

//inisialisasi variabel gyro (sudut & percepatan)
float roll = 0, pitch = 0, yaw = 0;
float gyroX, gyroY, gyroZ;

//inisialisasi variabel waktu
unsigned long prevTime = 0;
float dt = 0;

void controlServos() { //fungsi untuk mengontrol gerakan servo
  //Servo 1 & 2: Roll --> gerakan berlawanan
  int servo1Pos = 90 - roll;
  int servo2Pos = 90 - roll;
  
  //Servo 3 & 4: Pitch --> gerakan searah
  int servo3Pos = 90 + pitch;
  int servo4Pos = 90 + pitch;
  
  //Servo 5: Yaw --> gerakan searah
  int servo5Pos = 90 + yaw;
  
  //batasan posisi servo
  servo1Pos = constrain(servo1Pos, 0, 180);
  servo2Pos = constrain(servo2Pos, 0, 180);
  servo3Pos = constrain(servo3Pos, 0, 180);
  servo4Pos = constrain(servo4Pos, 0, 180);
  servo5Pos = constrain(servo5Pos, 0, 180);
  
  //nulis data posisi ke servo
  servo1.write(servo1Pos);
  servo2.write(servo2Pos);
  servo3.write(servo3Pos);
  servo4.write(servo4Pos);
  servo5.write(servo5Pos);
  
  //cek external movement
  detectExternalMovement();
}

void detectExternalMovement() { //fungsi deteksi dan kontrol terhadap external movement
  
  sensors_event_t a, g, temp; //baca data dari sensor 
  mpu.getEvent(&a, &g, &temp); //ada accel, gyro, sama temp (ga dipake)
  
  //total accel (√(x² + y² + z²))
  float totalAccel = sqrt(a.acceleration.x * a.acceleration.x + 
                          a.acceleration.y * a.acceleration.y + 
                          a.acceleration.z * a.acceleration.z);
  
  //external movement --> signifikan terhadap gravitasi
  if (abs(totalAccel - 9.8) > 5.0) {
    //biar balik ke posisi asli (decay)
    roll = roll * 0.9;
    pitch = pitch * 0.9;
    yaw = yaw * 0.9;
    
    //kalo udah kecil banget, dijadiin 0
    if (abs(roll) < 1.0) roll = 0;
    if (abs(pitch) < 1.0) pitch = 0;
    if (abs(yaw) < 1.0) yaw = 0;
  }
}

void setup() {
  Serial.begin(115200); //mengaktifkan serial monitor dengan baudrate 115200

  //menghubungkan servo ke pin servo
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);
  servo4.attach(SERVO4_PIN);
  servo5.attach(SERVO5_PIN);

  //inisialisasi sensor MPU
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  unsigned long prevTime = millis();
}

void loop() {
  //sensor event --> ambil gyro aja
  sensors_event_t a, g, temp; //baca data dari sensor 
  mpu.getEvent(&a, &g, &temp); //ada accel, gyro, sama temp (ga dipake)
  
  //hitungan perubahan waktu
  unsigned long currentTime = millis();
  dt = (currentTime - prevTime) / 1000;
  prevTime = currentTime;
   
  //data kecepatan sudut --> ngubah pengukuran radian ke derajaat
  gyroX = g.gyro.x * 180.0 / PI;
  gyroY = g.gyro.y * 180.0 / PI;
  gyroZ = g.gyro.z * 180.0 / PI;
  
  //hitung perubahan di tiap sumbu --> s = v.t
  roll += gyroX * dt;
  pitch += gyroY * dt;
  yaw += gyroZ * dt;
  
  //pembatasan rentang rotasi bidang
  roll = constrain(roll, -90, 90);
  pitch = constrain(pitch, -90, 90);
  yaw = constrain(yaw, -90, 90);
  
  //jalankan fungsi controlServos --> mengontrol gerakan servo
  controlServos();
  
  delay(100); //delay antar pengukuran
}