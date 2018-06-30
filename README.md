# Embedded Systems Task
- Write c function to control one input and output (IO Pins) the input pin is a switch as interrupt once it pressed it should    lighting the led connected to output pin.if the switch pressed must send “pressed” to the serial monitor only one time also send the LED states ”ON” or “OFF”
- Program send continuous serial date comes from temperature sensor with 3 second rate .
# Setting the app
###### You can use the app using keil for debuggenig and simulation from Task.uvproj file included:
- Use tm4c123gh6pm with 16.0 MHz.
- Switch connected to pin PF0 , and led to pin PF1.
- Temperature Sensor connected to pin PE2.
- Serial monitor Connected to pin PA0.
# Assumptions made
- Use tm4c123hg6pm with 16.0 MHz.
- Use interrupt on portF pin PF0(switch) to light the led.
- Use Temp Sensor LM35.
- Use UART0 to send data to serial monitor.
- Use TIMER0 interrupt every 3 seconds to send temp reading to serial monitor.
- Use ADC0 to convert data from LM35.
# Issues
- There is some error in temperature readings.
