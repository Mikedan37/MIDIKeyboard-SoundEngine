#include "tonegen.h"     // ✅ Add this
#include <unistd.h>      // for usleep

int main() {
    tonegen_init();

    // Play a C major chord: C4, E4, G4
    start_note(261.63);  // C
    start_note(329.63);  // E
    start_note(392.00);  // G

    usleep(1000000);     // hold for 1 second

    stop_note(261.63);
    stop_note(329.63);
    stop_note(392.00);

    return 0;
}