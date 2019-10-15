# Advanced Tutorial #2 - Image processing and magnetic sensor
## Magnetic Sensor (Inductor)
### Set up of competition environment
The magnetic track consists of one insulated copper wire with its 2 ends connected to a signal genereator. The signal generator will provide a 100mA AC(Alternating Current) with 20kHz frequency (with some tolerance on the actual values).

![left hand rule](https://qph.fs.quoracdn.net/main-qimg-e85253038ac05d464101131fbf29770b)

When there is current, it will generate a magnetic field according to the left hand rule.
### How inductor works
![inductor](https://electronicsarea.com/wp-content/uploads/inductor-magnetic-field-lines-e1437088112274.gif)

The inductor is actually just a circular coil of wire. The varying magnetic field will induce emf to the coil to produce an opposing magnetic field. The induced emf ![E](https://latex.codecogs.com/gif.latex?E) would be inversely proportional to the distance ![r](https://latex.codecogs.com/gif.latex?r) between the wire and the inductor.
Other than the position of the inductor, the angle of the inductor relative to the wire also matters to the magnitude of induced emf since it can only induce emf when the coil **cuts through** the magnetic field. Simply put, assuming the position of the coil didn't moved, when the coil is **parallel** to the signal wire, it will **not** pick up any singal from the wire. And when it is **perpendicular** to the signal wire, the induced emf should be at its **maximum**. When you rotate the coil from 0deg to 90deg, the induced emf should be a sin curve.

Here we take ![theta](https://latex.codecogs.com/gif.latex?%5Ctheta) as the angle between the coil and the magtic field, ![h](https://latex.codecogs.com/gif.latex?h)  and ![x](https://latex.codecogs.com/gif.latex?x) to be the vertical and horizontal distance between the coil and the wire.

![first equation](https://latex.codecogs.com/gif.latex?E%5Cpropto%5Cfrac%7B1%7D%7Br%7Dsin%5Ctheta)

![second eqution](https://latex.codecogs.com/gif.latex?%24%24E%5Cpropto%5Cfrac%7B1%7D%7Br%7D*%5Cfrac%7Bh%7D%7Br%7D%24%24)

![thrid equation](https://latex.codecogs.com/gif.latex?%24%24E%5Cpropto%5Cfrac%7Bh%7D%7Bx%5E2&plus;h%5E2%7D%24%24)

### How ADC works
The induced emf will be passed to ADC (analog-to-digital converter) after hardware magification. The ADC would convert the signal to a 12-bit numerical representation by comparing it with a reference voltage. For our magnetic board, the reference voltage is 3.3V. 

### Using ADC
Here is the library functions that you have to use for getting ADC values.

#### *Initialize ADC*
```
void ADC_Start(void)
```
This function is used to initialize all of the adc in the board

#### *Get value from ADC channels*

In inc/adc.h, we can see that all of the values are recorded are in
```
extern volatile uint16_t ADC_Values[ADC_HISTORY_LEN][NUM_ADC_CHANNELS];
```
where `ADC_HISTORY_LEN` is the amount of data recorded for every adc, and `NUM_ADC_CHANNELS` is the amount of adc channels that is present (don't change this) . The data will be updated by itself!

### Classwork
1. Why the data we get from the sensor isn't 0 even though it's far away from the signal wire?
2. (Demo) Move the inductor around the signal wire to show the change in sensor data. (Output the data on the tft.)
3. (Demo) Give your idea on inductor positions for internal competition. Provide reasons for that.
