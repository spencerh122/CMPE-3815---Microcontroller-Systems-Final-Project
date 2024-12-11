/*
Shea McGregor, Spencer Hart
Final Project Code: Testing of MPU6050

This is not code included on the final running aircraft, but an attempt to get useful readings from the MPU6050.
This is a mashup of code trying to get the MPU6050 to work correctly. 
The MPU6050 Raw example code from https://github.com/ElectronicCats/mpu6050/tree/master/examples/MPU6050_raw reads raw, drifting, error-filled data and transmits it to a
Madgwick filter written by Nicholas Rehm (GitHub: https://github.com/nickrehm/dRehmFlight)
*/

// Include libraries
#include "I2Cdev.h"
#include "MPU6050.h"

/* MPU6050 default I2C address is 0x68*/
MPU6050 mpu6050;

#define OUTPUT_READABLE_ACCELGYRO
//#define OUTPUT_BINARY_ACCELGYRO

// Accelerometer and gyroscope data variables
int16_t AcX, AcY, AcZ;
int16_t GyX, GyY, GyZ;
bool blinkState;

// General stuff
float dt;
unsigned long current_time, prev_time;


#define ACCEL_SCALE ACCEL_FS_SEL_2
#define ACCEL_SCALE_FACTOR 16384.0
#define GYRO_SCALE GYRO_FS_SEL_250
#define GYRO_SCALE_FACTOR 131.0
//IMU:
float AccX, AccY, AccZ;
float AccX_prev, AccY_prev, AccZ_prev;
float GyroX, GyroY, GyroZ;
float GyroX_prev, GyroY_prev, GyroZ_prev;
float MagX, MagY, MagZ;
float MagX_prev, MagY_prev, MagZ_prev;
float roll_IMU, pitch_IMU, yaw_IMU;
float roll_IMU_prev, pitch_IMU_prev;
float q0 = 1.0f;  //Initialize quaternion for madgwick filter
float q1 = 0.0f;
float q2 = 0.0f;
float q3 = 0.0f;
//IMU calibration parameters - calibrate IMU using calculate_IMU_error() in the void setup() to get these values, then comment out calculate_IMU_error()
float AccErrorX = 0.0;
float AccErrorY = 0.0;
float AccErrorZ = 0.0;
float GyroErrorX = 0.0;
float GyroErrorY = 0.0;
float GyroErrorZ = 0.0;
//Filter parameters - Defaults tuned for 2kHz loop rate; Do not touch unless you know what you are doing:
float B_madgwick = 0.04;  //Madgwick filter parameter
float B_accel = 0.14;     //Accelerometer LP filter paramter, (MPU6050 default: 0.14. MPU9250 default: 0.2)
float B_gyro = 0.1;       //Gyro LP filter paramter, (MPU6050 default: 0.1. MPU9250 default: 0.17)
float B_mag = 1.0;        //Magnetometer LP filter parameter
//Magnetometer calibration parameters - if using MPU9250, uncomment calibrateMagnetometer() in void setup() to get these values, else just ignore these
float MagErrorX = 0.0;
float MagErrorY = 0.0;
float MagErrorZ = 0.0;
float MagScaleX = 1.0;
float MagScaleY = 1.0;
float MagScaleZ = 1.0;

void setup() {
/*--Start I2C interface--*/
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  Serial.begin(38400);  //Initializate Serial wo work well at 8MHz/16MHz

  /*Initialize device and check connection*/
  Serial.println("Initializing MPU...");
  mpu6050.initialize();
  Serial.println("Testing MPU6050 connection...");
  if (mpu6050.testConnection() == false) {
    Serial.println("MPU6050 connection failed");
    while (true)
      ;
  } else {
    Serial.println("MPU6050 connection successful");
  }

  /* Use the code below to change accel/gyro offset values. Use MPU6050_Zero to obtain the recommended offsets */
  Serial.println("Updating internal sensor offsets...\n");
  mpu6050.setXAccelOffset(0);  //Set your accelerometer offset for axis X
  mpu6050.setYAccelOffset(0);  //Set your accelerometer offset for axis Y
  mpu6050.setZAccelOffset(0);  //Set your accelerometer offset for axis Z
  mpu6050.setXGyroOffset(0);   //Set your gyro offset for axis X
  mpu6050.setYGyroOffset(0);   //Set your gyro offset for axis Y
  mpu6050.setZGyroOffset(0);   //Set your gyro offset for axis Z
  /*Print the defined offsets*/
  Serial.print("\t");
  Serial.print(mpu6050.getXAccelOffset());
  Serial.print("\t");
  Serial.print(mpu6050.getYAccelOffset());
  Serial.print("\t");
  Serial.print(mpu6050.getZAccelOffset());
  Serial.print("\t");
  Serial.print(mpu6050.getXGyroOffset());
  Serial.print("\t");
  Serial.print(mpu6050.getYGyroOffset());
  Serial.print("\t");
  Serial.print(mpu6050.getZGyroOffset());
  Serial.print("\n");

  /*Configure board LED pin for output*/
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  prev_time = current_time;
  current_time = micros();
  dt = (current_time - prev_time) / 1000000.0;
  /* Read raw accel/gyro data from the module. Other methods commented*/
  mpu6050.getMotion6(&AcX, &AcY, &AcZ, &GyX, &GyY, &GyZ);
  //mpu6050.getAcceleration(&AcX, &AcY, &AcZ);
  //mpu6050.getRotation(&GyX, &GyY, &GyZ);

  // Print for debugging
  if (def OUTPUT_READABLE_ACCELGYRO) {
    Serial.print("a/g:\t");
    Serial.print(AcX);
    Serial.print("\t");
    Serial.print(AcY);
    Serial.print("\t");
    Serial.print(AcZ);
    Serial.print("\t");
    Serial.print(GyX);
    Serial.print("\t");
    Serial.print(GyY);
    Serial.print("\t");
    Serial.println(GyZ);
  }

  // if (def OUTPUT_BINARY_ACCELGYRO) {
  //   Serial.write((uint8_t)(AcX >> 8));
  //   Serial.write((uint8_t)(AcX & 0xFF));
  //   Serial.write((uint8_t)(AcY >> 8));
  //   Serial.write((uint8_t)(AcY & 0xFF));
  //   Serial.write((uint8_t)(AcZ >> 8));
  //   Serial.write((uint8_t)(AcZ & 0xFF));
  //   Serial.write((uint8_t)(GyX >> 8));
  //   Serial.write((uint8_t)(GyX & 0xFF));
  //   Serial.write((uint8_t)(GyY >> 8));
  //   Serial.write((uint8_t)(GyY & 0xFF));
  //   Serial.write((uint8_t)(GyZ >> 8));
  //   Serial.write((uint8_t)(GyZ & 0xFF));
  // }

  /*Blink LED to indicate activity*/
  blinkState = !blinkState;
  digitalWrite(LED_BUILTIN, blinkState);

  // Get the data from the IMU again, and pass it into the Madgwick filter to make it useful.
  // (We were not able to get the filter to get useful values out of the data)
  getIMUdata();
  Madgwick6DOF(GyroX, -GyroY, -GyroZ, -AccX, AccY, AccZ, dt);
}



void getIMUdata() {
  // Request full dataset from IMU and LP filter gyro, accelerometer, and magnetometer data. Adapted from https://github.com/nickrehm/dRehmFlight.
  /*
   * Reads accelerometer, gyro, and magnetometer data from IMU as AccX, AccY, AccZ, GyroX, GyroY, GyroZ, MagX, MagY, MagZ. 
   * These values are scaled according to the IMU datasheet to put them into correct units of g's, deg/sec, and uT. A simple first-order
   * low-pass filter is used to get rid of high frequency noise in these raw signals. Generally you want to cut
   * off everything past 80Hz, but if your loop rate is not fast enough, the low pass filter will cause a lag in
   * the readings. The filter parameters B_gyro and B_accel are set to be good for a 2kHz loop rate. Finally,
   * the constant errors found in calculate_IMU_error() on startup are subtracted from the accelerometer and gyro readings.
   */
  int16_t AcX, AcY, AcZ, GyX, GyY, GyZ;

  mpu6050.getMotion6(&AcX, &AcY, &AcZ, &GyX, &GyY, &GyZ);


  //Accelerometer
  AccX = AcX / ACCEL_SCALE_FACTOR;  //G's
  AccY = AcY / ACCEL_SCALE_FACTOR;
  AccZ = AcZ / ACCEL_SCALE_FACTOR;
  //Correct the outputs with the calculated error values
  AccX = AccX - AccErrorX;
  AccY = AccY - AccErrorY;
  AccZ = AccZ - AccErrorZ;
  //LP filter accelerometer data
  AccX = (1.0 - B_accel) * AccX_prev + B_accel * AccX;
  AccY = (1.0 - B_accel) * AccY_prev + B_accel * AccY;
  AccZ = (1.0 - B_accel) * AccZ_prev + B_accel * AccZ;
  AccX_prev = AccX;
  AccY_prev = AccY;
  AccZ_prev = AccZ;

  //Gyro
  GyroX = GyX / GYRO_SCALE_FACTOR;  //deg/sec
  GyroY = GyY / GYRO_SCALE_FACTOR;
  GyroZ = GyZ / GYRO_SCALE_FACTOR;
  //Correct the outputs with the calculated error values
  GyroX = GyroX - GyroErrorX;
  GyroY = GyroY - GyroErrorY;
  GyroZ = GyroZ - GyroErrorZ;
  //LP filter gyro data
  GyroX = (1.0 - B_gyro) * GyroX_prev + B_gyro * GyroX;
  GyroY = (1.0 - B_gyro) * GyroY_prev + B_gyro * GyroY;
  GyroZ = (1.0 - B_gyro) * GyroZ_prev + B_gyro * GyroZ;
  GyroX_prev = GyroX;
  GyroY_prev = GyroY;
  GyroZ_prev = GyroZ;
}

void Madgwick6DOF(float gx, float gy, float gz, float ax, float ay, float az, float invSampleFreq) {
  //DESCRIPTION: Attitude estimation through sensor fusion - 6DOF. From https://github.com/nickrehm/dRehmFlight.

  float recipNorm;
  float s0, s1, s2, s3;
  float qDot1, qDot2, qDot3, qDot4;
  float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2, _8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

  //Convert gyroscope degrees/sec to radians/sec
  gx *= 0.0174533f;
  gy *= 0.0174533f;
  gz *= 0.0174533f;

  //Rate of change of quaternion from gyroscope
  qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
  qDot2 = 0.5f * (q0 * gx + q2 * gz - q3 * gy);
  qDot3 = 0.5f * (q0 * gy - q1 * gz + q3 * gx);
  qDot4 = 0.5f * (q0 * gz + q1 * gy - q2 * gx);

  //Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
  if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {
    //Normalise accelerometer measurement
    recipNorm = invSqrt(ax * ax + ay * ay + az * az);
    ax *= recipNorm;
    ay *= recipNorm;
    az *= recipNorm;

    //Auxiliary variables to avoid repeated arithmetic
    _2q0 = 2.0f * q0;
    _2q1 = 2.0f * q1;
    _2q2 = 2.0f * q2;
    _2q3 = 2.0f * q3;
    _4q0 = 4.0f * q0;
    _4q1 = 4.0f * q1;
    _4q2 = 4.0f * q2;
    _8q1 = 8.0f * q1;
    _8q2 = 8.0f * q2;
    q0q0 = q0 * q0;
    q1q1 = q1 * q1;
    q2q2 = q2 * q2;
    q3q3 = q3 * q3;

    //Gradient decent algorithm corrective step
    s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
    s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
    s2 = 4.0f * q0q0 * q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
    s3 = 4.0f * q1q1 * q3 - _2q1 * ax + 4.0f * q2q2 * q3 - _2q2 * ay;
    recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3);  //normalise step magnitude
    s0 *= recipNorm;
    s1 *= recipNorm;
    s2 *= recipNorm;
    s3 *= recipNorm;

    //Apply feedback step
    qDot1 -= B_madgwick * s0;
    qDot2 -= B_madgwick * s1;
    qDot3 -= B_madgwick * s2;
    qDot4 -= B_madgwick * s3;
  }

  //Integrate rate of change of quaternion to yield quaternion
  q0 += qDot1 * invSampleFreq;
  q1 += qDot2 * invSampleFreq;
  q2 += qDot3 * invSampleFreq;
  q3 += qDot4 * invSampleFreq;

  //Normalise quaternion
  recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
  // Serial.println(q0);
  // Serial.println(q1);
  // Serial.println(q2);
  // Serial.println(q3);

  q0 *= recipNorm;
  q1 *= recipNorm;
  q2 *= recipNorm;
  q3 *= recipNorm;

  //Compute angles
  roll_IMU = atan2(q0 * q1 + q2 * q3, 0.5f - q1 * q1 - q2 * q2) * 57.29577951;                   //degrees
  pitch_IMU = -asin(constrain(-2.0f * (q1 * q3 - q0 * q2), -0.999999, 0.999999)) * 57.29577951;  //degrees
  yaw_IMU = -atan2(q1 * q2 + q0 * q3, 0.5f - q2 * q2 - q3 * q3) * 57.29577951;                   //degrees

  // Serial.println(roll_IMU);
  // Serial.println(pitch_IMU);
  // Serial.println(yaw_IMU);
}


float invSqrt(float x) {
  //Fast inverse sqrt for madgwick filter - From https://github.com/nickrehm/dRehmFlight.

  unsigned int i = 0x5F1F1412 - (*(unsigned int*)&x >> 1);
  float tmp = *(float*)&i;
  float y = tmp * (1.69000231f - 0.714158168f * x * tmp * tmp);
  return y;
}
