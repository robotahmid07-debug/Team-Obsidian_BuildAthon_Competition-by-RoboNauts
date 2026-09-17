# 🏗️ System Architecture & System Robustness

This document details the architectural layout, fault-tolerance mechanisms, and operational efficiency of the **AgriBot ESP32-S3 System Platform**. The system is built to survive communication drops, sensor failures, and physical bus noise without dropping execution frames.

---

## 📐 System Architecture Diagram

```text
                               ┌─────────────────────────────────────────┐
                               │       Mobile App / Edge AI Model        │
                               │  (TensorFlow Lite Crop Classifier)      │
                               └────────────────────┬────────────────────┘
                                                    │
                                      ┌─────────────┴─────────────┐
                                      │ USB Serial / Wi-Fi HTTP   │
                                      │ (JSON Inference Payload)  │
                                      └─────────────┬─────────────┘
                                                    │
                                                    ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                       ESP32-S3 Core Engine                                      │
│                                                                                                  │
│   ┌───────────────────────────┐   ┌───────────────────────────┐   ┌───────────────────────────┐  │
│   │   Synthetic Sensor Engine │   │   ESP-NOW Broadcast Engine│   │    Web & JSON Parser      │  │
│   │   (Noise & Drop Fallback) │   │   (Non-blocking Telemetry)│   │   (Wi-Fi AP + Web Server) │  │
│   └─────────────┬─────────────┘   └─────────────┬─────────────┘   └─────────────┬─────────────┘  │
└─────────────────┼───────────────────────────────┼───────────────────────────────┼────────────────┘
                  │                               │                               │
                  │ Shared I2C Bus                │ ESP-NOW Transceiver           │ GPIO PWM Signals
                  │ (GPIO 8 / GPIO 9)             │ (2.4GHz RF)                   │
                  │                               │                               │
        ┌─────────┴─────────┐                     ▼                               ▼
        │                   │           ┌───────────────────┐           ┌───────────────────┐
        ├──> PCA9685        │           │ Peripheral Nodes  │           │ L298N Driver      │
        │    (Arm + Soil)   │           │ (Remote Receivers)│           │ (6WD Skid-Steer)  │
        │                   │           └───────────────────┘           └───────────────────┘
        ├──> Standard LCD   │
        │    (16x2 / 20x4)  │
        │                   │
        └──> PN532 RFID     │
             (Tag Reader)   │
        └───────────────────┘
⚡ System Robustness & Fault Tolerance
The system incorporates hardware and firmware guardrails to ensure continuous uptime under harsh field conditions:

1. Synthetic Sensor Fallback Filtering
Problem: Analog sensor leads (Gas/Soil) can disconnect or pick up high inductive interference from chassis motor draw, resulting in floating ADC garbage values.

Mechanism: Every analog reading passes through a software bounds validator. If the reading drops below or exceeds physical thresholds (< 0.1V or > 3.0V), the pipeline intercepts the frame and replaces it with a 100% normal synthetic baseline (e.g., 0.40V for gas, 1.50V for soil).

Impact: Prevents bad decision triggers in downstream processing while keeping control loops executing smoothly.

2. Non-Blocking Wireless Fallbacks
ESP-NOW Resiliency: ESP-NOW protocol initialization runs inside an isolated, non-blocking routine. If peer handshake or RF setup fails, the firmware bypasses the block without throwing fatal errors.

Dual Ingestion (USB Serial + Wi-Fi HTTP): Edge AI inference data defaults to high-speed USB Serial parsing. If the USB link breaks, the local Wi-Fi Access Point (/api/inference) automatically takes over JSON ingestion.

3. Bus Power & Signal Isolation
I2C Clock Conditioning: The I2C bus operates at a reduced clock rate (100kHz) with explicit hardware reset logic on GPIO 5 for the PN532 module. This clears frozen bus states caused by voltage dips during servo movement.

Inductive Load Separation: Servos (PCA9685) and high-current traction motors (L298N) run on isolated power rails, shielding the ESP32-S3 logic pins from brownouts.

⏱️ Execution Efficiency & Performance
Module	Processing Paradigm	Latency / Frequency	Resource Footprint
Servo Control (PCA9685)	Hardware PWM offloading	50 Hz PWM Frame	Negligible CPU load
Chassis Drive (L298N)	Hardware LEDC / PWM	Real-time response	Minimum CPU cycles
LCD Refresh	Rate-limited timer (250ms)	4 FPS non-blocking	Low bus consumption
Telemetry Broadcast	Asynchronous ESP-NOW	500 ms cycle	Memory-efficient struct
JSON Parser	Static Memory Buffer (StaticJsonDocument)	Dynamic allocation-free	Heap fragment safe
