//Altitude Data
float altitudes[] = { 0, -2, 0 , -1, 0, 0, 104, 193, 287, 364, 459, 607, 746, 680, 552, 1023, 465, 433, 420, 377, 340, 295, 250, 215, 185, 850, 150, 117, 75, 38, 12, 0};
int altIndex = 0; //index for the simulaltion altitudes data that was provided

// Constants and parameters for moving average and exponential smoothing
const int windowSize = 10;  // Moving average window size
float smoothingFactor = 0.1;  // Exponential smoothing factor 
float threshold = 0.5;  // Threshold in meters to detect movement

// Array to store the altitude readings for moving average
float altitudeReadings[windowSize] = {0};  // Array to hold the altitude readings
int index = 0;  // Index for the moving average array
float total = 0;  // Sum of altitude readings for moving average

// Variables for exponential smoothing
float smoothedAltitude = 0;
bool firstReading = true;

// Altitude change threshold
float lastSmoothedAltitude = 0;  // Store the last smoothed value

// Function prototypes
float getAltitude();  // Function to get altitude from sensor (to be implemented)
void updateMovingAverage(float altitude);
float getMovingAverage();
void updateExponentialSmoothing(float altitude);
void detectMotion(float currentAltitude);

void setup() {
  Serial.begin(9600);  // Initialize serial communication
 
}

void loop() {
  // Get the current altitude
  float currentAltitude = getAltitude();

  // Update the moving average
  updateMovingAverage(currentAltitude);
  float avgAltitude = getMovingAverage();

  // Update exponential smoothing
  updateExponentialSmoothing(currentAltitude);

  // condition to check if we run out of data
  if (altIndex > sizeof(altitudes) / sizeof(float)) {
    //Serial.println("Out of data");
    delay(100);
    return ;
  }

  // Detect if the CanSat is moving up, down, or stationary
  detectMotion(smoothedAltitude);

  // printing avgAltitude so that we can also view it as a graph
  Serial.println(avgAltitude);

  // Delay to simulate data acquisition interval
  delay(100);  // Adjust as per data rate
}

// Function to get altitude 
// can be replaced with a function to get altitude from a sensor
float getAltitude() {
  float simulatedAltitude = altitudes[altIndex];
  altIndex++;
  return simulatedAltitude;
}

// Function to update moving average
void updateMovingAverage(float altitude) {
  // Subtract the oldest reading from the total
  total -= altitudeReadings[index];

  // Add the new reading to the array and the total
  altitudeReadings[index] = altitude;
  total += altitude;

  // Move to the next index in the circular buffer
  index = (index + 1) % windowSize;
}

// Function to calculate the current moving average
float getMovingAverage() {
  return total / windowSize;
}

// Function to apply exponential smoothing
void updateExponentialSmoothing(float altitude) {
  // if its the first reading 
  // initialise with the first altitude
  if (firstReading) {
    smoothedAltitude = altitude; 
    firstReading = false;
  } 
  else {
    // calculate the exponentially smoothed altitude
    smoothedAltitude = (smoothingFactor * altitude) + (1 - smoothingFactor) * smoothedAltitude;
  }
}

// Function to detect the motion of the CanSat
void detectMotion(float currentAltitude) {
  float altitudeChange = currentAltitude - lastSmoothedAltitude;

  if (abs(altitudeChange) < threshold) {
    Serial.println("The CanSat is stationary.");
  } else if (altitudeChange > 0) {
    Serial.println("The CanSat is moving up.");
  } else {
    Serial.println("The CanSat is moving down.");
  }

  // Update the last smoothed altitude for the next comparison
  lastSmoothedAltitude = currentAltitude;
}
