// This example is not self-contained.
// It requires usage of the example driver specific to your platform.
// See the HAL documentation.

#include "src/bindings/hal_common.h"
#include "src/bindings/bricklet_distance_ir_v2.h"

void check(int rc, const char *msg);
void example_setup(TF_HAL *hal);
void example_loop(TF_HAL *hal);

// Callback function for distance callback
static void distance_handler(TF_DistanceIRV2 *device, uint16_t distance,
                             void *user_data) {
	(void)device; (void)user_data; // avoid unused parameter warning

	tf_hal_printf("Distance: %d 1/%d cm\n", distance, 10);
}

static TF_DistanceIRV2 dir;

void example_setup(TF_HAL *hal) {
	// Create device object
	check(tf_distance_ir_v2_create(&dir, NULL, hal), "create device object");

	// Register distance callback to function distance_handler
	tf_distance_ir_v2_register_distance_callback(&dir,
	                                             distance_handler,
	                                             NULL);

	// Set period for distance callback to 1s (1000ms) without a threshold
	tf_distance_ir_v2_set_distance_callback_configuration(&dir, 1000, false, 'x', 0, 0);
}

void example_loop(TF_HAL *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
