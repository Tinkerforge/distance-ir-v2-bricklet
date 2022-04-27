// This example is not self-contained.
// It requires usage of the example driver specific to your platform.
// See the HAL documentation.

#include "src/bindings/hal_common.h"
#include "src/bindings/bricklet_distance_ir_v2.h"

void check(int rc, const char *msg);
void example_setup(TF_HAL *hal);
void example_loop(TF_HAL *hal);

static TF_DistanceIRV2 dir;

void example_setup(TF_HAL *hal) {
	// Create device object
	check(tf_distance_ir_v2_create(&dir, NULL, hal), "create device object");

	// Get current distance
	uint16_t distance;
	check(tf_distance_ir_v2_get_distance(&dir, &distance), "get distance");

	tf_hal_printf("Distance: %d 1/%d cm\n", distance, 10);
}

void example_loop(TF_HAL *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
