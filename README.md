# Real-Time Stress Detection and Relaxation Feedback System using EMG Sensor and Machine Learning

## 🧠 Project Overview

The **Real-Time Stress Detection and Relaxation Feedback System** is an IoT and Machine Learning-based healthcare project designed to monitor a user's physiological signals, detect stress levels, and provide personalized relaxation recommendations.

The system uses an **Electromyography (EMG) sensor** to measure muscle activity and a **temperature sensor** to monitor physiological changes. An ESP32 microcontroller collects sensor readings and transmits the data through Wi-Fi to a local database.

A Machine Learning model based on the **Decision Tree algorithm** analyzes the collected data and classifies stress into three categories:

- Normal
- Moderate Stress
- High Stress

A web-based dashboard displays real-time sensor readings and stress classifications. An AI-powered chatbot provides personalized relaxation suggestions, including breathing exercises, guided meditation, and motivational support.

## ✨ Key Features

- **Real-Time Stress Detection:** Continuously monitors physiological signals.
- **EMG-Based Monitoring:** Measures muscle activity and tension.
- **Temperature Monitoring:** Tracks physiological temperature variations.
- **Machine Learning Classification:** Uses a Decision Tree algorithm to identify stress levels.
- **IoT Integration:** Transfers sensor data wirelessly using ESP32 Wi-Fi.
- **Interactive Web Dashboard:** Displays real-time sensor readings and stress status.
- **AI-Powered Chatbot:** Provides personalized relaxation guidance.
- **Local Database:** Stores and retrieves physiological monitoring records.
- **Personalized Feedback:** Recommends relaxation techniques based on detected stress levels.

## 🛠️ Technologies Used

| Category | Technologies |
|---|---|
| Microcontroller | ESP32 DevKit V1 |
| Physiological Sensor | EMG Sensor |
| Temperature Sensor | LM35 / DHT22 |
| Signal Conditioning | AD8232 / EMG Signal Conditioning Module |
| Communication | Wi-Fi, IoT |
| Machine Learning | Decision Tree Algorithm |
| Artificial Intelligence | AI-powered Chatbot |
| Frontend | HTML, CSS, JavaScript |
| Database | Local Database |
| Data Processing | Analog-to-Digital Conversion (ADC) |

## 🔌 Hardware Components

1. ESP32 DevKit V1 Microcontroller
2. EMG Sensor Module
3. Temperature Sensor (LM35/DHT22)
4. Signal Conditioning Module
5. Electrode Pads (Ag/AgCl)
6. 5V DC Power Supply
7. Jumper Wires

## ⚙️ System Architecture

The system follows a multi-stage workflow:

1. **Data Acquisition:** EMG and temperature sensors collect physiological readings.
2. **Signal Processing:** ESP32 processes the incoming analog signals using its ADC.
3. **Data Transmission:** Processed readings are transmitted through Wi-Fi.
4. **Data Storage:** Sensor readings are stored in a local database.
5. **Stress Classification:** A trained Decision Tree model predicts the stress category.
6. **Dashboard Visualization:** The web dashboard displays real-time readings and stress levels.
7. **Relaxation Feedback:** The AI chatbot suggests appropriate relaxation techniques.

## 🔄 Working Flow

```text
EMG Sensor + Temperature Sensor
              |
              v
         ESP32 Module
              |
              v
       Signal Processing
              |
              v
         Wi-Fi Transfer
              |
              v
        Local Database
              |
              v
      Decision Tree Model
              |
              v
       Stress Classification
              |
          +---+---+
          |       |
          v       v
     Web Dashboard  AI Chatbot
          |       |
          +---+---+
              |
              v
     Relaxation Feedback
```

## 📊 Stress Classification

| Stress Category | System Response |
|---|---|
| Normal | Displays normal stress status |
| Moderate Stress | Suggests breathing exercises |
| High Stress | Provides meditation guidance and motivational support |

## 🎯 Project Objectives

- Develop an IoT-based real-time stress monitoring system.
- Integrate EMG and temperature sensors for physiological data acquisition.
- Use ESP32 for sensor interfacing and wireless communication.
- Implement Machine Learning for automated stress classification.
- Develop a user-friendly web dashboard.
- Integrate an AI chatbot for personalized relaxation feedback.

## 🚀 Future Scope

- Integration of additional sensors such as ECG, HRV, GSR, and EEG.
- Implementation of advanced ML and Deep Learning algorithms.
- Improved personalization of AI-based relaxation recommendations.
- Predictive stress analysis using historical sensor data.
- Advanced dashboard analytics and downloadable reports.
- Multi-user login and individual stress monitoring.
- Gamification and wellness progress tracking.

## 🏥 Applications

- Personal stress monitoring
- Smart healthcare systems
- Workplace wellness monitoring
- Educational institutions
- Wearable health monitoring devices
- Mental wellness and relaxation assistance

## 👥 Project Team

**Department:** Electronics and Communication Engineering

**Institution:** Priyadarshini College of Engineering, Nagpur

**Academic Year:** 2025–2026

**Project Members:**
1. Anugrah Adkane
2. Mariya Sonare
3. Supriya Bhongle
4. Ishita Ramteke

**Project Guide:** Prof. (Mrs.) S. I. Parihar

## 📌 Conclusion

This project demonstrates how IoT, Machine Learning, and Artificial Intelligence can work together to develop an intelligent stress monitoring and relaxation feedback system. By combining physiological sensing, real-time data processing, automated stress classification, and personalized relaxation guidance, the system provides a portable and cost-effective approach to proactive stress management.

---

**Note:** This project is developed for educational and research purposes. Its stress classifications are not a substitute for professional medical diagnosis.

⭐ If you find this project interesting, consider giving this repository a star!
:::Suggested GitHub repository name:

Real-Time-Stress-Detection-Using-EMG-and-ML

Suggested repository description:

Real-time stress detection and relaxation feedback system using EMG sensor, ESP32, temperature sensor, Decision Tree Machine Learning, IoT, and an AI-powered chatbot.
