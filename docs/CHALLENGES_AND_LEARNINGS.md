# Project Challenges and Lessons Learned

San Jose State University - Electrical Engineering Senior Design Capstone Project

## Project Overview

This document outlines the technical challenges, difficulties encountered, and key learnings from developing a velocity-sensitive USB MIDI keyboard controller with real-time polyphonic synthesis. The project integrates embedded systems (Raspberry Pi Pico), hardware design (keyboard matrix), firmware development (C), and software engineering (Python audio synthesis).

## Major Technical Challenges

### 1. Velocity Detection Implementation

**Challenge:**
Implementing accurate velocity detection using a 2-phase contact system (early/late contact) required precise timing measurements and careful calibration of the velocity calculation algorithm.

**Difficulties:**
- Timing precision: Measuring microsecond-level differences between early and late contacts
- Hardware settling time: Accounting for signal propagation delays in the matrix
- Velocity mapping: Converting time deltas (100μs to 50ms) to MIDI velocity values (1-127) with appropriate sensitivity
- Edge cases: Handling very fast presses where early contact might be missed, or very slow presses that exceed timeout thresholds

**Solution:**
- Implemented 2-phase scanning with 10μs settling time between row transitions
- Developed linear interpolation algorithm for velocity calculation
- Added timeout handling for edge cases (default velocity for missed contacts)
- Extensive testing with simulated input to validate algorithm before hardware integration

**Learning:**
- Importance of hardware characterization before algorithm design
- Value of comprehensive edge case testing
- Trade-offs between sensitivity and reliability in sensor systems

### 2. Dual-Core Architecture and Inter-Core Communication

**Challenge:**
Designing a system that simultaneously handles high-frequency GPIO polling (500Hz) and USB MIDI communication without blocking or missing key presses.

**Difficulties:**
- Real-time constraints: GPIO scanning must occur at consistent 2ms intervals
- USB task scheduling: USB stack requires regular servicing but shouldn't block GPIO operations
- Shared state management: Key states must be accessible from both cores safely
- Synchronization: Ensuring Core 0 reads consistent key states from Core 1

**Solution:**
- Separated concerns: Core 1 dedicated to GPIO polling, Core 0 handles USB and event processing
- Shared memory: Used static arrays for key state sharing (no explicit locking needed for read-only access)
- Non-blocking USB: Asynchronous MIDI transmission via TinyUSB stack
- Polling-based IPC: Core 0 polls key states rather than using interrupts

**Learning:**
- Understanding of multicore embedded systems architecture
- Importance of task separation in real-time systems
- Trade-offs between interrupt-driven and polling-based designs
- Memory consistency in shared-memory multiprocessing

### 3. Hardware Matrix Design and Shift Register Integration

**Challenge:**
Reading 25 keys using only a few GPIO pins through MSQT32 shift registers, while maintaining fast scan rates and reliable state detection.

**Difficulties:**
- Shift register timing: Proper sequencing of DATA, CLOCK, and LATCH signals
- Signal integrity: Ensuring clean digital signals over physical connections
- Scan rate optimization: Balancing speed with reliability
- Debouncing: Filtering mechanical switch bounce without losing legitimate key presses

**Solution:**
- Implemented proper shift register protocol with minimum pulse widths
- Added settling delays between signal transitions
- Developed debouncing algorithm with 5ms debounce time
- Two-phase scanning for velocity while maintaining fast overall scan rate

**Learning:**
- Digital signal timing and protocol implementation
- Hardware-software co-design principles
- Importance of datasheet analysis for timing specifications
- Debugging techniques for digital communication protocols

### 4. USB MIDI Protocol Implementation

**Challenge:**
Implementing USB MIDI Class specification correctly to ensure plug-and-play compatibility across operating systems.

**Difficulties:**
- USB descriptor configuration: Correctly defining device, configuration, and interface descriptors
- MIDI message formatting: Ensuring proper 3-byte message structure
- Endpoint configuration: Setting up IN/OUT endpoints correctly
- Cross-platform compatibility: Testing on macOS, Windows, and Linux

**Solution:**
- Used TinyUSB library which handles USB protocol details
- Studied USB MIDI Class specification for descriptor structure
- Implemented standard MIDI message format (status byte, note, velocity)
- Tested on multiple platforms to verify compatibility

**Learning:**
- USB protocol fundamentals and class specifications
- Importance of following industry standards for compatibility
- Value of using established libraries (TinyUSB) for complex protocols
- Cross-platform testing methodologies

### 5. Real-Time Audio Synthesis

**Challenge:**
Implementing polyphonic audio synthesis in Python with low latency and real-time performance.

**Difficulties:**
- Thread safety: Managing concurrent note additions/removals
- Audio buffer management: Generating samples at consistent 44100 Hz rate
- Phase tracking: Maintaining phase continuity for each note
- Performance: Ensuring synthesis doesn't drop frames or cause audio glitches

**Solution:**
- Used threading locks for thread-safe note state management
- Implemented phase accumulation per note for continuous waveforms
- Used sounddevice library for low-latency audio I/O
- Optimized buffer sizes (1024 samples) for balance between latency and stability

**Learning:**
- Real-time audio programming concepts
- Thread synchronization in Python
- Audio buffer management and sample rate considerations
- Performance profiling and optimization techniques

### 6. Cross-Platform Development and Deployment

**Challenge:**
Creating a system that works seamlessly on macOS, Windows, and Linux with appropriate platform-specific features.

**Difficulties:**
- Platform-specific dependencies: macOS menu bar requires `rumps` library
- Auto-launch mechanisms: Different systems (LaunchAgent, systemd, Task Scheduler)
- Pico detection: Different methods for detecting USB device on each platform
- Testing: Ensuring functionality across all platforms

**Solution:**
- Made GUI components optional (macOS-only)
- Created platform-specific setup scripts
- Implemented platform detection for Pico (system_profiler, lsusb, PowerShell)
- Comprehensive testing on each platform

**Learning:**
- Cross-platform software development practices
- Platform-specific system integration
- Importance of graceful degradation (background mode when GUI unavailable)
- Automated setup script design

### 7. Testing Without Hardware

**Challenge:**
Developing and validating firmware logic before physical hardware was available.

**Difficulties:**
- Hardware dependencies: GPIO, timing, and USB functions require hardware
- Velocity algorithm validation: Need to test time-based calculations
- Integration testing: Verifying full system behavior

**Solution:**
- Created comprehensive mock framework for Pico SDK functions
- Implemented standalone velocity calculation tests
- Used simulated timing values to test algorithm behavior
- Validated edge cases and boundary conditions

**Learning:**
- Test-driven development for embedded systems
- Mocking strategies for hardware-dependent code
- Importance of unit testing before integration
- Value of hardware-agnostic test suites

## Integration Challenges

### Hardware-Software Co-Design

**Challenge:**
Coordinating between hardware design (PCB, matrix layout) and firmware implementation (GPIO assignments, timing).

**Difficulties:**
- Pin assignment conflicts: Ensuring GPIO pins don't conflict with USB or other functions
- Timing constraints: Hardware propagation delays affecting software timing
- Iterative design: Changes in hardware requiring firmware updates

**Solution:**
- Early communication between hardware and software teams
- Documented pin assignments and timing requirements
- Modular firmware design allowing easy pin reconfiguration

**Learning:**
- Importance of early collaboration in hardware-software projects
- Value of clear documentation and specifications
- Modular design principles for maintainability

### System Integration

**Challenge:**
Integrating firmware (C), synthesizer (Python), and GUI components into a cohesive system.

**Difficulties:**
- Communication protocols: Ensuring MIDI messages are correctly interpreted
- Error handling: Graceful degradation when components fail
- User experience: Seamless operation from key press to audio output

**Solution:**
- Standard MIDI protocol for communication
- Comprehensive error handling and logging
- Auto-launch mechanisms for seamless startup

**Learning:**
- System integration methodologies
- Importance of standard protocols for component communication
- User experience considerations in technical projects

## Key Learnings and Skills Developed

### Technical Skills

1. **Embedded Systems Programming:**
   - C programming for microcontrollers
   - Real-time system design
   - Multicore programming and task scheduling
   - GPIO and peripheral management

2. **Hardware Interfacing:**
   - Digital communication protocols (shift registers)
   - Signal timing and synchronization
   - Hardware debugging techniques
   - PCB integration with firmware

3. **USB Protocol:**
   - USB device class specifications
   - USB descriptor configuration
   - MIDI over USB implementation
   - Cross-platform USB device development

4. **Audio Programming:**
   - Real-time audio synthesis
   - Polyphonic audio management
   - Thread-safe audio processing
   - Low-latency audio I/O

5. **Software Engineering:**
   - Python application development
   - Cross-platform software design
   - Testing methodologies
   - Documentation and code organization

### Engineering Process Skills

1. **Project Management:**
   - Breaking complex problems into manageable components
   - Iterative development and testing
   - Version control and collaboration
   - Documentation and knowledge transfer

2. **Problem Solving:**
   - Debugging complex systems
   - Analyzing timing and performance issues
   - Hardware-software troubleshooting
   - Edge case identification and handling

3. **System Design:**
   - Architecture planning for embedded systems
   - Separation of concerns
   - Modular design principles
   - Scalability considerations

4. **Testing and Validation:**
   - Unit testing strategies
   - Integration testing approaches
   - Hardware-agnostic testing
   - Performance validation

### Professional Development

1. **Technical Communication:**
   - Writing clear documentation
   - Explaining complex systems
   - Code comments and organization
   - Open source project preparation

2. **Collaboration:**
   - Working with hardware and software teams
   - Version control workflows
   - Code review practices
   - Knowledge sharing

3. **Industry Practices:**
   - Following standards and specifications
   - Using established libraries and frameworks
   - Professional code organization
   - Open source development practices

## Project Impact and Outcomes

### Technical Achievements

- Successfully implemented velocity-sensitive keyboard with 25 keys
- Achieved real-time polyphonic audio synthesis
- Created cross-platform solution (macOS, Windows, Linux)
- Developed comprehensive test suite
- Established clean, maintainable codebase

### Educational Value

- Gained deep understanding of embedded systems
- Learned real-time programming concepts
- Developed hardware-software integration skills
- Experienced full product development lifecycle
- Prepared professional-quality open source project

### Future Applications

- Foundation for more advanced MIDI controllers
- Extensible architecture for additional features
- Open source contribution to maker community
- Portfolio demonstration of embedded systems skills

## Conclusion

This capstone project provided comprehensive experience in embedded systems development, from low-level hardware interfacing to high-level software applications. The challenges encountered and overcome have significantly strengthened technical skills in embedded programming, real-time systems, and software engineering. The project demonstrates the integration of multiple engineering disciplines and the importance of systematic problem-solving approaches.

The lessons learned extend beyond technical skills to include project management, collaboration, and professional development practices. This experience has prepared for careers in embedded systems, firmware development, and related engineering fields.

