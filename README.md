Nowadays, modern cars have a keyless-entry system. When the key comes close to the associated car, it will automatically unlock without the user needing to search for their key.

The goal of this project is to lock and unlock a car via a smartphone. In this way, older cars can also obtain a keyless-entry system. Naturally, a physical key is still required to actually start the car. There are two possible solutions for unlocking/locking. The car can automatically unlock or lock when the user's smartphone comes within close proximity. Alternatively, a smartphone application with a few buttons can be used to open or close the car. Due to time constraints, this project uses the second solution. As an extension, the first solution could be implemented in the future.

The approach begins by investigating how a car transmits its data. This includes examining the data that moves across the CAN bus when the car is locked or unlocked. Next, we investigate how we can use our microcontroller to send the same data independently over the CAN bus. Once this is achieved, a Bluetooth connection between a mobile device and the microcontroller will be considered. Finally, all components must work together as a single system. This means that when the smartphone is used, it sends a Bluetooth signal indicating what action to take (lock or unlock). The microcontroller then transmits the necessary data over the CAN bus to lock or unlock the car.

The report begins with a brief outline of the project to give a clear overview. The second chapter discusses the Nucleo board used. Next, it demonstrates how the necessary CAN frames for locking and unlocking the car were identified. Then, it explains how we can send this data ourselves over the CAN bus. Following this is a chapter about the Bluetooth connection between the smartphone and the HC-06 module. The final chapter explains the overall functioning of the project with the help of code.

![image](https://github.com/user-attachments/assets/5d0b5534-29c7-49cd-b271-9d65f32ea724)
![image](https://github.com/user-attachments/assets/51b9226d-7568-40c9-8bc2-653851837065)
![image](https://github.com/user-attachments/assets/8215c460-4a97-4c14-bfae-3867acc64163)
![image](https://github.com/user-attachments/assets/d4ddbf1b-f3cf-4bcf-b88d-ab3566450d3c)
