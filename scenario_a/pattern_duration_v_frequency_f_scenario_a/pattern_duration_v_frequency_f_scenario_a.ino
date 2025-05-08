// === HARDWARE CONFIGURATION ===
int buttonPins[8] = {43, 39, 35, 31, 52, 50, 46, 42};
int motorPins[8] = {12, 11, 10, 9, 8, 7, 6, 5};

unsigned long motorTimers[8] = {0};
int pulseDurations[8]; // Duration of each pulse in milliseconds

//scenario A
int dataValues[8] = {0, 30, 50, 70, 90, 100, 20, 40}; // Input data (0–100 scale)

bool previousButtonStates[8] = {true};

// === EXPERIMENT CONFIGURATION ===
int currentGroup = 0;
int currentTrial = 0;
const int TOTAL_GROUPS = 3;
const int TRIALS_PER_GROUP = 3;
int pauseBetweenPulses = 100;

// === SETUP ===
void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 8; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    pinMode(motorPins[i], OUTPUT);
    previousButtonStates[i] = true;

    // If the data value is 0, skip vibration by assigning 0 duration
    if (dataValues[i] == 0) {
      pulseDurations[i] = 0;
    } else {
      pulseDurations[i] = map(dataValues[i], 1, 100, 100, 800);
    }
  }
  randomSeed(analogRead(0));
}

// === HELPER FUNCTIONS ===
int getPWMIntensity(int freq) {
  return map(freq, 40, 150, 68, 255);
}

void runPattern(int motorPin, unsigned long &lastTime, unsigned long totalCycle, int pwmValue) {
  if (totalCycle == 0) {
    analogWrite(motorPin, 0);
    return;
  }
  unsigned long elapsed = millis() - lastTime;
  if (elapsed < totalCycle) {
    analogWrite(motorPin, pwmValue);
  } else if (elapsed < totalCycle + pauseBetweenPulses) {
    analogWrite(motorPin, 0);
  } else {
    lastTime = millis();
  }
}

void shuffleArray(int source[], int target[], int size) {
  for (int i = 0; i < size; i++) target[i] = source[i];
  for (int i = 0; i < size; i++) {
    int j = random(i, size);
    int temp = target[i];
    target[i] = target[j];
    target[j] = temp;
  }
}

void pauseBetweenTasks() {
  for (int i = 0; i < 8; i++) analogWrite(motorPins[i], 0);
  Serial.println("\n--- Pause between tasks ---");
  Serial.println("Press any button to continue...");
  bool waiting = true;
  while (waiting) {
    for (int i = 0; i < 8; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        delay(500);
        waiting = false;
        break;
      }
    }
  }
}

// === TASK 1: Identify Shortest Duration ===
void runTask_IdentifyShortest() {
  int randomized[8];
  shuffleArray(pulseDurations, randomized, 8);
  for (int i = 0; i < 8; i++) motorTimers[i] = millis();

  Serial.println("\nTask: Identify Shortest");
  Serial.print("Trial: ");
  Serial.println(currentTrial + 1);
  Serial.print("Durations: ");
  for (int i = 0; i < 8; i++) {
    Serial.print(randomized[i]); if (i < 7) Serial.print(", ");
  }
  Serial.println();

  bool answered = false;
  unsigned long startTime = millis();

  while (!answered) {
    for (int i = 0; i < 8; i++) {
      runPattern(motorPins[i], motorTimers[i], randomized[i], getPWMIntensity(75));
      bool current = digitalRead(buttonPins[i]) == LOW;
      if (previousButtonStates[i] && current) {
        unsigned long responseTime = millis() - startTime;
        int selected = randomized[i];
        int minVal = 9999;
        for (int j = 0; j < 8; j++) if (randomized[j] > 0 && randomized[j] < minVal) minVal = randomized[j];

        Serial.print("Response time: "); Serial.print(responseTime); Serial.println(" ms");
        Serial.print("Button: "); Serial.println(i);
        Serial.print("Selected duration: "); Serial.println(selected);
        Serial.print("Correct: "); Serial.println(selected == minVal ? "yes" : "no");

        answered = true;
        delay(1000);
        break;
      }
      previousButtonStates[i] = !current;
    }
  }

  currentTrial++;
  if (currentTrial >= TRIALS_PER_GROUP) {
    currentTrial = 0;
    currentGroup++;
    if (currentGroup < TOTAL_GROUPS) pauseBetweenTasks();
  }
}

// === TASK 2: Identify Longest Duration ===
void runTask_IdentifyLongest() {
  int randomized[8];
  shuffleArray(pulseDurations, randomized, 8);
  for (int i = 0; i < 8; i++) motorTimers[i] = millis();

  Serial.println("\nTask: Identify Longest");
  Serial.print("Trial: ");
  Serial.println(currentTrial + 1);
  Serial.print("Durations: ");
  for (int i = 0; i < 8; i++) {
    Serial.print(randomized[i]); if (i < 7) Serial.print(", ");
  }
  Serial.println();

  bool answered = false;
  unsigned long startTime = millis();

  while (!answered) {
    for (int i = 0; i < 8; i++) {
      runPattern(motorPins[i], motorTimers[i], randomized[i], getPWMIntensity(75));
      bool current = digitalRead(buttonPins[i]) == LOW;
      if (previousButtonStates[i] && current) {
        unsigned long responseTime = millis() - startTime;
        int selected = randomized[i];
        int maxVal = 0;
        for (int j = 0; j < 8; j++) if (randomized[j] > maxVal) maxVal = randomized[j];

        Serial.print("Response time: "); Serial.print(responseTime); Serial.println(" ms");
        Serial.print("Button: "); Serial.println(i);
        Serial.print("Selected duration: "); Serial.println(selected);
        Serial.print("Correct: "); Serial.println(selected == maxVal ? "yes" : "no");

        answered = true;
        delay(1000);
        break;
      }
      previousButtonStates[i] = !current;
    }
  }

  currentTrial++;
  if (currentTrial >= TRIALS_PER_GROUP) {
    currentTrial = 0;
    currentGroup++;
    if (currentGroup < TOTAL_GROUPS) pauseBetweenTasks();
  }
}

// === TASK 3: Sort by Swapping Motors ===
void runTask_SortBySwapping() {
  int assigned[8];
  shuffleArray(pulseDurations, assigned, 8);
  for (int i = 0; i < 8; i++) motorTimers[i] = millis();

  Serial.println("\nTask: Sort by Swapping");
  Serial.print("Trial: ");
  Serial.println(currentTrial + 1);
  Serial.print("Initial order: ");
  for (int i = 0; i < 8; i++) {
    Serial.print(assigned[i]); if (i < 7) Serial.print(", ");
  }
  Serial.println();

  int firstClick = -1;
  int lastButton = -1;
  bool waitingSecond = false;
  bool finished = false;

  while (!finished) {
    for (int i = 0; i < 8; i++) {
      runPattern(motorPins[i], motorTimers[i], assigned[i], getPWMIntensity(75));
      bool current = digitalRead(buttonPins[i]) == LOW;
      if (previousButtonStates[i] && current) {
        if (waitingSecond && i == firstClick && i == 7 && lastButton == 7) {
          Serial.println("Double-click on button 7 — Task complete");
          finished = true;
          break;
        } else if (waitingSecond && i != firstClick) {
          int tmp = assigned[firstClick];
          assigned[firstClick] = assigned[i];
          assigned[i] = tmp;

          Serial.print("Swapped: "); Serial.print(firstClick); Serial.print(" <-> "); Serial.println(i);
          Serial.print("New order: ");
          for (int j = 0; j < 8; j++) {
            Serial.print(assigned[j]); if (j < 7) Serial.print(", ");
          }
          Serial.println();

          waitingSecond = false;
          firstClick = -1;
        } else {
          Serial.print("Clicked: button "); Serial.println(i);
          firstClick = i;
          waitingSecond = true;
        }
        lastButton = i;
        delay(300);
        break;
      }
      previousButtonStates[i] = !current;
    }
  }

  // Evaluate order
  int correct[8];
  for (int i = 0; i < 8; i++) correct[i] = pulseDurations[i];
  for (int i = 0; i < 7; i++)
    for (int j = i + 1; j < 8; j++)
      if (correct[i] > correct[j]) {
        int t = correct[i]; correct[i] = correct[j]; correct[j] = t;
      }

  int hits = 0;
  for (int i = 0; i < 8; i++) {
    if (assigned[i] == correct[i]) hits++;
  }

  Serial.print("Correct positions: "); Serial.println(hits);
  Serial.print("Sorted correctly: "); Serial.println(hits == 8 ? "yes" : "no");

  delay(1000);
  currentTrial++;
  if (currentTrial >= TRIALS_PER_GROUP) {
    currentTrial = 0;
    currentGroup++;
    if (currentGroup < TOTAL_GROUPS) pauseBetweenTasks();
  }
}

// === MAIN LOOP ===
void loop() {
  if (currentGroup < TOTAL_GROUPS) {
    switch (currentGroup) {
      case 0: runTask_IdentifyShortest(); break;
      case 1: runTask_IdentifyLongest(); break;
      case 2: runTask_SortBySwapping(); break;
    }
  } else {
    Serial.println("\n--- Experiment complete! ---");
    while (true); // Stops the experiment
  }
}
