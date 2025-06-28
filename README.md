LED Automotive Display System
A dynamic LED matrix display system for automotive applications featuring sequential turn indicators, scrolling speed display, and cruise control status indicator.
🚗 Features

Sequential Turn Indicators: XUV700-style animated arrows that grow column-by-column from tip to base
Speed Display: Scrolling speed text with color-coded speed limit warnings
Cruise Control Indicator: Visual cruise control status with dedicated symbol
Dual Arrow Animation: Synchronized left and right turn signal arrows
Speed Limit Monitoring: Automatic color changes when exceeding speed limits
Configurable Scrolling: Support for left-to-right and right-to-left text scrolling

🛠️ Hardware Requirements

Microcontroller: Arduino Mega 2560
LED Strip: WS2812B addressable LED strip (256 LEDs)
Matrix Configuration: 32 columns × 8 rows (256 total LEDs)
Data Pin: Digital pin 3 (configurable)
Power Supply: 5V capable of handling 256 LEDs (~15A at full brightness)

📋 Pin Configuration
ComponentPinDescriptionLED DataPin 3WS2812B data inputPower5VLED strip powerGroundGNDCommon ground
🔧 Installation
Dependencies
Install the required library through Arduino IDE Library Manager:
FastLED by Daniel Garcia
Upload Process

Clone this repository
Open led_automotive_display.ino in Arduino IDE
Install the FastLED library
Select your board and port
Upload the code

⚙️ Configuration
Display Settings
cpp#define NUM_LEDS 256        // Total number of LEDs
#define DATA_PIN 3          // LED data pin
#define BRIGHTNESS 10       // LED brightness (0-255)
#define ROW 32             // Matrix columns
#define COL 8              // Matrix rows
Animation Timing
cpp#define SPEED 16           // Overall frame rate (ms)
#define TEXT_SPEED 55      // Text scrolling speed (ms)
#define ARROW_WIDTH 10     // Arrow width in columns
Scroll Direction
cpp#define LEFT_TO_RIGHT 0
#define RIGHT_TO_LEFT 1
#define DIRECTION LEFT_TO_RIGHT  // Change scrolling direction
Speed Monitoring
cppuint8_t currentSpeed = 55;   // Current speed value
uint8_t speedLimit = 50;     // Speed limit threshold
🎨 Display Layout
┌─────────────────────────────────┐
│ LEFT    │  SPEED &  │   RIGHT   │
│ ARROW   │  CRUISE   │   ARROW   │
│ (0-9)   │  (11-21)  │  (22-31)  │
└─────────────────────────────────┘
Color Scheme

Turn Arrows: Amber (120, 255, 0)
Speed Text:

Green when under speed limit
Red when over speed limit


Cruise Control: Always red
Background: Black (off)

🚀 Usage
Basic Operation
The display automatically cycles through:

Arrow OFF Phase (400ms): Both arrows are hidden
Arrow GROWING Phase (160ms): Arrows grow sequentially column-by-column
Arrow FULL Phase (240ms): Both arrows fully displayed

Speed Display

Speed value scrolls continuously in the center section
Color changes automatically based on speed limit comparison
Cruise control symbol (@) appears when active

Customization
Change Speed Value
cppcurrentSpeed = 65;  // Set new speed
Modify Speed Limit
cppspeedLimit = 60;    // Set new speed limit
Toggle Cruise Control
Add/remove the @ symbol in the display string to show/hide cruise control status.
📐 LED Matrix Wiring
For a 32×8 matrix using WS2812B strips:

Serpentine Layout: Each row alternates direction

Even rows (0,2,4,6): Left to right
Odd rows (1,3,5,7): Right to left


Physical Layout:
Row 0: [0] → [1] → [2] → ... → [7]
Row 1: [15] ← [14] ← [13] ← ... ← [8]
Row 2: [16] → [17] → [18] → ... → [23]
Row 3: [31] ← [30] ← [29] ← ... ← [24]


🔍 Code Structure
Key Functions

setup(): Initialize FastLED and display parameters
loop(): Main animation loop with timing control
updateDualArrowAnimation(): Handle sequential arrow animations
printScrollingText(): Manage text scrolling in both directions
displayLED(): Render text to LED matrix
displayBothArrows(): Draw complete arrow patterns

Animation States
cppenum ArrowState {
  ARROW_OFF,      // Arrows hidden
  ARROW_GROWING,  // Sequential growth animation
  ARROW_FULL      // Full arrows displayed
};
🎯 Performance Notes

Frame Rate: 62.5 FPS (16ms per frame)
Text Scroll Rate: ~18 chars/second
Arrow Animation: 160ms growth + 240ms hold + 400ms off = 800ms cycle
Memory Usage: Optimized for Arduino Uno compatibility

🚧 Troubleshooting
Common Issues
LEDs not lighting up:

Check power supply (5V, sufficient amperage)
Verify data pin connection
Ensure proper grounding

Wrong colors/patterns:

Check LED strip type (WS2812B vs WS2811)
Verify matrix dimensions (ROW/COL settings)
Confirm wiring follows serpentine pattern


