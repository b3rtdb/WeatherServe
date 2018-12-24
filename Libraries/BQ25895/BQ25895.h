/* My comments */

#ifndef _LIB_TI_BQ25895_
#define _LIB_TI_BQ25895_

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>

#define ENABLE          true
#define DISABLE         false

#define VOC_LOW         (65)
#define VOC_HIGH        (95)
#define VBUS_MIN        (0X0D)
#define SAMPLE_TIME     (20)

#define ADDRESS         (0X6A)
#define HIZ_REGISTER    (0X00)
#define EN_HIZ          (0X80)

#define IINDPM_REGISTER (0X00)
#define IINLIM          (0X3F)
#define IINDPM_OFFSET   (0.100)
#define IINDPM_SIZE     (0.050)

#define ICHG_REGISTER   (0X04)
#define ICHG            (0X7F)
#define ICHG_OFFSET     (0.000)
#define ICHG_SIZE       (0.064)

#define ADC_REGISTER    (0X02)
#define CONV_START      (0X80)
#define CONV_RATE       (0X40)

#define WDT_REGISTER    (0X07)
#define WATCHDOG        (0X30)

#define STAT_REGISTER   (0X0B)
#define PG_STAT         (0X04)

#define VINDPM_REGISTER (0X0D)
#define FORCE_VINDPM    (0X80)
#define VINDPM          (0X7F)

#define VBAT_REGISTER   (0x0E)
#define BATV            (0X7F)

#define VSYS_REGISTER   (0X0F)
#define SYSV            (0X7F)

#define VBUS_REGISTER   (0X11)
#define VBUSV           (0X7F)
#define VBUSV_OFFSET    (0X1A)

#define ICHGR_REGISTER  (0X12)
#define ICHGR           (0X7F)


class TI_bq25895  {
  
  public:
    TI_bq25895();
    void          init();
    void          disable_CHG_WDT();
    void          set_IINDPM(float IINDPM_setting);
    void          set_ICHG(float ICHG_setting);
    void          startADC();
    _Bool         get_PGSTAT();
    void          set_HIZ(_Bool setting);
    unsigned char get_VBUS();
    void          set_VINDPM(char VINDPM_setting);
    unsigned char get_ICHGR();
    void          set_MPPT();


  private:
    uint16_t      read_Register(int regAddress);
    void          write_Register(int regAddress, byte data);

};

#endif