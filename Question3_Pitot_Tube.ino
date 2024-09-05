// including the standard wire library for I²C communication
#include <Wire.h>

// I²C address of the MS4525DO sensor from their doc
#define MS4525_ADDRESS 0x28

// setting air density as a constant
const float airDensity = 1.225;  // Air density at sea level in kg/m³
const float maxPSI = 1.0;


// Function prototypes
float getAirspeed();
uint16_t readPressure();

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  Wire.begin();  // Start I²C communication

}



void loop() {
  float airspeed = getAirspeed();  // Get the airspeed in m/s

  // Display the airspeed
  Serial.print("Airspeed: ");
  Serial.print(airspeed);
  Serial.println(" m/s");

  delay(1000);  // Update once per second
}



// Function to get the airspeed from the sensor
float getAirspeed() {
  uint16_t rawPressure = readPressure();  // Get the raw pressure value

  if (rawPressure == 0) {
    // Return 0 if the sensor read fails
    return 0;
  }

  // Convert the raw pressure to differential pressure in Pascals
  /*
    raw pressure reading will be in the range 0-16383
    as the first half of the range represents -ve values, we subtract 8192
    then we convert the raw pressure reading to the PSI
    and finally convert the PSI reading to Pascals using 1PSI = 6894.76 Pa
  */
  float differentialPressure = (rawPressure - 8192) * (maxPSI / 16384) * 6894.76;

  // Calculate airspeed using Bernoulli's principle: v = sqrt(2 * ΔP / ρ)
  float airspeed = sqrt(2 * differentialPressure / airDensity);

  return airspeed;
}

// Function to read pressure from the MS4525DO sensor
uint16_t readPressure() {
  /*
    the raw data will come in a total of 16 bits
    the first 2 bits are the status of I2C read
    the next 14 bits are the actual pressure data
    ranging from 0 to (2^14 -1)
    the first half represents -ve pressure and the second half is for +ve pressure
  */

  uint8_t byte1, byte2;
  uint16_t rawPressure;

  // Request data from the sensor
  Wire.beginTransmission(MS4525_ADDRESS);

  // Send a dummy write to initiate the read
  Wire.write(0x00);

  // Request the 2 status bytes from the sensor
  Wire.requestFrom(MS4525_ADDRESS, 2);

  if (Wire.available() == 2) {
    byte1 = Wire.read();  // Most significant byte
    byte2 = Wire.read();  // Least significant byte


    // Mask out the status bits and then
    // Combine byte1 and byte2 into a 14-bit pressure reading
    rawPressure = ((byte1 & 0x3F) << 8) | byte2;
    return rawPressure;

  }
  else {
    // Sensor read failed
    Serial.println("Error: Failed to read from the sensor.");
    return 0;
  }

  Wire.endTransmission();

}
