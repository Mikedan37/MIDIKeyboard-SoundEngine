# Contributing to QWERTY MIDI Keyboard

Thank you for your interest in contributing! This document provides guidelines and instructions for contributing to the project.

## Code of Conduct

- Be respectful and inclusive
- Welcome newcomers and help them learn
- Focus on constructive feedback
- Celebrate diverse perspectives

## Getting Started

1. **Fork the repository**
2. **Clone your fork**:
 ```bash
 git clone https://github.com/your-username/qwerty-midi-keyboard.git
 cd qwerty-midi-keyboard
 ```
3. **Create a branch**:
 ```bash
 git checkout -b feature/your-feature-name
 ```

## Development Workflow

### For Firmware (C)

1. Make changes in `qwerty_midi_pico/`
2. Test your changes:
 ```bash
 cd qwerty_midi_pico/tests
 make test_velocity_simple
 ./test_velocity_simple
 ```
3. Build firmware:
 ```bash
 cd qwerty_midi_pico
 mkdir build && cd build
 cmake ..
 make
 ```

### For Synthesizer (Python)

1. Make changes in `midi_sound_engine/`
2. Test in virtual environment:
 ```bash
 cd midi_sound_engine
 python3 -m venv venv
 source venv/bin/activate
 pip install -r requirements.txt
 python engine.py
 ```

## Pull Request Process

1. **Update documentation** if you change functionality
2. **Add tests** for new features
3. **Ensure all tests pass**
4. **Update CHANGELOG.md** with your changes
5. **Create a pull request** with a clear description

### PR Checklist

- [ ] Code follows project style guidelines
- [ ] Tests added/updated and passing
- [ ] Documentation updated
- [ ] CHANGELOG.md updated
- [ ] No build warnings or errors

## Code Style

### C (Firmware)

- Use 4 spaces for indentation
- Function names: `snake_case`
- Constants: `UPPER_SNAKE_CASE`
- Add comments for complex logic
- Keep functions focused and small

```c
// Good
void velocity_matrix_scan(void) {
 // Clear, focused function
}

// Avoid
void doEverything(void) {
 // Too broad
}
```

### Python (Synthesizer)

- Follow PEP 8
- Use 4 spaces for indentation
- Docstrings for functions/classes
- Type hints where helpful

```python
def calculate_velocity(time_delta: float) -> int:
 """Calculate MIDI velocity from time delta.

 Args:
 time_delta: Time between contacts in microseconds

 Returns:
 MIDI velocity value (1-127)
 """
 ...
```

## Testing

### Adding Tests

1. Create test file: `qwerty_midi_pico/tests/test_<feature>.c`
2. Add to Makefile
3. Use assertions for validation
4. Document test purpose

### Running Tests

```bash
cd qwerty_midi_pico/tests
make test_velocity_simple
./test_velocity_simple
```

## Documentation

- Update README.md for user-facing changes
- Add code comments for complex logic
- Update relevant docs/ files
- Include examples in docstrings

## Reporting Bugs

Use GitHub Issues with:

- **Clear title**
- **Description** of the issue
- **Steps to reproduce**
- **Expected vs actual behavior**
- **Environment** (OS, hardware, versions)
- **Screenshots/logs** if applicable

## Suggesting Features

Open an issue with:

- **Use case**: Why is this needed?
- **Proposed solution**: How should it work?
- **Alternatives considered**: Other approaches?
- **Additional context**: Any relevant info

## Review Process

- Maintainers will review within 1-2 weeks
- Feedback will be constructive and specific
- Changes may be requested before merging
- All contributors are welcome to review PRs

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

## Recognition

Contributors will be:
- Listed in CONTRIBUTORS.md
- Credited in release notes
- Appreciated in the community!

## Questions?

- Open a GitHub Discussion
- Check existing issues/PRs
- Review documentation in `docs/`

Thank you for contributing!
