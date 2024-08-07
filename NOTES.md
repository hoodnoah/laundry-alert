# Prototype Requirements

1. Gather data in a rolling window
2. Identify (w/ thresholding) active and inactive samples
3. Identify (w/ proportion) active and inactive windows
4. Physical lighting to indicate state
   - green for running
   - red for idle
   - flashing patterns for error states? e.g. IMU failed to initialize?
5. Server to receive and serve "Active/Inactive" state changes
