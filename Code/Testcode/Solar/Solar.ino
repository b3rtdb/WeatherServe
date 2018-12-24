#include <BQ25895.h>

// Instantiate BQ25895 object
TI_bq25895 bq25895;


void setup() {
  bq25895.init();
}

void loop() {
  bq25895.set_MPPT();
}
