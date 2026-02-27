# Welcome to the PetTriServe Project! 🐾

Have you ever worried about your pets getting hungry while you're at school or out with friends? **PetTriServe** is here to fix that!

PetTriServe is a smart, automatic pet feeding system. It acts like a robot butler for your pets, making sure up to **three different bowls** are filled with food at the exact times you choose.

---

## 🧐 What does it do?

1. **Knows the Time:** It connects to the internet to check the exact global time. It never runs fast or slow.
2. **Follows Your Rules:** You can tell it: _"Feed Fluffy every morning at 7:00 AM for 3 seconds."_ It remembers these rules even if the power goes out!
3. **Controls the Food:** When it's feeding time, the robot opens a small door (controlled by a tiny motor) to drop the food into the bowl, then closes the door quickly.
4. **Has a Cool Touchscreen:** It has a colorful touchscreen right on the front that shows you the current time and counts down to the next meal for each pet.

---

## 🧩 How does it work? (The Simple Version)

Imagine the project has two main parts: a **Brain** and some **Muscles**.

### 1. The Brain (The Screen & Wi-Fi Part)

The "brain" of our project is a small, cheap computer with a built-in touchscreen (sometimes called a "Cheap Yellow Display").

- **Job 1:** It connects to your home Wi-Fi to get the real time.
- **Job 2:** It shows the colorful buttons and timers on the screen.
- **Job 3:** It keeps an eye on the clock. When it’s 7:00 AM, the brain "shouts" a command: _"Hey Muscles! Open door number 1 now!"_

### 2. The Muscles (The Servo Motors)

The "muscles" are little motors called **Servos**. We have three of them, one for each food bowl.

- **The Follower:** The muscles are controlled by a second, simpler computer board whose _only job_ is to listen to the Brain.
- **The Action:** When the Brain shouts _"Open door number 1!"_, the Muscles turn the little motor to open the food chute. When the Brain says _"Close it!"_, the motor spins back and stops the food from falling.

---

## 📱 How do you set it up?

It's actually super easy—just like setting up a new smart TV or a smart bulb!

1.  **Turn it on:** Plug the PetTriServe into USB power.
2.  **Connect with your Phone:** If it hasn't connected to your home Wi-Fi before, it will create its own temporary Wi-Fi network called **"Portal32"**.
3.  **Open the Webpage:** Use your phone to connect to the "Portal32" Wi-Fi. A screen will pop up on your phone automatically.
4.  **Tell it Secrets:** On that screen, type in your home Wi-Fi name and password so it can get onto the internet.
5.  **Set the Schedule:** On the exact same page, pick what time each bowl should dispense food, how often, and how long the door should stay open.
6.  **Save and Relax:** Press save! The device will reboot, connect to your home internet, find the time, and start working on its own.

---

## 💡 Why did we build it this way?

You might wonder why we use two different computer boards (one for the Brain, one for the Muscles) instead of just one.

Think about when you are playing a really fast video game on your phone, and suddenly you get a phone call. The game might freeze or glitch for a second.

Our "Brain" computer is very busy drawing pictures on the touchscreen and talking to the Wi-Fi. If it was also trying to carefully control the moving motors at the exact same time, the motors might jitter or glitch! By giving the motor controls to a separate "Muscle" computer, everything runs perfectly smooth.

**Happy Feeding! 🐶🐱🐰**
