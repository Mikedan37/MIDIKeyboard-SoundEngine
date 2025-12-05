#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Mock velocity calculation constants
#define MIN_VELOCITY_TIME_US 100
#define MAX_VELOCITY_TIME_US 50000

// Velocity calculation function (standalone for testing)
uint8_t calculate_velocity(uint32_t time_delta_us) {
    if (time_delta_us < MIN_VELOCITY_TIME_US) {
        return 127;
    }
    if (time_delta_us > MAX_VELOCITY_TIME_US) {
        return 1;
    }
    
    uint32_t time_range = MAX_VELOCITY_TIME_US - MIN_VELOCITY_TIME_US;
    uint32_t velocity_range = 126;
    uint32_t normalized_time = time_delta_us - MIN_VELOCITY_TIME_US;
    uint8_t velocity = 127 - ((normalized_time * velocity_range) / time_range);
    
    if (velocity < 1) velocity = 1;
    if (velocity > 127) velocity = 127;
    
    return velocity;
}

void test_velocity_calculation(void) {
    printf("\n[TEST] Testing velocity calculation...\n");
    
    // Test maximum velocity (very fast press)
    uint8_t vel = calculate_velocity(100);
    assert(vel == 127);
    printf("  PASS: 100μs → Velocity %d (maximum)\n", vel);
    
    // Test minimum velocity (very slow press)
    vel = calculate_velocity(50000);
    assert(vel == 1);
    printf("  PASS: 50ms → Velocity %d (minimum)\n", vel);
    
    // Test medium time
    vel = calculate_velocity(10000);
    assert(vel >= 95 && vel <= 110);
    printf("  PASS: 10ms → Velocity %d (medium)\n", vel);
    
    // Test 1ms (fast press)
    vel = calculate_velocity(1000);
    assert(vel >= 120 && vel <= 127);
    printf("  PASS: 1ms → Velocity %d (fast)\n", vel);
    
    // Test 5ms (medium-fast)
    vel = calculate_velocity(5000);
    assert(vel >= 110 && vel <= 125);
    printf("  PASS: 5ms → Velocity %d (medium-fast)\n", vel);
    
    // Test edge cases
    vel = calculate_velocity(0);
    assert(vel == 127);
    printf("  PASS: Below minimum → Velocity %d\n", vel);
    
    vel = calculate_velocity(100000);
    assert(vel == 1);
    printf("  PASS: Above maximum → Velocity %d\n", vel);
    
    // Test velocity samples
    printf("  Velocity samples:\n");
    uint32_t times[] = {100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600};
    for (int i = 0; i < 9; i++) {
        vel = calculate_velocity(times[i]);
        printf("    %luμs → %d\n", (unsigned long)times[i], vel);
        assert(vel >= 1 && vel <= 127);
    }
    
    printf("[TEST] PASS: Velocity calculation PASSED\n");
}

void test_velocity_range(void) {
    printf("\n[TEST] Testing velocity range coverage...\n");
    
    bool has_low = false, has_mid = false, has_high = false;
    
    for (uint32_t time = 100; time <= 50000; time += 100) {
        uint8_t vel = calculate_velocity(time);
        if (vel >= 1 && vel <= 10) has_low = true;
        if (vel >= 60 && vel <= 70) has_mid = true;
        if (vel >= 120 && vel <= 127) has_high = true;
    }
    
    assert(has_low && has_mid && has_high);
    printf("  PASS: Velocity range covers low, mid, and high values\n");
    printf("[TEST] PASS: Velocity range PASSED\n");
}

void test_velocity_monotonicity(void) {
    printf("\n[TEST] Testing velocity monotonicity...\n");
    
    uint8_t prev_vel = 127;
    for (uint32_t time = 100; time <= 50000; time += 500) {
        uint8_t vel = calculate_velocity(time);
        assert(vel <= prev_vel);  // Velocity should decrease with time
        prev_vel = vel;
    }
    
    printf("  PASS: Velocity decreases monotonically with time\n");
    printf("[TEST] PASS: Velocity monotonicity PASSED\n");
}

int main(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════\n");
    printf("  VELOCITY CALCULATION TEST SUITE\n");
    printf("═══════════════════════════════════════════════════════\n");
    
    test_velocity_calculation();
    test_velocity_range();
    test_velocity_monotonicity();
    
    printf("\n");
    printf("═══════════════════════════════════════════════════════\n");
    printf("  ALL VELOCITY TESTS PASSED!\n");
    printf("═══════════════════════════════════════════════════════\n");
    printf("\n");
    
    return 0;
}

