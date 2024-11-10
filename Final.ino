
#include <SoftwareSerial.h>
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>

MPU6050 mpu6050(Wire);
SoftwareSerial MySerial(10, 11);  // pin 10 connects to TX of HC-05 | pin 11 connects to RX of HC-05

// Create an instance of the MLX90614 sensor
Adafruit_MLX90614 mlx = Adafruit_MLX90614();


long timer = 0;
long sleep_timer_start, sleep_timer_end, sleep_timer_end2;
float x, y, z;
int activate, interrupt, stage_sleep_time, interrupt_sleep_timer, interrupt_for_deep_sleep, total_sleep, total_deep_sleep, total_light_sleep, deep_sleep, light_sleep, interrupt_timer = 0;

void setup() {
  Serial.begin(38400);
  // Initialize SoftwareSerial for Bluetooth communication at 9600 baud rate
  MySerial.begin(9600);  // HC-05 default baud rate
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);

  if (!mlx.begin()) {
    Serial.println("Failed to find MLX90614 sensor!");
    while (1)
      ;
  }
}

void loop() {
  mpu6050.update();

  if (millis() - timer > 1000) {
    x = mpu6050.getGyroX();
    y = mpu6050.getGyroY();
    z = mpu6050.getGyroZ();

    if (activate == 0) {  // first sleep confirmation
      if ((x <= 20 || x >= -20) && (y <= 20 || y >= -20) && (z <= 20 || z >= -20)) {
        sleep_timer_start = millis() / 1000 - sleep_timer_end;
        if (sleep_timer_start == 6)  //300sec =5min ,for now 6sec
        {
          activate = 1;
          Serial.println("Entered Sleep Mode");
        }
      }
      if ((x >= 20 || x <= -20) || (y >= 20 || y <= -20) || (z >= 20 || z <= -20)) {
        sleep_timer_end = (millis() / 1000);
      }
    }

    if (activate == 1) {  // sleeping mode
      light_sleep = (millis() / 1000) - sleep_timer_end;

      if (interrupt == 0) {
        if (light_sleep >= 84)  //4200sec =1hr 10min ,for now 84sec
        {
          // if (interrupt_for_deep_sleep > 84)  //4200sec =1hr 10min ,for now 84sec
          // {
          if (light_sleep - interrupt_sleep_timer >= 12)  //600sec =10min ,for now 12sec
          {
            deep_sleep = light_sleep - interrupt_for_deep_sleep;
          }
          // }
        }
      }
      light_sleep = light_sleep - deep_sleep;

      if ((x >= 20 || x <= -20) || (y >= 20 || y <= -20) || (z >= 20 || z <= -20)) {
        interrupt_sleep_timer = (millis() / 1000) - sleep_timer_end;
        interrupt_for_deep_sleep = light_sleep;
        interrupt = interrupt + 1;

        Serial.print("Interrupt Detected - interrupt_sleep_timer:");
        Serial.print(interrupt_sleep_timer);
        Serial.print(", interrupt_for_deep_sleep:");
        Serial.println(interrupt_for_deep_sleep);

        // delay(8000);
      }



      if ((millis() / 1000) - sleep_timer_end - interrupt_sleep_timer > 6)  //300sec =5min ,for now 6sec
      {
        interrupt = 0;
      }

      if ((millis() / 1000) - sleep_timer_end - interrupt_sleep_timer <= 6)  //300sec =5min ,for now 6sec
      {
        if (interrupt >= 3)  //5interrupt ,for now 3
        {
          sleep_timer_end = (millis() / 1000);
          if (light_sleep >= 18) {  // second sleep confirmation
                                    //900sec =15min ,for now 18sec
            total_light_sleep = total_light_sleep + light_sleep;
            total_deep_sleep = total_deep_sleep + deep_sleep;
            total_sleep = total_light_sleep + total_deep_sleep;
          }
          activate = 0;
          interrupt = 0;
          deep_sleep = 0;
          light_sleep = 0;
          interrupt_sleep_timer = 0;
          interrupt_for_deep_sleep = 0;
        }
      }
    }
    stage_sleep_time = light_sleep + deep_sleep;
    if (stage_sleep_time >= 108)
    //5400sec =90min ,for now 108sec
    {
      sleep_timer_end = (millis() / 1000);
      total_light_sleep = total_light_sleep + light_sleep;
      total_deep_sleep = total_deep_sleep + deep_sleep;
      total_sleep = total_light_sleep + total_deep_sleep;
      activate = 0;
      interrupt = 0;
      deep_sleep = 0;
      light_sleep = 0;
      interrupt_sleep_timer = 0;
      interrupt_for_deep_sleep = 0;
    }

    Serial.print(sleep_timer_start);  // just to know code and sensor working fine
    Serial.print(",");

    if (light_sleep >= 18) {  // 900sec = 15min, for now 18sec
      // Serial.println("light_sleep >= 18");

      Serial.print("light_sleep:" + String(light_sleep));  // / 60);
      // MySerial.print(light_sleep);
      // MySerial.print(", ");
      Serial.print(", ");

      Serial.print("deep_sleep:" + String(deep_sleep));  // / 60);
      // MySerial.print(deep_sleep);
      // MySerial.print(", ");
      Serial.print(", ");

      Serial.print("Interrupts:" + String(interrupt));
      Serial.print(", ");

      Serial.print("total_light_sleep:" + String(total_light_sleep));  // / 60);
      // MySerial.print(total_light_sleep);
      // MySerial.print(", ");
      Serial.print(", ");

      Serial.print("total_deep_sleep:" + String(total_deep_sleep));  // / 60);
      // MySerial.print(total_deep_sleep);
      // MySerial.print(", ");
      Serial.print(", ");

      Serial.print("total_sleep:" + String(total_sleep));  // / 60);
      // MySerial.print(total_sleep);
      // MySerial.print(", ");
      Serial.println(";");
    } else {
      // Serial.println("light_sleep < 18");  // 900sec = 15min, for now 18sec

      Serial.print("light_sleep:0, ");
      // MySerial.print(0);
      // MySerial.print(", ");

      Serial.print("deep_sleep:0, ");
      // MySerial.print(0);
      // MySerial.print(", ");

      Serial.print("Interrupts:" + String(interrupt));
      Serial.print(", ");

      Serial.print("total_light_sleep:" + String(total_light_sleep));  // / 60);
      // MySerial.print(total_light_sleep);
      // MySerial.print(", ");
      Serial.print(", ");

      Serial.print("total_deep_sleep:" + String(total_deep_sleep));  // / 60);
      // MySerial.print(total_deep_sleep);
      // MySerial.print(", ");
      Serial.print(", ");

      Serial.print("total_sleep:" + String(total_sleep));  // / 60);
      // MySerial.print(total_sleep);
      // MySerial.print(";");
      Serial.println(";");

      //MySerial.print("total_light_sleep:");


      // MySerial.print("total_sleep:");
    }

    MySerial.print(total_sleep);
    MySerial.print(",");
    MySerial.print(total_light_sleep);
    MySerial.print(",");
    // MySerial.print("total_deep_sleep:");
    MySerial.print(total_deep_sleep);
    MySerial.print(",");
    //MySerial.print("Deep sleep percentage:");
    MySerial.print((total_light_sleep * 100) / total_sleep);
    //  MySerial.print("% ");
    MySerial.print(",");
    //MySerial.print("Deep sleep percentage:");
    MySerial.print(100 - (total_light_sleep * 100) / total_sleep);
    // MySerial.print("% ");
    MySerial.println(";");


    timer = millis();
  }
  //Serial.print("Body Temperature:");
  Serial.print(mlx.readObjectTempC());
  Serial.print("°C  ");
  MySerial.print(mlx.readObjectTempC());
  // MySerial.print("°C");
  MySerial.print(",");
  MySerial.print(mlx.readObjectTempC());
  // MySerial.print("°C");
  MySerial.print(",");

  delay(1000);
}
