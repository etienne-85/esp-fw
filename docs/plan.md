## 🧠 Context Summary for AI Code Assistant

The user is preparing for highly technical C++ interviews (specifically for **think-cell**) while working on a real **embedded C++ project**. Their goal is to identify and work on **coding tasks within the project** that will both:

User is re-familiarizing himself with C++ after some time away from the language

---

## 🎯 User’s Goal

Use the existing **embedded C++ project** to:

* ✅ Strengthen their **C++ problem-solving, systems design, and low-level programming skills** relevant to interviews (esp. think-cell style).
* ✅ Add real value to the project in parallel, improve the quality, performance, or structure of their embedded code

---

## 💡 What the Assistant Should Do

* Analyze the embedded codebase to suggest **real, meaningful coding tasks** that will also benefit interview prep.
* Prioritize parts of the codebase that can be rewritten or extended in a way that reflects **interview-level C++ reasoning**.
* Optionally simulate **interview prompts** using modules already in the codebase.
* Taper guidance to help the user regain fluency and confidence in C++.

---

## ✅ Example Task Areas

| Area              | Example Task                                      | Why It Helps                         |
| ----------------- | ------------------------------------------------- | ------------------------------------ |
| Custom Containers | Implement `StaticVector<T, N>` or `RingBuffer<T>` | Templates, memory layout, bounds     |
| Memory Management | Build a small object allocator or arena           | Ownership, allocation, fragmentation |
| Protocol Parsing  | Binary protocol or CLI parser                     | Bit manipulation, endian handling    |
| State Machines    | FSMs for device modes, error states               | Enums, clean transition logic        |
| Scheduler / Timer | Lightweight cooperative scheduler or event queue  | Timers, ISR-safe code                |
| Logging           | Ring buffer logging with compile-time filters     | Templates, embedded constraints      |

---

## ✅ What the User Already Has

* A partial hash table implementation from scratch (STL-free), used as a warm-up.
* Rebuilding fluency in C++ after a multi-year gap.
* Strong embedded engineering background and a real, ongoing codebase.

---

## 🤝 Collaboration Style

* Provide assistance with more support initially (scaffolding, hints)
* Decrease assistance gradually as the user regains comfort
* Offer quick reviews, corner case analysis, or test input design when needed
