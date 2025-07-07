// Mutual exclusion lock.
struct spinlock {
  uint locked;       // Is the lock held? 1 is held

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
#ifdef LAB_LOCK
  int nts;
  int n;
#endif
};

