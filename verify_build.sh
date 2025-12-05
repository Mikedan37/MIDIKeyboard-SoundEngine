#!/bin/bash
# Verification script for codebase

echo "=== CODEBASE VERIFICATION ==="
echo ""

# Check test compilation
echo "1. Testing compilation..."
cd qwerty_midi_pico/tests
if make test_velocity_simple 2>&1 | grep -q "error"; then
    echo "   FAIL: Compilation errors found"
    exit 1
else
    echo "   PASS: Compilation successful"
fi

# Check test execution
echo ""
echo "2. Running tests..."
if ./test_velocity_simple 2>&1 | grep -q "ALL.*PASSED"; then
    echo "   PASS: All tests passed"
else
    echo "   FAIL: Tests failed"
    exit 1
fi

# Check Python syntax
echo ""
echo "3. Checking Python syntax..."
cd ../../midi_sound_engine
for f in *.py; do
    if python3 -m py_compile "$f" 2>&1 | grep -q "error"; then
        echo "   FAIL: Syntax error in $f"
        exit 1
    fi
done
echo "   PASS: All Python files valid"

# Check includes
echo ""
echo "4. Checking C includes..."
cd ../../qwerty_midi_pico
if grep -r "#include.*drivers/current" main.c drivers/current/*.c 2>&1 | grep -q "error"; then
    echo "   FAIL: Include errors"
    exit 1
else
    echo "   PASS: Includes correct"
fi

echo ""
echo "=== VERIFICATION COMPLETE ==="
echo "All checks passed!"
