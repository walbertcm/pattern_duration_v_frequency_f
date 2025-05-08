// === HARDWARE CONFIGURATION ===
int buttonPins[8] = {43, 39, 35, 31, 52, 50, 46, 42};
int motorPins[8] = {12, 11, 10, 9, 8, 7, 6, 5};

unsigned long motorTimers[8] = {0};
int pulseDurations[8]; // Output durations per motor

// Fixed discrete vibration durations
int vibrationDurations[8] = {100, 200, 300, 400, 500, 600, 700, 800};

// Data for Scenario A (no zeros) and Scenario B (with a zero)
int dataValues_A[8] = {10, 30, 50, 70, 90, 100, 20, 40};
int dataValues_B[8] = {10, 0, 25, 50, 75, 100, 90, 60};

bool previousButtonStates[8] = {true};

// === EXPERIMENT CONFIGURATION ===
int currentScenario = 0; // 0 = A, 1 = B
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
  }
  randomSeed(analogRead(0));
  setupScenario(currentScenario);
}

// === MAPPING SCENARIO TO DISCRETE DURATIONS ===
void setupScenario(int scenario) {
  for (int i = 0; i < 8; i++) {
    int value = (scenario == 0) ? dataValues_A[i] : dataValues_B[i];
    if (value == 0) {
      pulseDurations[i] = 0; // No vibration
    } else {
      int index = map(value, 1, 100, 0, 7);
      index = constrain(index, 0, 7);
      pulseDurations[i] = vibrationDurations[index];
    }
  }
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

  Serial.println("Task: Identify Shortest Duration");
  Serial.print("Trial: "); Serial.println(currentTrial + 1);
  Serial.print("Order: ");
  for (int i = 0; i < 8; i++) {
    Serial.print(randomized[i]);
    if (i < 7) Serial.print(", ");
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
        Serial.print("Button pressed: "); Serial.println(i);
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

  Serial.println("Task: Identify Longest Duration");
  Serial.print("Trial: "); Serial.println(currentTrial + 1);
  Serial.print("Order: ");
  for (int i = 0; i < 8; i++) {
    Serial.print(randomized[i]);
    if (i < 7) Serial.print(", ");
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
        Serial.print("Button pressed: "); Serial.println(i);
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

void runTask_SortBySwapping() {
  int assigned[8];
  shuffleArray(pulseDurations, assigned, 8);
  for (int i = 0; i < 8; i++) motorTimers[i] = millis();

  Serial.println("Task: Sort Values");
  Serial.print("Trial: "); Serial.println(currentTrial + 1);
  Serial.print("Initial order: ");
  for (int i = 0; i < 8; i++) {
    Serial.print(assigned[i]);
    if (i < 7) Serial.print(", ");
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
          Serial.println("\n(Participant presses button 7 twice to finish)");
          Serial.println("\nTask completed!");
          finished = true;
          break;
        } else if (waitingSecond && i != firstClick) {
          Serial.print("Swap recorded: Button "); Serial.print(firstClick);
          Serial.print(" ↔ Button "); Serial.println(i);

          int tmp = assigned[firstClick];
          assigned[firstClick] = assigned[i];
          assigned[i] = tmp;

          Serial.print("New order:     ");
          for (int j = 0; j < 8; j++) {
            Serial.print(assigned[j]);
            if (j < 7) Serial.print(", ");
          }
          Serial.println();

          waitingSecond = false;
          firstClick = -1;
        } else {
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

  // Evaluate correctness
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

  Serial.print("Final order:   ");
  for (int i = 0; i < 8; i++) {
    Serial.print(assigned[i]);
    if (i < 7) Serial.print(", ");
  }
  Serial.println();

  Serial.print("Correct order: ");
  for (int i = 0; i < 8; i++) {
    Serial.print(correct[i]);
    if (i < 7) Serial.print(", ");
  }
  Serial.println();

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
      case 2: runTask_SortBySwapping(); break; // Você pode adicionar a função aqui
    }
  } else if (currentScenario == 0) {
    currentScenario = 1;
    currentGroup = 0;
    currentTrial = 0;
    Serial.println("\n=== Starting Scenario B ===");
    setupScenario(currentScenario);
    pauseBetweenTasks();
  } else {
    Serial.println("\n--- Experiment complete! ---");
    while (true);
  }
}

