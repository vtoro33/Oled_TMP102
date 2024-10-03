
/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "I2C.h"
#include "ThisThread.h"
#include "mbed.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include <cstring>

#define tiempo_muestreo 1s
#define TMP102_ADDRESS 0x90

// Pines y puertos
BufferedSerial serial(USBTX, USBRX);
I2C i2c(D14, D15);
Adafruit_SSD1306_I2c oled(i2c, D0);
AnalogIn ain(A0);

// Variables globales
float Vin = 0.0;
int ent = 0;
int dec = 0;
char men[40];
char comando[3] = {0x01, 0x60, 0xA0};
char data[2];
const char *mensaje_inicio = "Arranque del programa\n\r";

// Prototipos 
void arranque_programa ();
void m_inicio();
void limpiar_pantalla();

void leer_voltaje();
void mostrar_voltaje();
void leer_temp();
void mostrar_temp(float temp);


int main()
{
    arranque_programa();
    limpiar_pantalla();
    m_inicio();

    // Arranque del sensor de temperatura
    i2c.write(TMP102_ADDRESS, comando, 3);  

    while (true) {
        leer_voltaje();
        mostrar_voltaje();
        leer_temp();
        ThisThread::sleep_for(tiempo_muestreo);
    }
}

// Función para arrancar el programa 
void arranque_programa() 
{
    oled.begin();
    oled.setTextSize(1);
    oled.setTextColor(1);
    oled.display();
    ThisThread::sleep_for(3000ms);
}

// Función para limpiar el display
void limpiar_pantalla()
{
    oled.clearDisplay();
    oled.display();
    oled.printf("Test\r\n");
    oled.display();
}

// Función para enviar un mensaje de inicio por el puerto serial
void m_inicio() 
{
    serial.write(mensaje_inicio, strlen(mensaje_inicio));
}

// Función para leer el voltaje del sensor de temperatura 
void leer_voltaje() {
    Vin = ain * 3.3;
    ent = int(Vin);
    dec = int((Vin - ent) * 10000);
}

// Función para mostrar el voltaje 
void mostrar_voltaje()
 {
    oled.clearDisplay();
    oled.display();
    sprintf(men, "El voltaje es:\n\r %01u.%04u volts \n\r", ent, dec);
    oled.setTextCursor(0, 2);
    oled.printf(men);
    oled.display();
    serial.write(men, strlen(men));
}

// Función para leer la temperatura desde el sensor I2C
void leer_temp()
 {
    comando[0] = 0;  // Registro de temperatura
    i2c.write(TMP102_ADDRESS, comando, 1);  // Enviar comando para leer
    i2c.read(TMP102_ADDRESS, data, 2);      // Leer 2 bytes
    int16_t temp = (data[0] << 4) | (data[1] >> 4);
    float temperatura = temp * 0.0625;
    mostrar_temp(temp);
}

// Función para mostrar la temperatura 
void mostrar_temp(float temp) 
{
    ent = int(temp);
    dec = int((temp - ent) * 10000);
    oled.clearDisplay();
    oled.display();
    sprintf(men, "La Temperatura es:\n\r %01u.%04u Celsius\n\r", ent, dec);
    oled.setTextCursor(0, 2);
    oled.printf(men);
    oled.display();
    serial.write(men, strlen(men));
}
