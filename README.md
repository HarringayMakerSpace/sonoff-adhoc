# sonoff-adhoc
Control a Sonoff switch with Wifi probe requests

## Why???

I was looking for a way to control a Sonoff switch from a Raspberry Pi Zero W [(for the voice controlled light switch project)](https://github.com/HarringayMakerSpace/voice-controlled-switch) but didn't want all the messing around configuring Wifi networks so came up with this approach of using Wifi probe requests to switch the Sonoff on and off.

This makes it similar to using 433 MHz remote control switches but using the built-in Wifi on the Pi Zero so it doesn't need the faf of the extra 433 Mhz transmitter module.

## What are Wifi probe requests?

You could use any mac address, but to avoid clashing with some other device its best to use one reserved for provate use, [see here for those](https://serverfault.com/questions/40712/what-range-of-mac-addresses-can-i-safely-use-for-my-virtual-machines). 


## The Sonoff Arduino sketch

## On the Raspberry Pi

To make this work on the Pi Zero it needs two things - one is to send Wifi probe requests and the other is to dynamically change the Pi's mac address which is sent on those probe requests. It turns out both of those are easy to do and require no configuration at all to a default Raspbian Stretch Lite install.

You can view the current mac address used by the Pi Zero with the ```ip link show``` command. On my Pi that gives:
```
ip link show wlan0
3: wlan0: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc pfifo_fast state DOWN mode DORMANT group default qlen 1000
    link/ether b8:27:eb:1b:0a:c7 brd ff:ff:ff:ff:ff:ff
```

You can change the mac address of the Pi with the ```ifconfig wlan0 hw ether <some mac>``` command. You need to use ```sudo``` so a complete command example is:
```
sudo ifconfig wlan0 hw ether 36:01:00:00:00:01
```

That done, to have the Pi Zero send a Wifi probe request you can use the command:
```
sudo iwlist wlan0 scan
```

So putting those altogether to controll the Sonoff switch: To switch it on:
```
sudo ifconfig wlan0 hw ether 36:01:00:00:00:01
sudo iwlist wlan0 scan
```
and to switch it off:
```
sudo ifconfig wlan0 hw ether 36:01:00:00:00:02
sudo iwlist wlan0 scan
```

You can do all that from Python on the Raspberry Pi using ```os.system``` or ```subprocess.call```, for example:
```
from subprocess import call

def on():
    call("sudo ifconfig wlan0 hw ether 36:01:22:33:44:01", shell=True)
    call("sudo iwlist wlan0 scan", shell=True)

def off():
    call("sudo ifconfig wlan0 hw ether 36:01:22:33:44:02", shell=True)
    call("sudo iwlist wlan0 scan", shell=True)
 
```
(it would be nice to suppress the scan results from the ```iwlist``` command, can anyone work out how to do that?) 
