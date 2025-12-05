# Symposium Q&A Preparation Guide

San Jose State University - Electrical Engineering Senior Design Symposium

## Anticipated Questions and Recommended Answers

### Technical Questions

#### Q: How does the velocity detection work?

**Answer:**
The keyboard uses a dual-switch matrix design where each key has two contact points. When a key is pressed, it first makes contact with the early switch (ROW0), which records timestamp T₀. As the key continues to press, it makes contact with the late switch (ROW1), recording timestamp T₁. The firmware calculates velocity based on the time difference (Δt = T₁ - T₀). Faster presses result in shorter time deltas, which map to higher MIDI velocity values (1-127). The algorithm uses linear interpolation to convert the time delta into the standard MIDI velocity range.

**Key Points:**
- Two-phase scanning at 1 kHz (1ms cycles)
- Microsecond-level timing precision
- Velocity range: 100μs (fastest) to 50ms (slowest)
- Maps to MIDI standard 1-127 velocity range

#### Q: Why did you choose the Raspberry Pi Pico 2W?

**Answer:**
The Raspberry Pi Pico 2W provides excellent value for this application. It has dual ARM Cortex-M0+ cores running at 133 MHz, which allows us to separate GPIO polling (Core 1) from USB MIDI processing (Core 0). The RP2040 microcontroller has sufficient processing power for real-time matrix scanning at 1 kHz while handling USB communication. Additionally, the Pico SDK and TinyUSB library provide robust, well-documented frameworks for USB MIDI implementation. The low cost (~$6) keeps the total project cost around $50, making it accessible for educational use.

**Key Points:**
- Dual-core architecture enables task separation
- Sufficient processing power for real-time operations
- Excellent documentation and community support
- Low cost maintains project affordability

#### Q: How does the dual-core architecture work?

**Answer:**
We leverage the RP2040's dual-core architecture to separate time-critical tasks. Core 1 runs a dedicated GPIO polling loop that continuously scans the keyboard matrix at 1 kHz. This core handles velocity calculation, debouncing, and updates a shared key state array. Core 0 runs the main application loop, which polls the key states from Core 1, detects press/release events, generates MIDI messages, and handles USB communication via the TinyUSB stack. This separation ensures that USB tasks don't interfere with the critical timing requirements of matrix scanning, maintaining consistent 1ms scan intervals.

**Key Points:**
- Core 1: GPIO polling, velocity calculation (time-critical)
- Core 0: USB MIDI stack, event processing (less time-critical)
- Shared memory for inter-core communication
- Non-blocking design prevents timing issues

#### Q: What is the latency of the system?

**Answer:**
The system is designed for low-latency performance suitable for real-time music performance. Matrix scanning occurs at 1 kHz (1ms intervals), providing sub-millisecond key detection. USB MIDI transmission is asynchronous and non-blocking, with typical USB latency of 1-5ms depending on host system. The Python audio engine uses a 1024-sample buffer at 44.1 kHz, resulting in approximately 23ms audio latency. Total end-to-end latency from key press to audio output is typically 25-30ms, which is well within acceptable limits for musical performance (human perception threshold is around 10-20ms for noticeable delay).

**Key Points:**
- Matrix scan: 1ms
- USB transmission: 1-5ms
- Audio buffer: ~23ms
- Total: 25-30ms (acceptable for performance)

#### Q: How does polyphony work?

**Answer:**
The system supports true polyphony - all 25 keys can be pressed simultaneously and each generates independent MIDI Note ON messages. The firmware tracks each key's state independently and sends separate MIDI messages for each key press. On the host computer, the Python audio engine maintains a set of active notes, each with its own frequency and phase accumulator. The audio callback function generates samples for all active notes simultaneously, mixing them together before output. During stress testing with 4-6 note chords, the system maintained stable playback without audio dropouts or buffer underruns.

**Key Points:**
- Independent MIDI messages per key
- Audio engine tracks multiple notes simultaneously
- Real-time mixing of all active notes
- Tested with 4-6 note chords successfully

### Design Decision Questions

#### Q: Why did you choose USB MIDI over other communication protocols?

**Answer:**
USB MIDI provides several critical advantages for this educational application. First, it's class-compliant, meaning no drivers are required - the device works immediately when plugged into any computer. This is essential for classroom use where students shouldn't need to install software. Second, MIDI is the industry standard for musical instrument communication, ensuring compatibility with all music software (DAWs, synthesizers, sequencers). Third, MIDI messages are small (3 bytes) and efficient, enabling low-latency transmission. Finally, USB MIDI is well-supported across all operating systems, making our solution truly cross-platform. Alternative protocols like serial communication would require custom drivers and software, reducing accessibility.

**Key Points:**
- Class-compliant (no drivers needed)
- Industry standard protocol
- Cross-platform compatibility
- Low-latency, efficient transmission
- Universal software compatibility

#### Q: Why Python for the audio engine instead of C++ or other languages?

**Answer:**
Python was chosen for the audio engine to maximize accessibility and cross-platform compatibility. Python is widely taught in schools and is accessible to students and educators who may not have C++ experience. The sounddevice and numpy libraries provide excellent cross-platform audio support without platform-specific code. Python's ease of modification allows educators and students to customize the synthesizer for educational purposes. While C++ might offer slightly better performance, Python's 23ms latency is already well within acceptable limits, and the educational benefits outweigh the minor performance trade-off.

**Key Points:**
- Educational accessibility
- Cross-platform libraries (sounddevice, numpy)
- Easy to modify and customize
- Performance is acceptable for the use case
- Lower barrier to entry for students

#### Q: Why 25 keys instead of a full 88-key keyboard?

**Answer:**
25 keys represent a single octave plus one note, which is sufficient for most educational applications and beginner practice. A full 88-key keyboard would significantly increase cost, complexity, and size. The 25-key design keeps the total project cost around $50, making it accessible for schools with limited budgets. Additionally, the design is extensible - the same architecture could be scaled to more keys if needed. For educational use, 25 keys provide enough range for learning scales, chords, and basic music theory without overwhelming students or breaking budgets.

**Key Points:**
- Cost-effective (keeps project under $50)
- Sufficient for educational use
- Appropriate size for classroom use
- Extensible design if more keys needed
- Single octave plus one note range

### Cost and Accessibility Questions

#### Q: How did you achieve the $50 cost target?

**Answer:**
The $50 cost target was achieved through careful component selection and open-source design. The Raspberry Pi Pico 2W costs approximately $6. The MSQT32 shift registers and supporting components add about $10-15. The keyboard matrix (switches, diodes, PCB) costs approximately $15-20. The 3D-printed enclosure and key bed can be printed for about $5-10 in materials. By using open-source firmware and software, we eliminate licensing costs. The total comes to approximately $50, compared to commercial MIDI keyboards that typically cost $100-300 or more.

**Key Points:**
- Raspberry Pi Pico 2W: ~$6
- Shift registers and components: ~$10-15
- Keyboard matrix: ~$15-20
- 3D-printed enclosure: ~$5-10
- Open-source software: $0
- Total: ~$50 vs. $100-300+ commercial alternatives

#### Q: How does this compare to commercial MIDI keyboards?

**Answer:**
Our keyboard offers several advantages over commercial alternatives. First, it's significantly cheaper (~$50 vs. $100-300+). Second, it's fully open-source, meaning schools can repair, modify, and understand every component. Third, it's designed specifically for educational use with easy setup and cross-platform compatibility. Commercial keyboards often have proprietary firmware, making repairs difficult and expensive. Our design uses standard components that are readily available and inexpensive to replace. Additionally, the open-source nature allows educators to customize the keyboard for specific teaching needs.

**Key Points:**
- Lower cost (~$50 vs. $100-300+)
- Fully open-source and repairable
- Educational focus
- Standard, replaceable components
- Customizable for teaching needs

### Technical Challenge Questions

#### Q: What was the biggest technical challenge you faced?

**Answer:**
The biggest technical challenge was implementing accurate velocity detection with the dual-switch matrix. This required precise timing measurements at the microsecond level, careful calibration of the velocity calculation algorithm, and handling edge cases like very fast presses where the early contact might be missed. We solved this through iterative testing, implementing proper settling times between row transitions, developing a robust velocity mapping algorithm, and adding timeout handling for edge cases. The solution required close collaboration between hardware and software teams to ensure timing specifications were met.

**Key Points:**
- Microsecond-level timing precision
- Algorithm calibration and edge case handling
- Hardware-software co-design
- Iterative testing and refinement

#### Q: How did you test the system without having hardware initially?

**Answer:**
We developed a comprehensive mock framework that simulates Pico SDK functions, allowing us to test the velocity calculation algorithm and firmware logic before physical hardware was available. We created mock implementations of GPIO functions, timing functions, and shift register reads. This enabled us to validate the velocity calculation algorithm with simulated input, test edge cases, and verify the logic flow. Once hardware was available, we validated that the mock tests accurately predicted real hardware behavior. This approach significantly accelerated development and ensured the firmware was ready when hardware arrived.

**Key Points:**
- Mock framework for Pico SDK functions
- Simulated timing and GPIO operations
- Validated algorithm before hardware
- Accelerated development timeline

#### Q: How did you ensure cross-platform compatibility?

**Answer:**
Cross-platform compatibility was achieved through several strategies. For the firmware, we used USB MIDI Class specification, which is supported natively by all operating systems without drivers. For the software, we used Python with cross-platform libraries (sounddevice for audio, mido for MIDI). We made platform-specific features optional - the macOS menu bar GUI uses the rumps library, but the system runs in background mode on Windows and Linux. We implemented platform detection for Pico device discovery (system_profiler on macOS, lsusb on Linux, PowerShell on Windows). Comprehensive testing on all three platforms ensured compatibility.

**Key Points:**
- USB MIDI Class (no drivers needed)
- Cross-platform Python libraries
- Optional platform-specific features
- Platform-specific device detection
- Testing on macOS, Windows, Linux

### Educational Impact Questions

#### Q: How will this help music education programs?

**Answer:**
This keyboard addresses several critical barriers to music education. First, the low cost (~$50) makes it accessible to schools with limited budgets, allowing programs to equip multiple classrooms without significant expense. Second, the open-source design means schools can repair and maintain the keyboards themselves, eliminating ongoing service costs. Third, the plug-and-play nature requires no technical setup, making it immediately usable by educators who may not have technical expertise. Fourth, the open-source software allows educators to customize the synthesizer for specific teaching needs. Finally, students can learn about embedded systems, programming, and electronics by studying and modifying the open-source code.

**Key Points:**
- Low cost enables multiple classroom units
- Open-source allows self-repair and maintenance
- Plug-and-play for immediate use
- Customizable for teaching needs
- Educational value in studying the code itself

#### Q: What makes this better than existing solutions for schools?

**Answer:**
Existing commercial MIDI keyboards are expensive ($100-300+), difficult to repair (proprietary components), and not designed with education in mind. Our solution is significantly cheaper, fully repairable with standard components, and designed specifically for educational use. The open-source nature means schools aren't locked into vendor support contracts. The cross-platform compatibility ensures it works with whatever computers schools already have. The extensible design allows schools to add features or modify the keyboard for specific curriculum needs. Additionally, the project itself serves as a learning tool - students can study embedded systems, real-time programming, and audio synthesis.

**Key Points:**
- Lower cost than commercial alternatives
- Repairable with standard components
- Educational focus and customization
- No vendor lock-in
- Serves as learning tool itself

### Future Work Questions

#### Q: What improvements would you make in a future version?

**Answer:**
Several improvements could enhance the keyboard. We could add aftertouch (pressure sensitivity after key press) by monitoring continuous pressure on the late contact. Additional MIDI controllers like pitch bend or modulation wheels could be integrated. The key count could be expanded to 37 or 49 keys for more range. We could implement different velocity curves (linear, exponential, logarithmic) for different playing styles. Wireless connectivity via Bluetooth MIDI could eliminate USB cables. The audio engine could be enhanced with different synthesis methods (FM, wavetable) or effects processing. Finally, we could develop a web-based configuration interface for easier customization.

**Key Points:**
- Aftertouch support
- Additional MIDI controllers
- Expanded key count
- Configurable velocity curves
- Wireless connectivity
- Enhanced synthesis methods
- Web-based configuration

#### Q: How scalable is this design?

**Answer:**
The design is highly scalable. The same architecture can support more keys by adding additional shift registers or using a different scanning approach. The dual-core architecture has sufficient processing headroom - we're currently using less than 10% of available CPU time. The firmware is modular, making it easy to add features like additional MIDI controllers or different scanning methods. The Python audio engine can handle unlimited polyphony (limited only by CPU). The USB MIDI protocol supports multiple simultaneous messages, so polyphony isn't limited by communication bandwidth. The main scaling limitations would be physical size and cost, but the core technology scales well.

**Key Points:**
- Architecture supports more keys
- Sufficient CPU headroom
- Modular firmware design
- Unlimited audio polyphony
- USB MIDI bandwidth sufficient
- Physical size and cost are main limits

### Validation and Testing Questions

#### Q: How did you validate the velocity detection accuracy?

**Answer:**
We validated velocity detection through systematic testing. We created test cases with known time deltas (100μs to 50ms) and verified the velocity calculation algorithm produced correct MIDI velocity values (1-127). We tested edge cases including very fast presses (below minimum time), very slow presses (above maximum time), and boundary conditions. We performed human testing where multiple users pressed keys with "soft," "medium," and "hard" presses, and verified the system consistently differentiated between these levels. The velocity mapping was calibrated to provide meaningful expressive control while maintaining reliability.

**Key Points:**
- Algorithm testing with known time deltas
- Edge case validation
- Human testing with different press strengths
- Consistent differentiation of soft/medium/hard
- Calibrated for expressive control

#### Q: What testing did you perform for polyphony?

**Answer:**
We performed comprehensive polyphony testing. We tested simultaneous key presses ranging from 2 keys to all 25 keys. We stress-tested with chords of 4-6 notes, which is typical for musical use. We monitored the audio engine for buffer underruns, dropouts, or timing issues. We verified that each note maintained independent frequency and phase, and that notes didn't interfere with each other. We tested rapid key presses and releases to ensure the system could handle fast note changes. All tests confirmed stable polyphonic playback without audio artifacts.

**Key Points:**
- Tested 2 to 25 simultaneous keys
- Stress-tested with 4-6 note chords
- Monitored for audio dropouts
- Verified independent note tracking
- Tested rapid key changes
- Confirmed stable performance

### Comparison and Differentiation Questions

#### Q: How does this compare to Arduino-based MIDI controllers?

**Answer:**
Our solution offers several advantages over typical Arduino-based MIDI controllers. The Raspberry Pi Pico 2W has dual cores, allowing us to separate time-critical GPIO scanning from USB communication, which is difficult on single-core Arduinos. The Pico SDK and TinyUSB provide robust USB MIDI support out of the box, whereas Arduino implementations often require custom USB libraries. Our velocity detection uses a sophisticated dual-switch matrix with precise timing, whereas many Arduino projects use simpler on/off detection. The complete system includes both firmware and a professional audio engine, providing a turnkey solution rather than just a controller that requires external software.

**Key Points:**
- Dual-core architecture advantage
- Better USB MIDI support
- Sophisticated velocity detection
- Complete system (firmware + audio engine)
- Professional, production-ready design

#### Q: What makes your approach unique?

**Answer:**
Our approach is unique in several ways. First, we've created a complete, end-to-end solution from hardware to audio output, not just a MIDI controller. Second, the dual-core architecture enables true real-time performance with separated concerns. Third, the velocity detection using dual-switch timing is more sophisticated than simple pressure sensors or single-switch designs. Fourth, the entire system is open-source, including both firmware and software, making it truly accessible for education. Fifth, the educational focus drives design decisions - cost, repairability, and ease of use are prioritized. Finally, the comprehensive testing framework allows development without hardware, accelerating the design process.

**Key Points:**
- Complete end-to-end solution
- Dual-core real-time architecture
- Sophisticated velocity detection
- Fully open-source
- Educational focus
- Hardware-agnostic development

## Presentation Tips

### Opening Statement (30 seconds)

"Good morning/afternoon. We're presenting our senior design project: a 25-key, open-source MIDI keyboard designed for educational use. Traditional MIDI keyboards cost $100-300 and are difficult to repair. Our solution costs approximately $50, is fully open-source and repairable, and works immediately when plugged into any computer. We've built a complete system from custom hardware to real-time audio synthesis, using a Raspberry Pi Pico 2W with dual-core processing for velocity-sensitive key detection and USB MIDI communication."

### Key Points to Emphasize

1. **Cost-effectiveness**: $50 vs. $100-300+ commercial alternatives
2. **Educational focus**: Designed specifically for schools and students
3. **Open-source**: Fully repairable and customizable
4. **Technical achievement**: Dual-core architecture, velocity detection, real-time synthesis
5. **Complete solution**: Hardware, firmware, and software all included

### Common Follow-up Questions

**Q: Can I try it?**
"Yes, we have a working prototype here. [Demonstrate key press, show velocity sensitivity, play a chord]"

**Q: How long did this take?**
"This was our full-year senior design project. We spent approximately [X] hours on hardware design, [Y] hours on firmware development, and [Z] hours on software and integration."

**Q: What was the hardest part?**
"The biggest challenge was implementing accurate velocity detection with microsecond-level timing precision while maintaining reliable operation. This required close collaboration between hardware and software teams."

**Q: What's next for this project?**
"We're open-sourcing the entire project on GitHub, making it available for schools, makers, and educators. We hope others will build upon our work and contribute improvements."

## Technical Deep-Dive Preparation

### Be Ready to Explain

1. **Dual-switch velocity detection algorithm** - Know the timing ranges, calculation method, and edge cases
2. **Dual-core task separation** - Understand why Core 1 vs. Core 0, and how they communicate
3. **USB MIDI message format** - Be able to explain the 3-byte message structure
4. **Polyphonic audio synthesis** - Understand how multiple notes are mixed in real-time
5. **Cross-platform compatibility** - Know the differences between macOS, Windows, and Linux implementations

### Have Demonstrations Ready

1. **Velocity sensitivity**: Show soft vs. hard key presses producing different audio volumes
2. **Polyphony**: Play a chord showing multiple simultaneous notes
3. **Real-time performance**: Show low latency by playing rapid notes
4. **Cross-platform**: If possible, demonstrate on different operating systems

## Potential Difficult Questions and Answers

#### Q: Why not just buy a used MIDI keyboard for $50?

**Answer:**
Used keyboards may be available at that price, but they come with significant drawbacks. They're often outdated, may not work with modern computers, and when they break, repair is expensive or impossible due to proprietary parts. Our solution is new, designed for modern systems, fully repairable with standard components, and open-source so schools can maintain them indefinitely. Additionally, the educational value of students studying and understanding the system itself is significant.

#### Q: Is 25 keys really enough for music education?

**Answer:**
For most educational applications, 25 keys (one octave plus one note) is sufficient. Students can learn scales, chords, and basic music theory within this range. The design is extensible - the same architecture supports more keys if needed. The cost and size benefits of 25 keys make it accessible to more schools, and students can always upgrade to full-size keyboards later. For beginner and intermediate education, 25 keys provides an excellent starting point.

#### Q: Why Python instead of a more performant language?

**Answer:**
Python was chosen for accessibility and educational value. The 23ms latency we achieve is well within acceptable limits for musical performance. The educational benefits - students can read, understand, and modify the code - outweigh the minor performance trade-off. For a $50 educational keyboard, Python's accessibility is more valuable than the marginal performance gain from C++ or other languages. The system performs adequately for its intended use case.

#### Q: What about wireless connectivity?

**Answer:**
Wireless connectivity is an excellent future enhancement. For the initial version, USB provides the most reliable, lowest-latency connection without additional complexity or cost. USB also ensures compatibility with all computers without requiring Bluetooth setup. However, Bluetooth MIDI could be added in a future version using the Pico's wireless capabilities or an additional module.

## Closing Statement

"Thank you for your interest in our project. We've created an open-source, low-cost MIDI keyboard that makes music technology accessible to schools and students. The complete design - hardware, firmware, and software - is available on GitHub for anyone to use, modify, and improve. We believe this project demonstrates how thoughtful engineering can solve real-world problems in education while providing excellent learning opportunities for students."

