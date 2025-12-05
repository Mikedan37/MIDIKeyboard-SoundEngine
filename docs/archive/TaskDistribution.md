midi-qwerty-keyboard/
 firmware/ # Zac's domain: Embedded C/Assembly, low-level drivers
 src/
 midi_driver.c # MIDI communication handling
 qwerty_driver.c # USB-HID driver for keyboard input
 usb_handler.c # USB communication logic
 main.c # Firmware entry point
 include/ # Header files for firmware
 Makefile # Build system for firmware
 platformio.ini # PlatformIO config (if using)

 hardware/ # PCB design files, schematics, and hardware specs
 schematics/ # KiCad/Eagle schematics
 pcb_design/ # PCB layout and design files
 bom/ # Bill of Materials (BOM)
 docs/ # Hardware design documentation

 software/ # Your domain: Higher-level programming (API, UI)
 api/ # API for user customization
 midi_api.py # Python API for MIDI customization
 qwerty_api.py # Python API for remapping keys
 api_server.py # Backend API server
 ui/ # Configuration interface
 app.py # GUI application for device settings
 components/ # UI elements (buttons, sliders, etc.)
 utils/ # Helper functions and scripts

 docs/ # Project documentation
 setup.md # Instructions for setting up the project
 firmware_notes.md # Notes on firmware development
 software_notes.md # Notes on API and UI development

 tests/ # Unit and integration tests
 firmware_tests/ # Tests for low-level firmware components
 software_tests/ # Tests for API and UI

 .gitignore # Ignore compiled files, dependencies, and temp files
 README.md # Main project documentation
 LICENSE # Open-source license
