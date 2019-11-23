#pragma once

#define SENSOR
#define SENSOR_1 0x9f
#define SENSOR_2 0xd9

#define SERVER
#define SERVER_1 0x9c
#define SERVER_2 0x75

#define IS_ADDRESS(var, addr) (((var).u8[0] == addr##_1) && ((var).u8[1] == addr##_2))

#define SET_ADDRESS(var, addr) \
  (var).u8[0] = addr##_1;      \
  (var).u8[1] = addr##_2;

// Structure
struct temp_msg {
  double mv;
  double temp;
};
