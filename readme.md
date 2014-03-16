This project is made of 2 program. They are Arduino sketch for Aruino UNO and Parallax Board of Education, and the Android application. The goal was to integrate sensors which detect a persons vital stats such as heart rate, temperature, blood oxygen level, blood pressure, and to transit the information to a mobile application.

Arduino

Several sensors were integrated into an Arduino UNO board using the Parallax Board of Education. Sketches were written in order to interpret the signals sent from each of these sensors to perform calculations and to transmit this data to an Android Application over Bluetooth.

Android

An Android application was created to receive the data transmitted by the Arduino application and hardware. The Android app performs calculations and provides a friendly interface to display realtime results as vital medical information textually and in graph format.