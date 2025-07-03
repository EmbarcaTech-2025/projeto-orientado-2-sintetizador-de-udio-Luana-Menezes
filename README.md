# Project 2 – Audio Synthesizer - EmbarcaTech 2025

Author: **Luana Menezes**

Course: Technological Residency in Embedded Systems

Institution: EmbarcaTech - HBr

Campinas, July 2025

---

# Audio Synthesizer with BitDogLab

This project develops an embedded audio recording and playback system using the BitDogLab board, programmed in structured C language on VSCode. The system implements a **complete audio pipeline** that captures, stores, processes, and reproduces audio signals:

- **Audio Capture**: Records microphone input at 8kHz using 12-bit ADC converted to 8-bit for memory efficiency
- **Audio Storage**: Stores up to 10 seconds of audio in RAM buffer (80KB)
- **Audio Processing**: Applies high-pass filtering to reduce noise and DC offset
- **Audio Playback**: Reproduces stored audio through PWM-driven buzzer with 12-bit resolution
- **Visual Feedback**: Displays real-time waveform on OLED screen and LED status indicators

The implementation demonstrates **analog-to-digital conversion**, **digital signal processing**, **PWM audio synthesis**, and **interactive embedded system design**.


## Objective

Develop skills in embedded audio processing and digital signal manipulation. Upon completion, you will be able to:

- Implement ADC-based audio capture systems
- Apply digital signal processing techniques (filtering, noise reduction)
- Generate audio using PWM modulation
- Optimize memory usage through bit-depth reduction
- Create interactive embedded interfaces with buttons and displays
- Visualize audio waveforms in real-time

## Bill of Materials

| Component                | BitDogLab Connection      |
|--------------------------|---------------------------|
| BitDogLab (RP2040)       | -                         |
| Microphone (Analog)      | GPIO 28 (ADC Channel 2)   |
| Buzzer (PWM)             | GPIO 21                   |
| Button A                 | GPIO 5                    |
| Button B                 | GPIO 6                    |
| LED Red                  | GPIO 13                   |
| LED Green                | GPIO 11                   |
| OLED Display (I2C)       | SDA: GPIO 14, SCL: GPIO 15|

## Setup and Execution

1. Open the project in VS Code with Raspberry Pi Pico SDK environment
2. Build the project (Ctrl+Shift+B or cmake/make)
3. Connect BitDogLab via USB in boot mode (BOOTSEL button)
4. Copy the generated .uf2 file to RPI-RP2 storage
5. The system will start with interactive controls:
   - **Press Button A**: Start recording (Red LED on)
   - **Press Button B**: Play recorded audio (Green LED on)

## System Logic

### Audio Recording Process:
- Samples microphone at 8kHz (125μs intervals)
- Converts 12-bit ADC values (0-4095) to 8-bit (0-255) for memory efficiency
- Stores 10 seconds of audio (80,000 samples) in RAM buffer
- Provides real-time progress feedback via serial output

### Audio Processing:
- **High-pass filter**: Removes DC offset and low-frequency noise
- Calculates average signal level and centers audio around 127
- Maintains signal integrity while reducing background noise

### Audio Playback:
- Converts 8-bit stored values back to 12-bit PWM resolution
- Outputs audio through PWM at original sampling rate
- Uses 4095-level PWM for smooth audio reproduction

### Visual Display:
- **Waveform visualization**: Shows amplitude as vertical bars on OLED
- **Center line reference**: Displays silence level (middle of screen)
- **Amplitude mapping**: Scales audio levels to screen height

## Project Files

- [`main.c`](main.c): Complete audio synthesizer implementation
- [`include/ssd1306.h`](include/ssd1306.h): OLED display driver interface

## Features Demonstrated

- **Analog-to-Digital Conversion**: 12-bit ADC sampling with configurable rates
- **Memory Optimization**: 8-bit storage with 12-bit playback resolution
- **Digital Signal Processing**: High-pass filtering and noise reduction
- **PWM Audio Synthesis**: Variable duty cycle audio generation
- **Interactive Control**: Button-based recording/playback interface
- **Real-time Visualization**: Live waveform display on OLED screen
- **Hardware Integration**: LEDs, buttons, microphone, buzzer, and display coordination

## Technical Specifications

- **Sampling Rate**: 8kHz (suitable for voice applications)
- **Recording Duration**: 10 seconds
- **Storage Format**: 8-bit unsigned integers
- **Playback Resolution**: 12-bit PWM (4096 levels)
- **Memory Usage**: 80KB RAM for audio buffer
- **Audio Quality**: Voice-optimized with noise filtering

## 🖼️ Project Images/Videos

.

## Final Reflection

### What programming techniques can we use to improve audio recording and playback?

Several programming techniques can significantly enhance audio quality and system performance. Memory management is crucial, where converting from 12-bit to 8-bit storage reduces memory usage by 50%, allowing longer recordings. Implementing circular buffers enables continuous recording and playback without interruption, while dynamic memory allocation provides flexibility for variable recording lengths. DMA (Direct Memory Access) can reduce CPU overhead during audio transfer operations.

For signal processing improvements, digital filtering techniques such as high-pass, low-pass, and band-pass filters effectively remove unwanted noise. Amplitude normalization through automatic gain control optimizes signal levels, while noise gating suppresses background noise during quiet periods. Compression algorithms like ADPCM can reduce file sizes without significant quality loss.

Real-time optimization involves using interrupt-driven sampling for precise timing, implementing multi-threading with separate tasks for recording, processing, and playback, and utilizing hardware acceleration when available. Double-buffering strategies prevent audio dropouts during continuous operation.

### How is it possible to record longer audio without compromising recording quality?

Recording longer audio while maintaining quality requires a combination of memory expansion and intelligent compression strategies. External storage solutions like SD cards can support hours of audio recording, enabling real-time streaming to storage during recording. Utilizing all available RAM efficiently through multiple memory banks maximizes the internal storage capacity.

Compression techniques play a vital role in extending recording time. Lossless compression algorithms reduce file size without quality degradation, while variable bit-rate encoding allocates more bits to complex audio segments and fewer bits to simple ones. Silence detection can skip storage of quiet periods, and delta encoding stores differences between consecutive samples rather than absolute values.

Quality-preserving approaches include adaptive sampling that uses higher rates for complex audio and lower rates for simple audio, multi-resolution storage for different quality requirements, and psychoacoustic modeling to remove inaudible frequency components. Hardware upgrades to microcontrollers with larger RAM or storage capacity provide the most direct solution for longer recordings without quality compromise.

## 📜 License
GNU GPL-3.0.