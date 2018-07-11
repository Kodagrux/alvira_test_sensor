/*
 * Project PhotonTest
 * Description:
 * Author:
 * Date:
 */

#include <ArduinoJson.h>

#define MOTIONPIN D0
#define MOTIONPIN2 A0
#define BUZZERPIN D2
#define LEDPIN1 D0
#define LEDPIN2 D7

int motionNumber;
int heartbeatNumber;
int motion;
int motionData;
int val;

String uuid;

bool armingStatus;
bool larmStatus;



void setup() {

    pinMode(LEDPIN1, OUTPUT);
    pinMode(LEDPIN2, OUTPUT);
    pinMode(MOTIONPIN, INPUT);
    pinMode(BUZZERPIN, OUTPUT);


    // Subscribe to the integration response event
    Particle.subscribe(System.deviceID() + "/hook-response/motion/", motionServerResponse, MY_DEVICES);
    Particle.subscribe(System.deviceID() + "/hook-response/getState/", getStateServerResponse, MY_DEVICES);
    Serial.begin(9600);


    motionNumber = 0;
    heartbeatNumber = 0;
    motion = 0;

    armingStatus = false;
    larmStatus = false;

    Particle.publish("Setup", "Complete", PRIVATE);
    Serial.println("Setup Complete");
    digitalWrite(LEDPIN1, LOW);
    digitalWrite(LEDPIN2, LOW);

    //Testing

    char uuid[37];
    uuid::generateUuidV4(uuid);

    //size_t length = EEPROM.length(); //2048
    //Particle.publish("Size 1", String(length), PRIVATE);

    struct SensorMemory {
        //uint8_t version;
        //float field1;
        //uint16_t field2;
        char uuid[37];
    };
    int addr = 40;
    SensorMemory sensorData = {"7f2a1719-5594-4005-b47d-c7c19177c905"};
    EEPROM.put(addr, sensorData);
    SensorMemory sensorData2;
    EEPROM.get(addr, sensorData2);
    uuid = sensorData2.uuid;

    //Particle.publish("Size 2", String(EEPROM.length()), PRIVATE);
    Particle.publish("Eprom", String(String(sensorData2.uuid)), PRIVATE);

}


void loop() {

    //motion = digitalRead(MOTIONPIN);
    int delayTime;

    if (armingStatus == true) {

        Particle.publish("getState", String(uuid), PRIVATE);

        if (larmStatus == true) {
            tone(BUZZERPIN, 4000, 5);
            delayTime = 500;

        } else {
            delayTime = 1500;
        }

        //Sample the motion-sensor 100 times during one second
        int motionData = 0;
        for (int i = 0; i < 100; i += 1) {
            int rawMotionData = digitalRead(MOTIONPIN);
            if (rawMotionData > motionData) {
                motionData = rawMotionData;
            }
            delay(10);
        }

        if (motionData == 1) {
            // If there is a motion

            motionNumber = motionNumber + 1;
            digitalWrite(LEDPIN1, HIGH);
            digitalWrite(LEDPIN2, HIGH);
            tone(BUZZERPIN, 3000, 3);

            String value = String(motionNumber);
            Particle.publish("motion", uuid, PRIVATE);
            Serial.println("motion " + String(value));

            larmStatus = true;

        } else {
            // If there isn't a motion
            //Serial.println("heartbeat " + String(heartbeatNumber));
            Particle.publish("heartbeat", String(heartbeatNumber), PRIVATE);

            //Particle.publish("Motion", uuid, PRIVATE);
            digitalWrite(LEDPIN1, LOW);
            digitalWrite(LEDPIN2, LOW);
        }




    } else {

        Particle.publish("getState", String(uuid), PRIVATE);

        delayTime = 15000;

    }


    heartbeatNumber = heartbeatNumber + 1;

    delay(delayTime);
    Serial.println("heartbeat = " + String(heartbeatNumber) + ", armed = " + String(armingStatus) + ", larm = " + String(larmStatus));

}


void motionServerResponse(const char *event, const char *data) {
    // Handle the integration response
    DynamicJsonBuffer jsonBuffer;


    Serial.println("Motion Reponse");
    //char name[strlen(data)+1];
    //strcpy(name, data);
    char *tempCopy = (char *)malloc(strlen(data) * sizeof(char *));
    char *dataCopy = strcpy(tempCopy, data);
    JsonObject &rawData = jsonBuffer.parseObject(dataCopy);
    const char *state = rawData["state"];

    if(String(state) == String("armed")) {
        armingStatus = true;
        Particle.publish("response", String("statusArmed: ") + String(state), PRIVATE);
    } else if (String(state) == String("disarmed")) {
        armingStatus = false;
        larmStatus = false;
        Particle.publish("response", String("statusArmed: ") + String(state), PRIVATE);
    } else {
        Particle.publish("response", String("Fail: ") + String(data), PRIVATE);
    }

    //long times = rawData[String("time")];

    //Particle.publish("Response", String("test") + String(rawData), PRIVATE);
    //Particle.publish("Response", String(rawData)/*String("Response satus: ") + String(status)*/, PRIVATE);
    tone(BUZZERPIN, 3000, 3);

    free(tempCopy);

    delay(150);
}






void getStateServerResponse(const char *event, const char *data) {
    // Handle the integration response
    DynamicJsonBuffer jsonBuffer;



    //char name[strlen(data)+1];
    //strcpy(name, data);
    Serial.println("getState Reponse");
    char *tempCopy = (char *)malloc(strlen(data) * sizeof(char *));
    char *dataCopy = strcpy(tempCopy, data);
    JsonObject &rawData = jsonBuffer.parseObject(dataCopy);
    const char *state = rawData["state"];

    if(String(state) == String("armed")) {
        armingStatus = true;
        Particle.publish("response", String("getState: ") + String(state), PRIVATE);
    } else if (String(state) == String("disarmed")) {
        armingStatus = false;
        larmStatus = false;
        Particle.publish("response", String("getState: ") + String(state), PRIVATE);
    } else {
        Particle.publish("response", String("Fail: ") + String(data), PRIVATE);
    }

    //long times = rawData[String("time")];

    //Particle.publish("Response", String("test") + String(rawData), PRIVATE);
    //Particle.publish("Response", String(rawData)/*String("Response satus: ") + String(status)*/, PRIVATE);
    tone(BUZZERPIN, 3000, 3);

    free(tempCopy);

    delay(150);
}







 // EXPERIMENTAL
 /*
 val = 0;
 String data = "data-set: ";
 for (int i = 0; i < 100; i += 1) {
     int rval = analogRead(MOTIONPIN2);
     //if (rval > val) {
     data = data + String(rval) + String(", ");
         val = val + rval;
     //}
     delay(10);
 }
 String toti = String(val/100) + data;
 Particle.publish("Test", toti, PRIVATE);
 delay(1000);
 */





 /*
 val = 0;
 for (int i = 0; i < 100; i += 1) {
     int rval = analogRead(MOTIONPIN2);
     if (rval > val) {
         val = rval;
     }
     delay(10);
 }
 Particle.publish("Test", String(val), PRIVATE);
 delay(1000);
 */
