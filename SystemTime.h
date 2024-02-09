#pragma once

class SystemTime {
  int firstTimestamp; // first timestamp received since boot
  int syncTimestamp;
  int cyclesRatePerMs; // updated at each sync
  int cyclesCount;     // since last ms
public:
  int elapsedTimeEstimate; // estimated elapsed time since last sync
  void sync(int timestamp);
  void onCycle();
};
