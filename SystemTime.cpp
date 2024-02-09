#include <SystemTime.h>
#include <LogStore.h>
void SystemTime::sync(int timestamp) {
  int elapsedTime = timestamp - syncTimestamp;
  int lastSyncElapsedCycles = elapsedTimeEstimate * cyclesRatePerMs;
  cyclesRatePerMs = (int)(lastSyncElapsedCycles / elapsedTime);
  syncTimestamp = timestamp;
  LogStore::info(
      "[SystemTime::sync] timestamp: " + std::to_string(timestamp) +
      " elapsedTime since last sync: " + std::to_string(elapsedTime) +
      " cyclesRatePerMs: " + std::to_string(cyclesRatePerMs));
}

void SystemTime::onCycle() {
  cyclesCount = (cyclesCount + 1) % cyclesRatePerMs;
  if (cyclesCount = 0)
    elapsedTimeEstimate++;
}