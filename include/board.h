#if !defined(_BOARD_H_)
#define _BOARD_H_

//
// PIN CONFIG
//
#define ENABLE_5V_PORT PORTA
#define ENABLE_5V_PIN PIN5_bm
#define ENABLE_3V3_PORT PORTA
#define ENABLE_3V3_PIN PIN2_bm

// PORT A
#define SMBALERT_Port PORTA
#define SMBALERT_Pin PIN6_bp
#define DS18B20_PORT PORTA
#define DS18B20_PIN PIN3_bp

// PORT B
#define I2C_Port PORTB
#define I2C_SCL_Pin PIN0_bp
#define I2C_SDA_Pin PIN1_bp

#endif // _BOARD_H_
