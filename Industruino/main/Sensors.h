#ifndef SENSORS_H
#define SENSORS_H

//////////////////// Port information ///////////////////
#define baud         9600
#define timeout_time 1000
#define polling      20
#define retry_count  10

// Used to toggle the receive/transmit pin on the driver
#define TxEnablePin 9

// Sensors IDs
#define Sensor1_ID  1
#define Sensor2_ID  2
#define Sensor3_ID  3

// The total amount of available memory on the master to store data
#define TOTAL_NO_OF_REGISTERS 8

enum {
  PACKET1,
  PACKET2,
  PACKET3,
  TOTAL_NO_OF_PACKETS
};

Packet packets[TOTAL_NO_OF_PACKETS];
unsigned int unsignedData[TOTAL_NO_OF_REGISTERS];
int data[TOTAL_NO_OF_REGISTERS];

#endif
