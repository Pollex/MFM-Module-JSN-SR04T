#if !defined(_BOARD_H_)
#define _BOARD_H_

//
// PIN CONFIG
//
#define SENSOR_5VEN_PORT PORTA
#define SENSOR_5VEN_PIN PIN4_bp
#define SENSOR_3V3EN_PORT PORTA
#define SENSOR_3V3EN_PIN PIN5_bp

// PORT A
#define MCU_DBG_Port PORTA
#define MCU_RX_DBG_Pin PIN2_bp
#define MCU_TX_DBG_Pin PIN3_bp
#define SMBALERT_Port PORTA
#define SMBALERT_Pin PIN6_bp
#define ONEWIRE_Port PORTA
#define ONEWIRE_Pin PIN7_bp

// PORT B
#define I2C_Port PORTB
#define I2C_SCL_Pin PIN0_bp
#define I2C_SDA_Pin PIN1_bp

#endif // _BOARD_H_
