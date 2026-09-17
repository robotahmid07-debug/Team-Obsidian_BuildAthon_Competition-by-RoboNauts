🏗️ System Architecture & System Robustness

EdgeCrop — Edge-AI Agricultural Plant Health Monitoring Rover

This document explains how the EdgeCrop system is divided into its main hardware, edge-AI, communication, and user-interface layers. The design keeps the robot controller independent from the AI workload so that the rover can continue handling movement and sensors while the smartphone performs local image inference.

📐 System Architecture

The system is split into four practical layers:

Cloud / Development Layer — dataset preparation, model training and evaluation.

Edge AI Layer — smartphone-based local image inference and dashboard.

Robot Control Layer — ESP32-S3 firmware for motors, sensors, servo and LCD.

Physical Layer — camera, RFID, gas sensor, motors and display.

flowchart TB

    %% =========================
    %% DEVELOPMENT / CLOUD
    %% =========================
    subgraph CLOUD["☁️ CLOUD / DEVELOPMENT"]
        DATA["📚 Agricultural Dataset"]
        TRAIN["🧠 Model Training<br/>Google Colab"]
        EVAL["📊 Validation & Evaluation"]
        MODEL["📦 Optimized<br/>Edge Model"]

        DATA --> TRAIN
        TRAIN --> EVAL
        EVAL --> MODEL
    end

    %% =========================
    %% EDGE DEVICE
    %% =========================
    subgraph EDGE["📱 EDGE DEVICE — SMARTPHONE"]
        CAMERA["📷 Camera Input"]
        PRE["Image Pre-processing"]
        AI["🤖 Local AI Inference"]
        RESULT["🌱 Plant Health Result"]
        DASH["📊 Mobile Dashboard"]
        HISTORY["🗂️ Inspection History"]

        CAMERA --> PRE --> AI --> RESULT
        RESULT --> DASH
        RESULT --> HISTORY
    end

    MODEL -->|Deploy trained model| AI

    %% =========================
    %% ROBOT CONTROLLER
    %% =========================
    subgraph ROBOT["🤖 ROBOT CONTROL — ESP32-S3"]
        CORE["⚙️ Main Control Loop"]
        MOTOR["🚗 Motor Control"]
        SERVO["🎯 Servo Control"]
        RFID["🏷️ RFID Reader"]
        GAS["🌫️ MQ Gas Sensor"]
        LCD["📺 I²C LCD"]
        LINK["🔗 Edge Communication"]

        CORE --> MOTOR
        CORE --> SERVO
        RFID --> CORE
        GAS --> CORE
        CORE --> LCD
        CORE <--> LINK
    end

    %% =========================
    %% PHYSICAL SYSTEM
    %% =========================
    subgraph FIELD["🌾 PHYSICAL INSPECTION SYSTEM"]
        PLANT["🌱 Plant / Leaf"]
        DRIVE["⚙️ DC Gear Motors"]
        TAG["🏷️ Plot RFID Tag"]
        GASPHY["🌫️ Environmental / VOC Source"]
        DISPLAY["📺 Local Status"]
    end

    PLANT -->|Image| CAMERA
    TAG --> RFID
    GASPHY --> GAS
    MOTOR --> DRIVE
    LCD --> DISPLAY

    %% =========================
    %% EDGE ↔ ROBOT
    %% =========================
    LINK <-->|Sensor data / commands / results| DASH

    %% =========================
    %% COLORS
    %% =========================
    classDef cloud fill:#E8F1FF,stroke:#2563EB,stroke-width:2px,color:#111827;
    classDef edge fill:#EAFBF0,stroke:#16A34A,stroke-width:2px,color:#111827;
    classDef robot fill:#FFF4E5,stroke:#EA580C,stroke-width:2px,color:#111827;
    classDef field fill:#F0FDF4,stroke:#15803D,stroke-width:2px,color:#111827;
    classDef ai fill:#F3E8FF,stroke:#9333EA,stroke-width:2px,color:#111827;

    class DATA,TRAIN,EVAL,MODEL cloud;
    class CAMERA,PRE,RESULT,DASH,HISTORY edge;
    class AI ai;
    class CORE,MOTOR,SERVO,RFID,GAS,LCD,LINK robot;
    class PLANT,DRIVE,TAG,GASPHY,DISPLAY field;

Reading the diagram

The important separation is:

TRAINING ≠ INFERENCE ≠ ROBOT CONTROL

The cloud environment is used to develop the model. The trained model is then deployed to the smartphone, where inference can happen locally. The ESP32-S3 remains responsible for real-time robot and sensor tasks.

🔄 Inspection Data Flow

A normal plant inspection follows this sequence:

┌───────────────┐
│  Rover moves  │
└───────┬───────┘
        ↓
┌────────────────┐
│ RFID reads plot│
│      ID        │
└───────┬────────┘
        ↓
┌────────────────┐
│ Camera captures│
│     plant      │
└───────┬────────┘
        ↓
┌────────────────┐
│ Smartphone     │
│ local inference│
└───────┬────────┘
        ↓
┌──────────────────────┐
│ AI result + sensor   │
│ data + plot ID       │
└──────────┬───────────┘
           ↓
    ┌──────┴──────┐
    ↓             ↓
┌────────┐   ┌──────────┐
│   LCD  │   │Dashboard │
└────────┘   └──────────┘

This keeps the inspection record tied to a physical plot instead of storing an image without knowing where it came from.

🧩 Responsibility of Each Layer

Layer

Main Responsibility

Typical Components

☁️ Development

Train and evaluate the AI model

Kaggle dataset, Google Colab

📱 Edge

Run local inference and provide UI

Smartphone, edge model

🤖 Control

Handle physical robot operation

ESP32-S3

🌾 Sensing

Collect field information

Camera, RFID, MQ sensor

⚙️ Actuation

Move and position the system

Motors, motor driver, servo

📺 Local UI

Show immediate status

I²C LCD

🛡️ System Robustness

The design does not depend on one component doing everything. Responsibilities are separated so that a problem in one part does not automatically mean that the entire system has to stop.

1. Separation of AI and Robot Control

The ESP32-S3 does not need to perform the main computer-vision workload.

             ┌──────────────────┐
             │ Smartphone       │
             │ AI / Dashboard   │
             └────────┬─────────┘
                      │
                Results / Data
                      │
                      ▼
             ┌──────────────────┐
             │ ESP32-S3         │
             │ Robot Controller │
             └──────────────────┘

This prevents the AI workload from being mixed directly into the motor-control logic.

If the model is replaced or improved, the basic robot-control firmware can remain largely unchanged.

2. Local Edge Inference

The trained model is deployed to the smartphone rather than requiring every image to be processed remotely.

Camera
  │
  ▼
Phone
  │
  ├── Internet available ──► Optional cloud services
  │
  └── Internet unavailable
              │
              ▼
       Local AI inference

The important operation is the local inference path. This is also relevant to the Agritech requirement for edge deployment and low-connectivity environments.

3. Sensor Data Validation

Sensor readings should not automatically be treated as valid just because a value was received.

The firmware can apply basic validation before displaying or recording a reading.

Sensor Reading
      ↓
Is the value valid?
   ↙       ↘
 YES        NO
  ↓          ↓
Use value   Flag reading
  ↓          ↓
  └────┬─────┘
       ↓
  Display / Log

For the MQ-series sensor in particular, readings are treated as supporting environmental information, not as proof that a plant has a specific disease.

4. Communication Failure Handling

The robot and smartphone are separate subsystems.

A communication problem should therefore be handled as a connection problem rather than as a reason to corrupt the robot's control state.

ESP32
  │
  │ connection
  ▼
Smartphone
  │
  ├── Connected
  │      ↓
  │  Normal data exchange
  │
  └── Disconnected
         ↓
   Robot remains under
   local ESP32 control

The exact communication transport can be changed during development without changing the overall architecture.

5. Display Redundancy

Important information is available through two interfaces:

                 Inspection Result
                        │
                ┌───────┴───────┐
                ↓               ↓
          📱 Smartphone      📺 LCD
          Detailed UI       Quick status

The LCD is intentionally simple.

Example:

┌────────────────────┐
│ PLOT: B-02         │
│ TOMATO             │
│ STATUS: ATTENTION  │
│ AI: 91%            │
└────────────────────┘

The smartphone remains the main interface for detailed information.

⚙️ Execution Model

The ESP32-S3 firmware is organized around short, independent tasks rather than one long blocking sequence.

┌────────────────────────────┐
│       Main Control Loop    │
├────────────────────────────┤
│ Read RFID                  │
│ Read sensor                │
│ Update motors              │
│ Update servo               │
│ Update LCD                 │
│ Process communication      │
└──────────────┬─────────────┘
               │
          Repeat quickly

This approach keeps the robot responsive while individual peripherals are being updated.

📊 Performance Considerations

Subsystem

Processing Location

Main Goal

Motor control

ESP32-S3

Fast local response

Servo control

ESP32-S3

Stable positioning

RFID

ESP32-S3

Plot identification

MQ sensor

ESP32-S3

Environmental reading

LCD

ESP32-S3

Rate-limited status updates

Image processing

Smartphone

Local AI inference

Model training

Cloud/PC

Training and evaluation

Dashboard

Smartphone

Human-readable results

The system does not require the ESP32-S3 to wait for the AI model before performing basic control tasks.

🌐 Low-Connectivity Approach

The architecture is intended to support field conditions where internet access may not always be reliable.

During development

Dataset
   ↓
Google Colab
   ↓
Training
   ↓
Evaluation

During inspection

Plant
  ↓
Camera
  ↓
Smartphone
  ↓
Local Model
  ↓
Result
  ↓
Local Dashboard / LCD

The inspection pipeline therefore has a local path that does not require a cloud request for every image.

🔧 Failure Scenarios

Failure

Expected System Behaviour

Internet unavailable

Local edge inference can continue

AI model unavailable

Robot control remains separate

RFID not detected

Plot ID remains unconfirmed until a valid tag is read

Invalid sensor reading

Reading can be flagged instead of blindly used

Smartphone disconnected

ESP32 can continue basic local robot control

LCD unavailable

Detailed information remains available through the dashboard

Camera unavailable

AI inspection cannot run, but the robot controller remains operational

🧠 Why This Architecture?

The main design decision is to avoid putting the entire system on one device.

Instead:

                 EdgeCrop
                    │
       ┌────────────┼────────────┐
       ↓            ↓            ↓
    AI/Phone     ESP32        Sensors
       │            │            │
       └────────────┼────────────┘
                    ↓
                 Results

Each part has a clear job.

The smartphone handles intelligence.

The ESP32 handles control.

The sensors collect field information.

The dashboard turns the information into something a person can understand.

