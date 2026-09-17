# Team-Obsidian_BuildAthon_Competition-by-RoboNauts Track C(Agritech)
https://colab.research.google.com/drive/1R-qn433yajfv_r_-DDeRw32P4jHemGH7?usp=sharing
from pathlib import Path

readme = r'''# 🌱 EdgeCrop
## Edge-AI Agricultural Plant Health Monitoring & Precision Field Inspection Rover

> **EdgeCrop** is a practical edge-AI agricultural inspection system that combines a mobile inspection rover, ESP32-S3 IoT control, RFID-based plot identification, environmental sensing, and a **custom-trained computer vision model deployed directly on a smartphone**.
>
> The system is designed around a simple principle:
>
> **Collect in the field → Analyze at the edge → Record locally → Reduce unnecessary resource use.**

---

## 🏆 BuildAthon Track

**Track C — Agritech**

BuildAthon's Agritech track focuses on precision farming, pest detection, yield prediction, supply-chain optimization, and resource management. It also emphasizes **edge deployment / low-connectivity operation**, resource conservation, and scalable communication for physical systems. fileciteturn1file0L43-L52

EdgeCrop focuses primarily on:

- 🌿 **Precision farming**
- 🔎 **Plant-health / pest-condition detection**
- 📊 **Field resource monitoring**
- 📱 **Edge AI**
- 🤖 **Physical agricultural inspection**
- 💧 Potential reduction of unnecessary chemical and water usage through targeted inspection

---

# 1. 🎯 Problem

Traditional crop inspection can require farmers or agricultural workers to manually inspect large areas and identify abnormal plants one by one.

This creates several practical problems:

- Large fields require significant inspection time.
- Early signs of plant stress or disease can be missed.
- Manual observations are difficult to organize by exact plot.
- Repeated inspection can require unnecessary movement and labor.
- Sending every image to a cloud AI service increases connectivity dependence and data transfer.
- A disease/condition observed in one location may not be properly linked to that specific plot.

EdgeCrop addresses this by combining **physical field inspection + local AI inference + structured plot identification**.

---

# 2. 💡 Solution

EdgeCrop uses a small rover as a mobile agricultural data-collection platform.

The rover:

1. Moves through the inspection area.
2. Identifies the current plot using RFID.
3. Collects environmental/VOC sensor information.
4. Captures plant or leaf imagery.
5. Sends the required information to the smartphone.
6. Runs the **custom-trained AI model locally on the smartphone**.
7. Displays the prediction and confidence.
8. Associates the observation with the identified plot.
9. Records the inspection for later review.

### Core workflow

```text
┌─────────────┐
│ 🌱 FIELD    │
└──────┬──────┘
       ↓
┌─────────────┐
│ 🤖 SCAN     │
│ Rover moves │
└──────┬──────┘
       ↓
┌─────────────┐
│ 🏷️ IDENTIFY │
│ RFID → Plot │
└──────┬──────┘
       ↓
┌─────────────┐
│ 📷 CAPTURE  │
│ Plant image │
└──────┬──────┘
       ↓
┌─────────────┐
│ 🧠 ANALYZE  │
│ Local AI    │
└──────┬──────┘
       ↓
┌─────────────┐
│ 📝 RECORD    │
│ Plot result │
└──────┬──────┘
       ↓
┌─────────────┐
│ ⚠️ ALERT    │
│ Inspect     │
└─────────────┘
3. 🏗️ System Architecture
4. 🧠 Custom-Trained AI

A major component of EdgeCrop is its custom-trained agricultural computer vision model.

The model is not simply treated as a generic off-the-shelf prediction API.

The development pipeline is:

Agricultural Dataset
        │
        ▼
Dataset Preparation
        │
        ▼
Model Initialization
        │
        ▼
┌─────────────────────────┐
│ PHASE 1                 │
│ Initial Training        │
│ 5–8 Epochs              │
└───────────┬─────────────┘
            │
            ▼
       Validation
            │
            ▼
┌─────────────────────────┐
│ PHASE 2                 │
│ Fine-Tuning             │
│ 15 Epochs               │
└───────────┬─────────────┘
            │
            ▼
     Final Evaluation
            │
            ▼
    Model Optimization
            │
            ▼
      📱 Smartphone
Phase 1 — Initial Training

Epochs: 5–8

The first phase establishes the initial trained model using the selected agricultural dataset.

Purpose:

Learn the main visual patterns.
Establish a baseline.
Monitor training and validation behavior.
Identify obvious underfitting or data issues.
Phase 2 — Fine-Tuning

Epochs: 15

The second phase performs dedicated fine-tuning after the initial training stage.

Purpose:

Refine learned visual representations.
Improve class separation.
Adapt the model more closely to the target agricultural image classes.
Prepare the final model for edge deployment.

The exact hyperparameters and model architecture will be documented from the actual Colab experiment.

5. 📊 Current AI Training Results

The current measured training information is recorded below.

Metric	Result
Phase 1 training time	931 s
Phase 1 validation accuracy	0.2058 (20.58%)
Phase 2 validation accuracy	0.3079 (30.79%)
Phase 1 epochs	5–8
Phase 2 fine-tuning	15 epochs
Precision	To be calculated from validation predictions
Recall	To be calculated from validation predictions
F1-score	To be calculated from validation predictions
Important

Precision, recall, and F1-score should be calculated from the actual validation predictions/confusion matrix. They should not be inferred from accuracy.

This repository will therefore use measured metrics rather than fabricated performance claims.

6. 📱 Edge AI Deployment

After training, the model is exported and prepared for smartphone deployment.

The smartphone becomes the edge intelligence device.

Why smartphone edge AI?

A smartphone already contains:

CPU
GPU/NPU depending on device
camera
display
battery
local storage
wireless communication

Instead of adding another expensive AI computer to the rover, EdgeCrop uses the smartphone as the edge-computing layer.

This reduces:

Hardware complexity
Power requirements
Additional processing hardware
Dependence on continuous internet connectivity
7. 🌐 Low-Connectivity Design

The Agritech track specifically emphasizes edge deployment and low-connectivity environments, including offline-first capabilities and minimal payload delivery. fileciteturn1file0L48-L52

EdgeCrop follows an edge-first inference architecture.

Cloud is mainly used for development
Internet
   │
   ▼
Dataset + Colab
   │
   ▼
Train Model
   │
   ▼
Export Model
Field operation
🌱 FIELD
   │
   ▼
📷 Image
   │
   ▼
📱 Smartphone
   │
   ▼
🧠 Local AI
   │
   ▼
📊 Result

Continuous cloud AI inference is therefore not required for every plant image.

8. ♻️ Sustainability

EdgeCrop is designed around targeted agricultural inspection rather than treating an entire field uniformly.

Potential sustainability benefits include:

💧 Resource optimization

Identifying abnormal areas can help direct attention toward specific plots instead of applying the same intervention everywhere.

🧪 Reduced chemical usage

Early identification of potentially affected plants can support more targeted inspection and treatment rather than unnecessary blanket application.

⚡ Computational efficiency

The trained model is deployed on an existing smartphone rather than requiring a dedicated cloud inference server for every image.

📡 Reduced network dependency

Local inference reduces the need to continuously upload agricultural images.

🚜 Reduced unnecessary inspection

A mobile inspection platform can repeatedly collect structured observations without requiring a person to manually inspect every location.

These are system-design goals. Actual water, chemical, carbon, or labor savings should only be reported after measurement.

9. 🏷️ RFID Plot Identification

Each inspection can be associated with a physical plot.

        🏷️ RFID TAG
             │
             ▼
        ESP32-S3
             │
             ▼
       Plot ID: A-03
             │
             ▼
       📷 Plant Image
             │
             ▼
       🧠 AI Result
             │
             ▼
┌──────────────────────────┐
│ Plot A-03                │
│ AI Status: Attention     │
│ Confidence: XX%          │
│ Sensor Data: Recorded    │
└──────────────────────────┘

This converts an isolated image prediction into a location-associated agricultural observation.

10. 🤖 ESP32-S3 Control Layer

The ESP32-S3 is responsible for the physical robot rather than the heavy computer-vision workload.

Responsibilities
Motor control
Servo control
RFID reading
Environmental/VOC sensor reading
LCD status
Communication
Robot state management
Separation of responsibility
ESP32-S3
│
├── 🚗 Movement
├── 🎥 Servo
├── 🏷️ RFID
├── 🌫️ Sensor
├── 🖥️ LCD
└── 📡 Communication

        ↕

Smartphone
│
├── 📷 Image
├── 🧠 AI
├── 📊 Dashboard
└── 📝 Inspection Record

This separation makes the system easier to debug and allows the AI and robot-control components to be improved independently.

11. 🖥️ Dashboard Concept

The smartphone dashboard is intended to provide a simple agricultural command center.

Main dashboard
┌─────────────────────────────────────┐
│          🌱 EDGECROP                │
├─────────────────────────────────────┤
│ Plot: A-03                          │
│                                     │
│       📷 LIVE / CAPTURED IMAGE      │
│                                     │
│ AI Status:  ATTENTION               │
│ Confidence: XX%                     │
│ VOC Reading: XXX                    │
│                                     │
│ Rover: CONNECTED                    │
│ Battery: XX%                        │
├─────────────────────────────────────┤
│ 🌿 Healthy      XX                  │
│ ⚠️ Attention    XX                  │
│ 🔎 Review       XX                  │
└─────────────────────────────────────┘

Planned/implemented UI elements should be clearly distinguished in the final demo.

12. 🗺️ Farm Inspection View

The dashboard can associate inspection results with plot IDs.

                 FARM

        ┌────────┬────────┬────────┐
        │ A-01   │ A-02   │ A-03   │
        │   ✓    │   ✓    │   ⚠️   │
        ├────────┼────────┼────────┤
        │ B-01   │ B-02   │ B-03   │
        │   ✓    │   🔎   │   ✓    │
        ├────────┼────────┼────────┤
        │ C-01   │ C-02   │ C-03   │
        │   ✓    │   ✓    │   ⚠️   │
        └────────┴────────┴────────┘

The actual dashboard visualization can be replaced with the final implementation screenshot.

13. 🛡️ System Robustness

EdgeCrop is designed with subsystem separation so that one failure does not automatically bring down the complete system.

AI failure
Image
  │
  ▼
Inference
  │
  ├── Valid ──────► Record Result
  │
  └── Invalid ────► Manual Review

The system should never convert a failed or uncertain prediction into a false certainty.

Communication failure
ESP32-S3 ──────X────── Smartphone

        │
        ▼
Local control continues
        │
        ▼
LCD provides basic status

The robot's fundamental movement/control logic remains separated from dashboard communication.

Sensor failure
Sensor Reading
      │
      ▼
Validity Check
   ┌──┴──┐
   │     │
Valid   Invalid
   │     │
   ▼     ▼
Use    Flag / Ignore
14. 🔄 Inspection State Machine
15. 📈 Scalability

The MVP can operate with one rover and one smartphone, but the architecture can be expanded.

MVP
1 Rover
   ↓
1 ESP32-S3
   ↓
1 Smartphone
Larger deployment
ROVER 01 ─┐
ROVER 02 ─┤
ROVER 03 ─┼──► Regional Gateway
ROVER 04 ─┤
ROVER 05 ─┘
               │
               ▼
        Farm / Regional System

For larger physical deployments, scalable communication technologies such as MQTT or LoRaWAN can be introduced as the communication layer rather than treating the prototype's local communication method as the final regional-network architecture. The BuildAthon rules specifically request scalable communication protocols when physical components are used. fileciteturn1file0L51-L52

16. 🧩 Hardware Architecture
Component	Role
ESP32-S3	Main rover controller
Motor Driver	Drives rover motors
DC Gear Motors	Rover movement
Servo	Camera positioning
RFID Reader	Plot identification
MQ-series Sensor	Supporting environmental/VOC information
USB Webcam	Plant/leaf image capture
I2C LCD	Local system status
Battery	Mobile power
Buck Converter	Regulated power for electronics
Smartphone	Edge AI + dashboard
17. 🔌 Software Architecture
EdgeCrop/
│
├── firmware/
│   ├── motor_control/
│   ├── sensor_manager/
│   ├── rfid_manager/
│   ├── servo_control/
│   ├── lcd_manager/
│   └── communication/
│
├── ai/
│   ├── dataset/
│   ├── preprocessing/
│   ├── phase1_training/
│   ├── phase2_finetuning/
│   ├── evaluation/
│   └── export/
│
├── mobile/
│   ├── inference/
│   ├── dashboard/
│   ├── inspection_records/
│   └── communication/
│
├── docs/
│   ├── architecture/
│   ├── screenshots/
│   ├── ai-training/
│   └── hardware/
│
└── README.md
18. 🧪 Testing Strategy

Testing is divided into independent layers.

Hardware testing
Motor movement
Servo positioning
RFID detection
Sensor readings
LCD output
Battery behavior
Communication testing
ESP32 ↔ smartphone connection
Data transmission
Invalid/missing packet handling
Reconnection behavior
AI testing
Dataset validation
Phase 1 training
Phase 2 fine-tuning
Confusion matrix
Precision
Recall
F1-score
Inference latency
Model size
Full-system testing
RFID
 ↓
Plot ID
 ↓
Image Capture
 ↓
AI Inference
 ↓
Prediction
 ↓
Sensor Context
 ↓
Inspection Record
19. 📸 Project Evidence

Screenshots from the actual implementation will be added here.

AI Training
Dataset

[ADD SCREENSHOT HERE]

Phase 1 — 5–8 Epochs

[ADD SCREENSHOT HERE]

Phase 1 Results

[ADD SCREENSHOT HERE]

Phase 2 — 15 Epoch Fine-Tuning

[ADD SCREENSHOT HERE]

Phase 2 Results

[ADD SCREENSHOT HERE]

Confusion Matrix

[ADD SCREENSHOT HERE]

Classification Report

[ADD SCREENSHOT HERE]

Hardware
Complete Rover

[ADD IMAGE HERE]

ESP32-S3 Electronics

[ADD IMAGE HERE]

RFID Plot Identification

[ADD IMAGE HERE]

Camera / Plant Inspection

[ADD IMAGE HERE]

Mobile Edge AI
Smartphone AI Inference

[ADD SCREENSHOT HERE]

Dashboard

[ADD SCREENSHOT HERE]

Inspection History

[ADD SCREENSHOT HERE]

20. 📊 Final Performance Table

This table should be updated after final testing.

Metric	Result
Phase 1 training time	931 s
Phase 1 validation accuracy	20.58%
Phase 2 validation accuracy	30.79%
Macro Precision	[FINAL MEASURED VALUE]
Macro Recall	[FINAL MEASURED VALUE]
Macro F1-score	[FINAL MEASURED VALUE]
Model size	[FINAL MEASURED VALUE]
Smartphone inference time	[FINAL MEASURED VALUE]
Communication latency	[FINAL MEASURED VALUE]
Battery runtime	[FINAL MEASURED VALUE]
21. 📚 Third-Party Resources & AI Usage

BuildAthon allows third-party resources when they are documented and copyright requirements are respected. The rules also require documentation when AI is used extensively. fileciteturn1file0L11-L19

EdgeCrop will maintain a dedicated resource record.

Resource	Purpose	Source	License	Usage
Agricultural dataset	AI training	[ADD SOURCE]	[ADD LICENSE]	Training / validation
Base model, if used	Model initialization	[ADD SOURCE]	[ADD LICENSE]	Fine-tuning
ML framework	Training / inference	[ADD SOURCE]	[ADD LICENSE]	Software dependency
Mobile framework	Smartphone application	[ADD SOURCE]	[ADD LICENSE]	Application
ESP32 libraries	Hardware control	[ADD SOURCE]	[ADD LICENSE]	Firmware
AI-assisted development documentation

Where AI tools are used during development, the repository should record:

What was generated or suggested.
Which code was reviewed by the team.
What was modified.
What the team understood and tested.
Which parts were written independently.
Relevant prompts or development notes where appropriate.

The team should be able to explain the final implementation during judging.

22. 🌐 Repository & Commit Practice

The BuildAthon rules require a public/shared GitHub or GitLab repository with an active commit history and clear documentation. fileciteturn1file0L11-L19

Recommended commit structure:

feat: add RFID plot identification
feat: add motor control
feat: add environmental sensor reading
feat: add smartphone communication
feat: add phase 1 training pipeline
feat: add phase 2 fine-tuning pipeline
feat: add model export
feat: add mobile inference
feat: add inspection dashboard
docs: add system architecture
docs: add AI training evidence
fix: handle invalid sensor readings
test: verify local inference latency

Avoid one giant final commit.

The repository history should demonstrate the actual development process.

23. 🎬 3-Minute Demonstration Plan

The BuildAthon rubric includes UX/UI and specifically evaluates the execution of the 3-minute video/demonstration. fileciteturn1file0L68-L73

0:00–0:25 — Problem

Show:

Large field
     ↓
Manual inspection
     ↓
Slow + difficult to track

Explain the need for targeted agricultural inspection.

0:25–0:50 — Solution

Show the complete rover and explain:

Rover + ESP32-S3 + RFID + sensors + smartphone edge AI

0:50–1:20 — AI

Show:

Dataset
 ↓
Phase 1 — 5–8 epochs
 ↓
Validation
 ↓
Phase 2 — 15 epochs fine-tuning
 ↓
Final model

Then show the actual training screenshots.

1:20–2:15 — Live inspection

Demonstrate:

RFID → Plot ID → Image → Local AI → Result

The most important moment should be proving that the smartphone performs the inference locally.

2:15–2:40 — Dashboard

Show:

Plot ID
AI result
confidence
sensor information
inspection history
2:40–3:00 — Impact

Finish with:

EdgeCrop brings AI directly to the field, connects every observation to a physical plot, and reduces dependence on continuous cloud connectivity.

24. ⚙️ Feasibility & Cost Philosophy

The MVP intentionally uses widely available components.

Instead of building a dedicated high-performance AI computer into the rover, the system uses an existing smartphone as the edge-computing platform.

This keeps the physical system comparatively lightweight while preserving the ability to run a trained computer-vision model.

The architecture also allows individual components to be replaced without redesigning the entire system.

25. 🔮 Future Expansion

Possible future versions can add:

🌿 More crop classes
🐛 Dedicated pest detection
💧 Soil-moisture sensing
🌡️ Temperature / humidity sensing
💦 Irrigation recommendations
📍 GPS-based field mapping
📡 LoRaWAN regional sensor networks
📬 MQTT farm telemetry
📈 Long-term crop-health trends
🗺️ Multi-rover coordination
📊 Yield prediction
🧪 More targeted treatment recommendations
📱 Fully offline inspection synchronization

These are future extensions, not claims about the current MVP.

26. 🏆 Why EdgeCrop Fits Agritech

EdgeCrop maps directly to several areas emphasized by the Agritech track:

BuildAthon Focus	EdgeCrop Implementation
Precision Farming	Plot-specific inspection
Pest / condition detection	Custom-trained plant image model
Resource Management	Targeted inspection and intervention support
Edge Deployment	Smartphone local inference
Low Connectivity	Reduced dependence on cloud inference
Physical Prototype	Agricultural inspection rover
Scalable Communication	MQTT / LoRaWAN expansion path
Sustainability	Targeted resource use + reduced unnecessary data transfer
Technical Complexity	Robotics + embedded system + AI + mobile edge computing

The Agritech rules explicitly identify precision farming, pest detection, resource management, sustainability/resource conservation, and edge/low-connectivity deployment as relevant areas. fileciteturn1file0L43-L52

27. 🧠 Engineering Philosophy

EdgeCrop is not designed as a single large AI application.

It is a modular field system:

                  EDGECROP
                     │
       ┌─────────────┼─────────────┐
       │             │             │
       ▼             ▼             ▼
   🤖 ROBOT       📱 EDGE AI     📊 DATA
       │             │             │
       │             │             │
   ESP32-S3       Smartphone     Records
       │             │             │
   Sensors         Local AI      Plot IDs
   Motors          Dashboard     Results
   RFID            Inference     History

Each subsystem has a defined responsibility.

This makes the system easier to:

Test
Debug
Replace
Scale
Demonstrate
Explain to judges
28. 📌 Project Status
Module	Status
Rover platform	🔧 In development
ESP32-S3 control	🔧 In development
RFID plot identification	🔧 In development
Environmental sensing	🔧 In development
Smartphone edge AI	🔧 In development
Custom AI training	✅ Training pipeline started
Phase 1	✅ 931 s recorded
Phase 2 fine-tuning	🔧 Evaluation ongoing
Dashboard	🔧 In development
Physical demonstration	🔧 In development
Final screenshots	⏳ To be added
Final precision / recall / F1	⏳ To be calculated from validation predictions
🌱 Final Statement

EdgeCrop connects three technologies that are often developed separately:

🤖 Robotics

A physical platform collects agricultural observations.

🧠 Custom AI

A custom-trained computer-vision model is developed through:

Phase 1 — 5–8 epochs

→ Phase 2 — 15 epochs fine-tuning

→ Final evaluation

→ Edge deployment

📱 Edge Computing

The trained model runs locally on a smartphone, reducing dependence on continuous cloud connectivity.

Together, these form a practical architecture for plot-aware, edge-AI agricultural inspection.

📄 Competition Compliance Checklist
 Track C — Agritech
 Public/shared repository planned
 Active commit history planned
 Third-party resources documented
 AI usage documentation included
 Physical prototype architecture documented
 Edge deployment architecture documented
 Low-connectivity approach documented
 Scalable communication expansion documented
 Final screenshots added
 Final confusion matrix added
 Final precision calculated
 Final recall calculated
 Final F1-score calculated
 Final hardware testing completed
 Final 3-minute demonstration recorded

EdgeCrop — Bring the intelligence to the field, not the field to the cloud.
'''
