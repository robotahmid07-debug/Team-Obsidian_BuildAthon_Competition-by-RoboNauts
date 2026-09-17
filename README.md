# Team-Obsidian_BuildAthon_Competition-by-RoboNauts
https://colab.research.google.com/drive/1R-qn433yajfv_r_-DDeRw32P4jHemGH7?usp=sharing
# 🌱 EdgeCrop — Edge AI Agricultural Plant Health Monitoring

> **An edge-based agricultural inspection system that uses computer vision, environmental sensing, and a mobile device to identify plants that may require further inspection.**

---

## 📌 Overview

Farmers and agricultural workers often need to inspect a large number of plants manually. This can take a lot of time, and problems may not always be noticed early.

**EdgeCrop** is a small agricultural inspection rover designed to make this process easier.

The rover moves through a farm or a small test field, identifies the current plot using RFID, collects environmental readings, and captures images of plants. A trained computer-vision model runs directly on a smartphone, which acts as the system's **edge AI device**.

The result is shown both on the phone dashboard and on an I²C LCD mounted on the rover.

The system is designed around a simple idea:

```text
SCAN → ANALYZE → IDENTIFY → RECORD → ALERT
```

---

# 🎯 What EdgeCrop Does

The system can:

* 🚗 Move between inspection points
* 🏷️ Identify plots using RFID
* 📷 Capture plant/leaf images
* 🤖 Run plant-health analysis using a trained AI model
* 📱 Perform AI inference locally on a smartphone
* 🌡️ Collect supporting environmental/VOC readings
* 📺 Display important results on an onboard LCD
* 📊 Show detailed information through a mobile dashboard
* 📝 Keep inspection records for different plots

---

# 💡 Why Edge AI?

A major part of EdgeCrop is that the AI model is **not dependent on a cloud server for every prediction**.

The model is trained during development using a suitable agricultural dataset and cloud computing resources such as Google Colab.

After training, the model is prepared for deployment on the smartphone.

### Training

```text
Agricultural Dataset
        ↓
Data Preparation
        ↓
Model Training
        ↓
Validation & Testing
        ↓
Model Optimization
        ↓
Deployable Model
```

### Real-World Inference

```text
Plant / Leaf
     ↓
Camera
     ↓
Smartphone
     ↓
Local AI Inference
     ↓
Prediction
     ↓
Dashboard + LCD
```

This reduces the need to continuously upload images to a remote server and makes the system more suitable for environments where internet connectivity may be limited.

---

# 🧠 AI System

The AI component is responsible for analyzing images captured during plant inspection.

The development workflow includes:

1. Selecting an appropriate agricultural image dataset
2. Preparing and cleaning the data
3. Training or fine-tuning a computer-vision model
4. Testing the model on unseen images
5. Measuring model performance
6. Optimizing the model for edge inference
7. Deploying the resulting model on the smartphone

The AI output is treated as an **early-warning/decision-support result**, not as a confirmed agricultural diagnosis.

For example:

```text
Plant: Tomato

AI Result:
Possible Disease

Confidence:
91.4%

Recommendation:
Further inspection recommended.
```

---

# 📱 Edge Device

A smartphone is used as the edge-computing device.

The phone handles:

* Image processing
* AI inference
* Inspection results
* Dashboard
* Inspection history
* Communication with the rover

Using a smartphone allows the project to demonstrate that useful AI inference can be performed on a relatively low-cost, commonly available device rather than requiring a dedicated GPU computer at every inspection point.

The architecture is also designed so that the edge device can be replaced by another suitable device in the future.

---

# 🤖 Robot Architecture

The robot is controlled by an **ESP32-S3**.

The ESP32-S3 is responsible for the physical part of the system:

```text
                  ┌──────────────────┐
                  │   Smartphone     │
                  │   Edge AI + UI   │
                  └────────┬─────────┘
                           │
                    Communication
                           │
                           ▼
                  ┌──────────────────┐
                  │     ESP32-S3     │
                  │  Main Controller │
                  └───────┬──────────┘
                          │
       ┌──────────────────┼──────────────────┐
       │                  │                  │
       ▼                  ▼                  ▼
   Motor Driver       Sensors             LCD
       │             ┌────┴────┐            │
       ▼             │         │            ▼
    Motors          RFID      MQ Sensor   Status
```

### ESP32 responsibilities

* Motor control
* Servo control
* RFID reading
* MQ sensor reading
* LCD control
* Rover status
* Communication with the edge device

The ESP32 does **not** need to run the main computer-vision model. This keeps the robot controller lightweight and separates physical control from AI processing.

---

# 🔍 Inspection Process

A typical inspection works like this:

### 1. Move

The rover moves to an inspection point.

### 2. Identify

The RFID reader detects the tag assigned to the plot.

```text
RFID → PLOT-B02
```

### 3. Capture

The camera is positioned toward the plant and captures an image.

### 4. Analyze

The image is processed locally by the AI model on the smartphone.

### 5. Combine

The AI result is associated with the plot ID and supporting sensor readings.

### 6. Display

The result appears on both the phone and the onboard LCD.

### 7. Record

The inspection is saved for later review.

---

# 📺 Onboard Display

The I²C LCD provides quick information without requiring the user to open the dashboard.

Example:

```text
┌────────────────────┐
│ PLOT: B-02         │
│ TOMATO             │
│                    │
│ STATUS: ATTENTION  │
│ AI: 91%            │
│ VOC: 327           │
└────────────────────┘
```

The LCD is intended for short status information, while the smartphone provides the detailed analysis.

---

# 📊 Mobile Dashboard

The mobile dashboard provides a more complete view of the inspection.

It can display:

* Current plot
* Plant information
* Captured image
* AI prediction
* AI confidence
* Sensor readings
* Inspection status
* Previous inspection records

Example:

```text
PLOT B-02
───────────────

Tomato

Possible Disease
Confidence: 91.4%

VOC Reading: 327
Risk: Moderate

Further inspection
recommended.
```

---

# 🧩 Hardware

| Component          | Quantity | Purpose                    |
| ------------------ | -------: | -------------------------- |
| ESP32-S3 Dev Board |        1 | Main robot controller      |
| L298N / TB6612FNG  |        1 | Motor control              |
| DC Gear Motors     |      2–4 | Rover movement             |
| Wheels             |      2–4 | Mobility                   |
| Continuous Servo   |      1–2 | Mechanical actuation       |
| SG90 / MG90S Servo |        1 | Camera positioning         |
| MQ-135 / MQ-2      |      1–2 | Supporting gas/VOC reading |
| USB Webcam         |        1 | Plant/leaf image capture   |
| I²C LCD            |        1 | Local status display       |
| RC522 RFID Reader  |        1 | Plot identification        |
| Battery Pack       |      1–2 | Power supply               |

The exact hardware configuration may vary depending on the final prototype.

---

# 🔌 System Data Flow

```text
                   AGRICULTURAL FIELD
                           │
                           ▼
                    ┌─────────────┐
                    │    Plant    │
                    └──────┬──────┘
                           │
                      Camera Image
                           │
                           ▼
                  ┌─────────────────┐
                  │   Smartphone    │
                  │    Edge AI      │
                  └────────┬────────┘
                           │
                    AI Prediction
                           │
                           ▼
                  ┌─────────────────┐
                  │    ESP32-S3     │
                  └───────┬─────────┘
                          │
          ┌───────────────┼──────────────┐
          ▼               ▼              ▼
        RFID          MQ Sensor        Motors
          │               │              │
          └───────────────┼──────────────┘
                          │
                          ▼
                    Inspection Data
                          │
                 ┌────────┴────────┐
                 ▼                 ▼
              LCD             Mobile UI
```

---

# 🌾 Sustainability & Low-Connectivity Design

EdgeCrop is designed with resource efficiency in mind.

Instead of sending every captured image to a remote server, the trained model can perform inference locally on the edge device.

This can help reduce:

* Continuous internet dependency
* Data transfer requirements
* Cloud inference requests
* Infrastructure requirements at every field location

The system can also store inspection information locally and synchronize data when connectivity becomes available, depending on the final implementation.

---

# 🔮 Future Improvements

The current prototype focuses on proving the core system.

Future versions could include:

* More plant and disease classes
* Improved model accuracy
* Offline-first data synchronization
* Soil-moisture sensing
* Temperature and humidity sensing
* Automated irrigation recommendations
* Larger farm mapping
* Better autonomous navigation
* More efficient edge models
* Multiple rover support
* Centralized farm analytics

---

```

---

