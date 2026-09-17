"""
AgriBot BuildAthon  team "Obsidian"
"""
import os
import sys
import json
import numpy as np
import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers
from tensorflow.keras.applications import MobileNetV3Small

#our dataset path
DATASET_PATH = "./PlantVillage_Raw/New Plant Diseases Dataset(Augmented)/New Plant Diseases Dataset(Augmented)/train"

CFG = {
    "dataset_dir"     : DATASET_PATH,
    "img_size"        : (224, 224),
    "batch_size"      : 16,
    "val_split"       : 0.15,
    "test_split"      : 0.10,

    "epochs_warmup"   : 8,
    "epochs_finetune" : 15,
    "lr_warmup"       : 1e-3,
    "lr_finetune"     : 1e-5,
    "dropout"         : 0.4,
    "label_smooth"    : 0.1,

    "tflite_output"   : "./plant_disease_classifier.tflite",
    "labels_output"   : "./labels.json",
}

#Gpu Setup ────────────────────────────────────────────────────────────
gpus = tf.config.list_physical_devices('GPU')
if gpus:
    try:
        for gpu in gpus:
            tf.config.experimental.set_memory_growth(gpu, True)
        print(f"✓ GPU Memory Growth Enabled: {gpus}")
    except RuntimeError as e:
        print(f"GPU Setup Error: {e}")

# Dataset Loader Pipeline
def load_dataset_safe(cfg: dict):
    dataset_root = cfg["dataset_dir"]
    img_size     = cfg["img_size"]
    batch_size   = cfg["batch_size"]
    SEED         = 42
    total_split  = cfg["val_split"] + cfg["test_split"]

    full_train = keras.utils.image_dataset_from_directory(
        dataset_root,
        validation_split = total_split,
        subset           = "training",
        seed             = SEED,
        image_size       = img_size,
        batch_size       = batch_size,
        label_mode       = "categorical",
    )

    full_val_test = keras.utils.image_dataset_from_directory(
        dataset_root,
        validation_split = total_split,
        subset           = "validation",
        seed             = SEED,
        image_size       = img_size,
        batch_size       = batch_size,
        label_mode       = "categorical",
    )

    class_names = full_train.class_names
    num_classes = len(class_names)
    print(f"\n Found {num_classes} distinct classes.")

    val_batches = max(1, int(len(full_val_test) * (cfg["val_split"] / total_split)))
    val_ds  = full_val_test.take(val_batches)
    test_ds = full_val_test.skip(val_batches)

    AUTOTUNE = tf.data.AUTOTUNE
    train_ds = full_train.prefetch(buffer_size=AUTOTUNE)
    val_ds   = val_ds.prefetch(buffer_size=AUTOTUNE)
    test_ds  = test_ds.prefetch(buffer_size=AUTOTUNE)

    return train_ds, val_ds, test_ds, class_names

train_ds, val_ds, test_ds, CLASS_NAMES = load_dataset_safe(CFG)
NUM_CLASSES = len(CLASS_NAMES)

# label saving
labels_map = {i: name for i, name in enumerate(CLASS_NAMES)}
with open(CFG["labels_output"], "w") as f:
    json.dump(labels_map, f, indent=2)
print(f" Labels map saved → {CFG['labels_output']}")

# building model
def build_model(num_classes: int, img_size: tuple, dropout: float):
    augmentation = keras.Sequential([
        layers.RandomFlip("horizontal_and_vertical"),
        layers.RandomRotation(0.15),
        layers.RandomZoom(0.15),
    ], name="data_augmentation")

    base = MobileNetV3Small(
        input_shape = (*img_size, 3),
        include_top = False,
        weights     = "imagenet",
        pooling     = "avg",
    )
    base.trainable = False

    inputs = keras.Input(shape=(*img_size, 3))
    x = augmentation(inputs)
    x = layers.Rescaling(1.0 / 127.5, offset=-1)(x)
    x = base(x, training=False)
    x = layers.Dropout(dropout)(x)
    x = layers.Dense(256, activation="relu")(x)
    x = layers.BatchNormalization()(x)
    x = layers.Dropout(dropout * 0.5)(x)
    outputs = layers.Dense(num_classes, activation="softmax")(x)

    return keras.Model(inputs, outputs, name="AgriBot_PlantDisease_v1"), base

model, base_model = build_model(NUM_CLASSES, CFG["img_size"], CFG["dropout"])

# phase 1 of tranning
model.compile(
    optimizer = keras.optimizers.Adam(learning_rate=CFG["lr_warmup"]),
    loss      = keras.losses.CategoricalCrossentropy(label_smoothing=CFG["label_smooth"]),
    metrics   = ["accuracy", keras.metrics.TopKCategoricalAccuracy(k=3, name="top3_acc")]
)

callbacks_warmup = [
    keras.callbacks.EarlyStopping(patience=3, restore_best_weights=True, monitor="val_accuracy"),
    keras.callbacks.ReduceLROnPlateau(factor=0.5, patience=2, monitor="val_loss"),
]

print("\n" + "="*60)
print("PHASE 1: Warmup Training")
print("="*60)
history_warmup = model.fit(
    train_ds,
    validation_data = val_ds,
    epochs          = CFG["epochs_warmup"],
    callbacks       = callbacks_warmup,
    verbose         = 1
)

#model tranning phase 2 finetuning part
unfreeze_from = int(len(base_model.layers) * 0.70)
for layer in base_model.layers[unfreeze_from:]:
    if not isinstance(layer, layers.BatchNormalization):
        layer.trainable = True

model.compile(
    optimizer = keras.optimizers.Adam(learning_rate=CFG["lr_finetune"]),
    loss      = keras.losses.CategoricalCrossentropy(label_smoothing=CFG["label_smooth"]),
    metrics   = ["accuracy", keras.metrics.TopKCategoricalAccuracy(k=3, name="top3_acc")]
)

callbacks_finetune = [
    keras.callbacks.EarlyStopping(patience=5, restore_best_weights=True, monitor="val_accuracy"),
    keras.callbacks.ReduceLROnPlateau(factor=0.3, patience=2, monitor="val_loss"),
]

print("\n" + "="*60)
print("PHASE 2: Fine-Tuning Top Layers")
print("="*60)
history_finetune = model.fit(
    train_ds,
    validation_data = val_ds,
    epochs          = CFG["epochs_finetune"],
    callbacks       = callbacks_finetune,
    verbose         = 1
)

# lite Weight model conversion
print("\n" + "="*60)
print("Exporting Optimized TFLITE Model")
print("="*60)

converter = tf.lite.TFLiteConverter.from_keras_model(model)
converter.optimizations = [tf.lite.Optimize.DEFAULT]
tflite_model = converter.convert()

with open(CFG["tflite_output"], "wb") as f:
    f.write(tflite_model)

size_mb = os.path.getsize(CFG["tflite_output"]) / (1024 * 1024)
print(f"✓ TFLite export complete → {CFG['tflite_output']} ({size_mb:.2f} MB)")
