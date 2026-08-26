SEDS Aviation Inductions Task 1 and 2
Name: Priyankar Das
ID: 2026A4PS1208H


NOTE: Gemini and Claude have been used in these tasks. However I have tried to make sure that I understand the code and circuitry that has gone into this.


                                                                                                    /\   
                                                                                                   /__\  
                                                                                                  |    | 
                                                                                                  |    | 
                                                                                                  |____| 
                                                                                                  /|__|\ 
                                                                                                   /  \  
                                                                                                   ^^^^
                                                                                                 ^^^^^^^^
                                                                                               ^^^^^^^^^^^^


TASK 1: Sensor Data Visualization and Filtering
* I did python in +2 so this part was relatively easier for me :)
* Used libraries such as matplotlib, numpy and pandas for data manipulation and graph plotting.
* Filtered erratic values and random noise using the rolling median filter and exponential moving average (EMA) filter.
* Converted corrupted strings like #VALUE! into NaN and patched them using forward/backward propagation (ffill()/bfill()).
* Used the function FuncAnimation which simulates live data streaming by progressively slicing the dataset and updating existing line objects every 1000 ms.


TASK 2: Odysseus Monitoring System - State Machine
* For the tinkercad circuitry, I had 0 knowledge so watched the suggested videos. Also specifically watched documentation for each component such as lcd, distance sensor, photoresistor, pushbutton, potentiometer and arduino.
* I want to say that I almost gave up because of the tinkercad circuitry as 6-7 design copies were made and I was just burnt out. Thankfully tried again the next day and finally got it ^^
* Learnt and tried to understand the c++ code from scratch. This part was veru tuf :(
* Avoided using the delay() function to prevent the arduino from freezing, instead used the millis() function
* Full light corresponds to 1023 value, but I realised that 512 is not the halfway point as the ldr scale is not linear. Instead from manual inspection it came out to be around 83.
* There is one problem though: the light sensor by default starts at 0 which according to the logic should lead to storm state. However since in the tasklist it is written that it should start in open sea state, I have inverted the code logic so that 0 light value corresponds to open sea and above halfway point corresponds to storm. IRL perhaps we could label the 0 as max light value and vice versa.


Lastly, I wanted to thank the seniors for these tasks. It has been a difficult but rewarding journey
