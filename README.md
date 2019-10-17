# arch-iot
_Architecture of Wireless Networks for IoT : Lab 1_

* Julien CAPELLARI
* Manar AGGOUN
* Roman GOUGE


The goal is to use new technologies : 
- Zolertia RE-Mote hardware 
- Contiki OS
- Virtual Machine 

*Exercice 1* 

     * Switch on the LED when the button is pressed.   
     * Switch off the LED when the button is pressed again.

*Exercice 2*

1. Switch on the LED when your Mote receives a message.  
      * Broadcast application
      * Unicast application
      
2. Connect your mote's battery to keep it far from your Sink mote (Mote connected to your host ). 
      * What is the maximum coverage of your wireless communication ?
 
3.  
     * How to increase the output power of your transmitter? 
     * Measure the new maximum coverage of your transmitter?
 
4. 
     * Can you change the radio frequency channel? How ?
     * What is the default channel used by the motes ?
 Set the new channel equal to 23. Compare the quality of transmission between the default channel and the new one.
     * Analyze your Packet Error Rate (PER) for each channel ?
 
5.
     * How can you transmit in the frequency band 868 MHz ? 
Run the example zolertia zoul /cc1200 demo and measure the new possible coverage of your zolertia
motes. 
    * What is the relation between the range and the frequency?
