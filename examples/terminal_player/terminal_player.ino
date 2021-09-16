/*
  WT2003M02 Terminal Player

  By: Andrey Rybalko
  Date: 15 Sept 2021

  This example makes an MP3 player that can be controlled from the terminal window on a computer.
  It exposes the full set of commands that are available for the WT2003M02
  Don't forget to load some MP3s and plug it in too!

  Name all files with a numeric prefix (4 characters with leading zeros),
  otherwise the playFileNumber function will not work correctly. Examples of filenames:

  0001 Best song.mp3
  0002 my song.mp3
  0003 not my song.mp3
  ...
  0123 fox's voice.mp3

  I recommend that you use methods playFileNumber or playFileName because method playTrackNumber
  works depending on the order in which the files were copied to the flash drive.

  Hardware Connections:
  WT2003M02 V3.0 -> Arduino Pin
  -------------------------------------
  TXO                  ->   5
  RXI                  ->   4
  5V                   ->  5V
  GND                  -> GND

  WT2003M02 V3.0 -> WeMos D1 mini Pin
  -------------------------------------
  TXO                  ->  D1
  RXI                  ->  D2
  5V                   ->  5V
  GND                  -> GND
*/


#include "WT2003M02.h"

#define WEMOS_A0     17
#define WEMOS_D0     16
#define WEMOS_D1     5
#define WEMOS_D2     4
#define WEMOS_D3     0
#define WEMOS_D4_LED 2
#define WEMOS_D5     14
#define WEMOS_D6     12
#define WEMOS_D7     13
#define WEMOS_D8     15
#define WEMOS_TX     1
#define WEMOS_RX     3

#define SR_LEN 8
char cmd[SR_LEN];

WT2003M02 MP3;

void setup() {
    Serial.begin(9600);
    while (!Serial) {};
    MP3.begin(WEMOS_D1, WEMOS_D2);
    delay(4);

    int count = MP3.getSongCount();
    Serial.print("SongCount: ");
    Serial.println(count);

    if (count == 0) {
        Serial.println("No songs found... try adding songs and try again");
        while (1);
    }

    printMenu();
}

void loop() {
    srRead();
    if (cmd[0] == 'p') {
        MP3.pause();
    } else if (cmd[0] == 's') {
        MP3.stop();
    } else if (cmd[0] == '>') {
        MP3.next();
    } else if (cmd[0] == '<') {
        MP3.prev();
    } else if (cmd[0] == '?') {
        uint16_t songCount = MP3.getSongCount();
        Serial.print("SongCount: ");
        Serial.println(songCount);

        uint8_t volume = MP3.getVolume();
        Serial.print("Volume: ");
        Serial.println(volume);

        uint8_t playStatus = MP3.getStatus();
        Serial.print("PlayStatus: ");
        if (playStatus == MP3_STATUS_PLAY) {
            Serial.println("Play");
            uint8_t *songName = MP3.getSongName();
            Serial.print("SongName: ");
            Serial.write(songName, 8);
            Serial.println();
        } else if (playStatus == MP3_STATUS_STOP) {
            Serial.println("Stop");
        } else if (playStatus == MP3_STATUS_PAUSE) {
            Serial.println("Pause");
        } else {
            Serial.println(playStatus);
        }
    } else if (cmd[0] == 'm') {
        if (cmd[1] == 'n') {
            MP3.setPlaymodeSingleNoLoop();
        } else if (cmd[1] == 's') {
            MP3.setPlaymodeSingleLoop();
        } else if (cmd[1] == 'a') {
            MP3.setPlaymodeAllLoop();
        } else if (cmd[1] == 'r') {
            MP3.setPlaymodeRandom();
        }
    } else if (cmd[0] == '@') {
        static uint16_t n0 = 0;
        for (int i = 1; i < 5; i++) {
            for (int i = 1; i < 5; i++) {
                if (cmd[i] < '0' || cmd[i] > '9') {
                    break;
                }
                n0 = n0 * 10 + (cmd[i] - '0');
            }
        }
        MP3.playTrackNumber(n0);
    } else if (cmd[0] == '#') {
        static uint16_t n1 = 0;
        for (int i = 1; i < 5; i++) {
            if (cmd[i] < '0' || cmd[i] > '9') {
                break;
            }
            n1 = n1 * 10 + (cmd[i] - '0');
        }
        MP3.playFileNumber(n1);
    } else if (cmd[0] == '*') {
        static uint8_t fileName[4];
        fileName[0] = cmd[1];
        fileName[1] = cmd[2];
        fileName[2] = cmd[3];
        fileName[3] = cmd[4];
        MP3.playFileName(fileName);
    } else if (cmd[0] == 'v') {
        uint8_t volume = 0;
        for (int i = 1; i < 3; i++) {
            if (cmd[i] < '0' || cmd[i] > '9') {
                break;
            }
            volume = volume * 10 + (cmd[i] - '0');
        }
        MP3.setVolume(volume);
    } else if (cmd[0] == 'e') {
        if (cmd[1] == '0') {
            MP3.setEQ(MP3_EQ_MODE_NORMAL);
        }
        if (cmd[1] == '1') {
            MP3.setEQ(MP3_EQ_MODE_POP);
        }
        if (cmd[1] == '2') {
            MP3.setEQ(MP3_EQ_MODE_ROCK);
        }
        if (cmd[1] == '3') {
            MP3.setEQ(MP3_EQ_MODE_JAZZ);
        }
        if (cmd[1] == '4') {
            MP3.setEQ(MP3_EQ_MODE_CLASSIC);
        }
        if (cmd[1] == '5') {
            MP3.setEQ(MP3_EQ_MODE_BASS);
        }
    } else if (cmd[0] != '\0') {
        printMenu();
    }
    delay(20);
}

void srRead() {
    for (int i = 0; i < SR_LEN; i++) {
        cmd[i] = '\0';
    }
    int i = 0;
    while (Serial.available()) {
        cmd[i++] = Serial.read();
        if (i >= SR_LEN - 1) {
            break;
        }
        delay(4);
    }
    if (i > 0) {
        Serial.print("Your command: ");
        Serial.println(cmd);
    }
}

void printMenu() {
    Serial.println("Command List:");
    Serial.println("p     play or pause");
    Serial.println("s     stop playing");
    Serial.println(">     next song");
    Serial.println("<     last song");
    Serial.println("v??   Example: v20. Set volume 20");
    Serial.println("?     status");
    Serial.println("mn    set playmode single no loop");
    Serial.println("ms    set playmode single loop");
    Serial.println("ma    set playmode all loop");
    Serial.println("mr    set playmode random");
    Serial.println("@?    Example: @1. Play track number 1");
    Serial.println("#?    Example: #1. Play a file whose name starts with 0001");
    Serial.println("*???? Example: *0101. Play a file whose name starts with 0101");
    Serial.println("e0    Equalizer normal");
    Serial.println("e1    Equalizer pop");
    Serial.println("e2    Equalizer rock");
    Serial.println("e3    Equalizer jazz");
    Serial.println("e4    Equalizer classic");
    Serial.println("e5    Equalizer bass");
    Serial.println("-----------------");
    Serial.println("To display this menu, type any other character");
    Serial.println("-----------------");
    Serial.println("Just enter > to start playback");
    Serial.println();
}