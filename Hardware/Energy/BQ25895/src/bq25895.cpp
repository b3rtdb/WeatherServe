/* My comments */

#include "bq25895.h"
#include <Wire.h>

// Bq25895 object
bq25895::bq25895() {

}

// Function to intialise
void bq25895::init() {
  Wire.begin();
  disable_CHG_WDT();			//Disable charger watchdog timer
	set_IINDPM(3.000);      //Set the IINDPM setting to maximum
	set_ICHG(5.056);        //Set the ICHG setting to maximum

}

/*****************************************************************/
/************* Reads back the charger register "reg" *************/
/*****************************************************************/
uint16_t bq25895::read_Register(int regAddress) {
  byte data = 0;
  Wire.beginTransmission(0x6a);
  Wire.write(regAddress);
  Wire.endTransmission();
  Wire.requestFrom(0x6a, 1);
  data = Wire.read();
  return (uint16_t) data;

}

/*****************************************************************/
/*****8******* Writes to the charger register "reg" *****8********/
/*****************************************************************/
void bq25895::write_Register(int regAddress, byte data) {
  Wire.beginTransmission(0x6a);
  Wire.write(regAddress);
  Wire.write(data);
  Wire.endTransmission();

}

/*****************************************************************/
/************* Disables the charger's watchdog timer *************/
/*****************************************************************/
void bq25895::disable_CHG_WDT() {
    unsigned char reg = read_Register( WDT_REGISTER );
    write_Register( WDT_REGISTER, (reg & ~(WATCHDOG)) );

}

/*****************************************************************/
/************ Sets the charger's input current limit *************/
/*****************************************************************/
void bq25895::set_IINDPM(float IINDPM_setting){
    unsigned char reg = read_Register( IINDPM_REGISTER );
    reg &= ~(IINLIM);
    unsigned char iindpm = (char)((int)((IINDPM_setting - IINDPM_OFFSET)/IINDPM_SIZE));
    iindpm |= reg;
    write_Register( IINDPM_REGISTER, iindpm );

}

/*****************************************************************/
/*********** Sets the charger's charging current limit ***********/
/*****************************************************************/
void bq25895::set_ICHG(float ICHG_setting){
    unsigned char reg = read_Register( ICHG_REGISTER );
    reg &= ~(ICHG);
    unsigned char ichg = (char)((int)( ( ICHG_setting - ICHG_OFFSET ) / ICHG_SIZE ) );
    ichg |= reg;
    write_Register( ICHG_REGISTER, ichg );

}

/*****************************************************************/
/************* Starts the charger's ADC conversion. **************/
/*************** Defaults to burst mode operation. ***************/
/*****************************************************************/
void bq25895::startADC() {
    unsigned char reg = read_Register( ADC_REGISTER );
    reg |= CONV_START;
    write_Register( ADC_REGISTER, reg );

}

/*****************************************************************/
/*****8**** Returns state of the input source capability *********/
/*****************************************************************/
_Bool bq25895::get_PGSTAT(){
    unsigned char reg = read_Register( STAT_REGISTER );
    reg &= PG_STAT;

    return reg;
}

/*****************************************************************/
/************ Enables/disables the charger's HIZ Mode ************/
/*****************************************************************/
void bq25895::set_HIZ(_Bool setting){
    unsigned char reg = read_Register( HIZ_REGISTER );

    if (setting == ENABLE){
        reg |= EN_HIZ;
        write_Register( HIZ_REGISTER, reg );

    }

    else {
        reg &= ~(EN_HIZ);
        write_Register( HIZ_REGISTER, reg );

    }
}

/*****************************************************************/
/**** Returns the charger's ADC reading of the input voltage  ****/
/*****************************************************************/
unsigned char bq25895::get_VBUS(){
    int i;
    unsigned char reg;
    for (i=0; i<3; i++){
        startADC();

    }
    delay(1);
    reg = read_Register( VBUS_REGISTER );


    unsigned char vbus = (reg & VBUSV);

    return vbus;

}

/*****************************************************************/
/**** Sets the operating voltage through the charger's VINDPM ****/
/*****************************************************************/
void bq25895::set_VINDPM(char VINDPM_setting){
    unsigned char vindpm = VINDPM_setting;
    vindpm |= FORCE_VINDPM;
    write_Register( VINDPM_REGISTER, vindpm );

}

/*****************************************************************/
/*** Returns the charger's ADC reading of the charging current  **/
/*****************************************************************/
unsigned char bq25895::get_ICHGR(){
    int i;
    unsigned char reg;
    for (i=0; i<3; i++){
        startADC();
    }
    delay(1);
    reg = read_Register( ICHGR_REGISTER );


    unsigned char ichg = reg;

    return ichg;

}

/*****************************************************************/
/*********** MPPT Routine, set most ideal point  *****************/
/*****************************************************************/
void bq25895::set_MPPT() {
  if (get_PGSTAT()){


/******************************************************************************************************/
/*************************************** Preconditioning Routine **************************************/
/******************************************************************************************************/
    set_HIZ(ENABLE);
    unsigned char voc = get_VBUS();         //Reads and stores the ADC measurement of open circuit voltage
    set_HIZ(DISABLE);

    unsigned char voc_low = VOC_LOW*voc/100 - (100-VOC_LOW)*VBUSV_OFFSET/100;   //Sets lower VINDPM limit

    if (voc_low < 0X0D)                     //Clamps lower bound to 3.9V charger setting
            voc_low = 0X0D;

    unsigned char voc_high = VOC_HIGH*voc/100 - (100-VOC_HIGH)*VBUSV_OFFSET/100; //Sets upper VINDPM limit
    unsigned char ichg_max = 0X0;
    unsigned char vindpm_max = 0X0;

    unsigned char i=0;
    int count=0;


/******************************************************************************************************/
/****************************************** Tracking Routine ******************************************/
/******************************************************************************************************/
    for (i=voc_low ; i<voc_high ; i+=0X01){ //Tracks from the lower to the upper VINDPM limits
        set_VINDPM(i);

        unsigned char ichgr = get_ICHGR();  //Reads and stores the ADC charging current measurement

        if (ichgr > ichg_max){              //Condition to record the current VINDPM @ the max ICHG
            count = 0;
            ichg_max = ichgr;
            vindpm_max = i;

        }

        else if (ichgr == ichg_max){        //Condition to account for the same ICHG ADC readings
            if (i > vindpm_max){
                vindpm_max = i;
                count++;

            }
        }

        else {
            i = voc_high;                   //Optional Tracking Time Optimizer. Uncomment to set

        }
    }

    vindpm_max -= (count/2)*0X01;           //Calculates midpoint. Can be changed different P-V curves
    set_VINDPM( vindpm_max );               //Sets VINDPM operating point to the MPP voltage found

  }
}