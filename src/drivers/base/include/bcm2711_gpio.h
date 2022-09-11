#define GPIO_MAX_PIN    53

// Register Offsets
#define GPFSEL_REG      0x00    // Pin Function Select
#define GPFSET_REG      0x07    // Pin Output Set 
#define GPCLR_REG       0x0A    // Pin Output Clear
#define GPLEV_REG       0x0D    // Pin Level 
#define GPED_REG        0x10    // Pin Event Detcet Status
#define GPREN_REG       0x13    // Pin Rising Edge Dectect Enable
#define GPFEN_REG       0x16    // Pin Falling Edge Detect Enable
#define GPHEN_REG       0x19    // Pin High Detect Enable
#define GPLEN_REG       0x1C    // Pin Low Detect Enable
#define GPAREN_REG      0x1F    // Pin Async. Rising Edge Detect
#define GPAFEN_REG      0x22    // Pin Async. Falling Edge Detect
#define GPPULL_REG      0x39    // Pin Pull-up/Pull-down

// Register Field Size (Number of Bits per GPIO line)
#define GPFSEL_FIELD_SIZE   3
#define GPFSET_FIELD_SIZE   1
#define GPCLR_FIELD_SIZE    1
#define GPLEV_FIELD_SIZE    1
#define GPED_FIELD_SIZE     1
#define GPREN_FIELD_SIZE    1
#define GPFEN_FIELD_SIZE    1
#define GPHEN_FIELD_SIZE    1
#define GPLEN_FIELD_SIZE    1
#define GPAREN_FIELD_SIZE   1
#define GPAFEN_FIELD_SIZE   1
#define GPPULL_FIELD_SIZE   2

// GPIO Function Selections

#define GPIO_FUNCTION_INPUT     0x00
#define GPIO_FUNCTION_OUTPUT    0x01
#define GPIO_FUNCTION_ALT0      0x04
#define GPIO_FUNCTION_ALT1      0x05
#define GPIO_FUNCTION_ALT2      0x06
#define GPIO_FUNCTION_ALT3      0x07
#define GPIO_FUNCTION_ALT4      0x03
#define GPIO_FUNCTION_ALT5      0x02

// GPIO Pull-up/Pull-down Selection
#define GPIO_PULL_NONE          0x00
#define GPIO_PULL_UP            0x01
#define GPIO_PULL_DOWN          0x02 
