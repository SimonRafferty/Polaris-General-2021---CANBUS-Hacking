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
