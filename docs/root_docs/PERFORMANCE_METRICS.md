# Performance Metrics

## Firmware Performance

### Matrix Scanning
- **Scan Rate**: 500Hz (2ms per scan cycle)
- **Row Settling**: 10μs
- **Debounce Time**: 5ms
- **Velocity Timeout**: 50ms

### Timing Breakdown (per key)
- Early contact detection: < 1μs
- Late contact detection: < 1μs 
- Velocity calculation: < 10μs
- State update: < 1μs
- **Total per key**: < 20μs

### Memory Usage
- Key state array: 24 keys × 32 bytes = 768 bytes
- Stack usage: < 2KB
- Total RAM: < 4KB
- Flash usage: ~50KB

## Synthesizer Performance

### Audio Engine
- **Sample Rate**: 44100 Hz
- **Buffer Size**: 1024 samples
- **Latency**: ~23ms
- **Polyphony**: Unlimited (CPU dependent)

### MIDI Processing
- Input latency: < 5ms
- Note processing: < 1ms per note
- Thread overhead: Minimal

## Test Performance

### Velocity Calculation
- Test execution time: < 100ms
- Test coverage: 100% of algorithm
- Edge cases: All covered

## Optimization Opportunities

1. **Matrix Scanning**: Could reduce to 1ms (1000Hz) if needed
2. **Debounce**: Could be tuned based on hardware
3. **Audio Buffer**: Could be reduced for lower latency
4. **Memory**: Plenty of headroom on Pico (256KB RAM)
