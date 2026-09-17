# Team-Obsidian_BuildAthon_Competition-by-RoboNauts
https://colab.research.google.com/drive/1R-qn433yajfv_r_-DDeRw32P4jHemGH7?usp=sharing
<!-- HEADER BANNER -->
<div align="center">
  <img src="docs/banner.png" alt="Project Banner" width="100%" />
  
  # 🚀 AgriBot ESP32-S3 & Edge AI Vision Pipeline

  <p align="center">
    <b>An end-to-end embedded pipeline combining ESP32-S3 hardware control, I2C/RFID sensor integration, and real-time TensorFlow Lite Edge AI inference.</b>
  </p>

  <!-- BADGES -->
  <a href="https://github.com/your-username/your-repo/stargazers"><img src="https://img.shields.io/github/stars/your-username/your-repo?style=for-the-badge&color=00C853" alt="Stars"></a>
  <a href="https://github.com/your-username/your-repo/network/members"><img src="https://img.shields.io/github/forks/your-username/your-repo?style=for-the-badge&color=0288D1" alt="Forks"></a>
  <a href="https://github.com/your-username/your-repo/issues"><img src="https://img.shields.io/github/issues/your-username/your-repo?style=for-the-badge&color=FF6D00" alt="Issues"></a>
  <a href="https://github.com/your-username/your-repo/blob/main/LICENSE"><img src="https://img.shields.io/github/license/your-username/your-repo?style=for-the-badge&color=D500F9" alt="License"></a>
</div>

---

## 📌 Overview

This repository houses the firmware and edge intelligence pipeline for the **AgriBot** platform. The system integrates hardware execution on the ESP32-S3 MCU with offloaded or local neural network classification models to handle real-time tag identification, sensor telemetry, and disease detection.

<div align="center">
  <img src="docs/architecture_diagram.png" alt="System Architecture" width="85%" />
  <br>
  <sub><i>Figure 1: Hardware control loop and Edge AI decision pipeline.</i></sub>
</div>

---

## ✨ Key Features

* **Dual-Protocol Hardware Interface:** Configured for I2C communication (`GPIO 8` / `GPIO 9`) with PN532 RFID modules and motor controllers.
* **On-Device Inference Pipeline:** Accepts JSON inference payloads containing class predictions and confidence scores over local Wi-Fi SoftAP or Serial.
* **Fault-Tolerant Bus Control:** Low-frequency I2C clock adjustments and explicit reset pin toggling to prevent bus lockups under inductive load spikes.
* **Non-Blocking Execution Loop:** Event-driven target activation ensuring sub-100ms response times without stalling core system tasks.

---

## 🛠 Tech Stack & Hardware Components

| Category | Component / Tool | Details |
| :--- | :--- | :--- |
| **Microcontroller** | ESP32-S3 | Xtensa® 32-bit LX7 Dual-Core, 240 MHz |
| **Peripherals** | PN532 NFC/RFID Kit | I2C Mode (`0x24` / `0x28`), Custom Pin Mapping |
| **Edge AI Framework** | TensorFlow Lite / MobileNet | Quantized `.tflite` model deployed on mobile/edge host |
| **Communication** | Wi-Fi HTTP SoftAP / USB Serial | RESTful JSON payload ingestion |
| **Development** | Arduino IDE / C++ / Wire.h | Custom firmware architecture |

---

## 🏗 Pipeline Architecture

```text
  ┌─────────────────┐       I2C (GPIO 8/9)       ┌──────────────────┐
  │ PN532 RFID /    │ ─────────────────────────> │                  │
  │ Sensors         │                            │                  │
  └─────────────────┘                            │   ESP32-S3      │
                                                 │   Core Engine    │ ───> Actuators / Motors
  ┌─────────────────┐    HTTP POST / Serial      │                  │
  │ Mobile Device / │ ─────────────────────────> │                  │
  │ Edge AI Model   │     (JSON Payload)         └──────────────────┘
  └─────────────────┘
