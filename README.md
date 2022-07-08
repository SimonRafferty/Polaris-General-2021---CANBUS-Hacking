# Polaris-General-2021---CANBUS-Hacking
Useful tools &amp; Results from reverse engineering the canbus on my 2021 Polaris General XP1000 

I'm in the process of converting my Polaris General to Electric drive.
Why? I use it in connection with the movie industry & although the engine mages a great noise, it just makes way too much of it!
I've grown tired of people chasing after me asking me to switch it off as it's disturbing filming on a nearby stage.  Being Electric will also mean I can use it inside a stage - which on occasion will be very valuable.
It's also an interesting project!
If you think I'm an idiot because you love your General / RZR and want it to make even more noise, I'm delighted for you!

The first part of the conversion is to hack the CANBUS.  Without an engine, the dash will display the check engine light, no RPM and the fuel level gauge will be meaningless as the tank won't be there! Likewise for engine temperature.
I want to keep the dash (it's a cool dash!) so I'm going to interrupt the CAN lines where they come out of the ECU (I'm keeping the ECU) and filter the data with a bi-directional bridge.  I'll remove the messages I'm not interested in such as RPM but pass on everything else.  That means things like the ABS will continue to function as normal.  The messages I remove will be substituted with information from the electric drivetrain to drive the dashboard.
The Power Steering appears to operate when it can see an RPM message indicating 800rpm or above.

If you're not considering an electric conversion, the info here could prove useful if you wanted to build your own CAN driven dashboard, for example.  Or, perhaps you are considering putting a different engine in one and are wondering how to make the dash & power steering work?

Feel free to copy, share, change, eat - whatever you like!

There are two tools I've used for the reverse engineering:
CAN_Infrequently_Changing.ino
  This compiles a list of CAN messages received as well as keeping a count of how many times the
  data for that message has changed.  If more than 10 times (value seems to work OK), it no longer prints the 
  message data.

  If you leave it to settle for a couple of mins, so it is ignoring all the frequent update messages then clear
  the serial monitor window, anything that pops up has changed just then.

  This is ideal for discovering the codes for switches & things like that
  
CAN_RecordAndPlayback.ino
  This stores a block of 50 messages (all I could fit in an Arduino Nano).  When it's got 50, 
  it pauses for 30 sec then starts playing them back.  
  
  If you switch on the ignition, wait until it's got a block of 50, switch off the ignition then 
  unplug the ECU, when it starts playing back, you will see the results of what that block of messages do
  
  In the code, there are two lines Offset = 150; and GetCount = 50;
  Offset is the number of messages received before it starts filling up the block of data.  Start with 0, then 50, 100, 150 etc
  GetCount is the number of messages it will record.
  
  There were a few values, such as the fuel level which the first tool did not identify.  
  Keep repeating in blocks of 50 until you find the thing you're interested in, then keep reducing the size of GetCount 
  until you've narrowed it down to one message.
  It's a bit labourious - but can work really well!
  
CAN_Polaris_Read_Write.ino
  This encapsulates the results I've found so far (everything except the scrolling text messages sent to the dash)
  It prints data like this in the serial monitor:
  Gear=P|Speed=0|RPM=0|CEL=255|Fuel=112|4WD=2WD|DIFF=LOCK|Temp=17|Gas=25|PkBrk=APP|FtBrk=REL|Belt=SEC|Msg=
  Gear=P|Speed=0|RPM=0|CEL=255|Fuel=112|4WD=2WD|DIFF=LOCK|Temp=17|Gas=25|PkBrk=APP|FtBrk=REL|Belt=SEC|Msg=
  Gear=P|Speed=0|RPM=0|CEL=255|Fuel=112|4WD=2WD|DIFF=LOCK|Temp=17|Gas=25|PkBrk=REL|FtBrk=REL|Belt=SEC|Msg=

If you uncomment out the lines starting "CAN.sendMsgBuf", it will generate data for that function & send it on the Bus.  These will only be useful if you unplug the ECU, as the ECU will also be sending it's own versions of the data - which confuses the display.  Use these to spoof data from your Electric drive / new engine.
