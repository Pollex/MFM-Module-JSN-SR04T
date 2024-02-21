#if !defined(_BOARD_H_)
#define _BOARD_H_

//
// PIN CONFIG
//
#define SENSOR_5VEN_PORT PORTA
#define SENSOR_5VEN_PIN PIN4_bp

// PORT A
#define SMBALERT_Port PORTA
#define SMBALERT_Pin PIN6_bp
#define OW_PORT PORTA
#define OW_PIN PIN7_bp

// PORT B
#define I2C_Port PORTB
#define I2C_SCL_Pin PIN0_bp
#define I2C_SDA_Pin PIN1_bp

#endif // _BOARD_H_
