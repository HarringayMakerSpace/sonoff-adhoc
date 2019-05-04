# sonoff-adhoc
Enables non-ESP8266's to send point-to-point or broadcast messages to a Sonoff (or any other ESP8266 device) without needing a central Wifi access point. A kind of simple vendor neutral ESP-Now.

### (17th Nov update: see the advance config note at the bottom)

## Why???

I had some Sonoff switches lying around and was looking for a simple way to control them from a Raspberry Pi Zero W [(for the voice controlled light switch project)](https://github.com/HarringayMakerSpace/voice-controlled-switch), but I didn't want all the complexity around configuring Wifi networks, so came up with this approach of using Wifi probe requests to switch the Sonoff on and off.

This makes the Sonoff similarly easy to using as 433 MHz remote control switches but uses the built-in Wifi on the Raspberry Pi so it doesn't need the faf of the external 433 Mhz transmitter module.

## What are Wifi probe requests?

A probe request is a special message sent by a Wifi client to discover what Wifi access points are within range. Basically, the client sends a probe request message and then all access points that receive it respond saying they're there. Its how your Wifi devices - PC or phone etc - are able to show you a list of the available Wifi SSID's. The probe request message is un-encrypted, doesn't need to know anything at all about the access points, and it includes the mac address of the client, so by fiddling with the mac address you can broadcast a small amount of data.  

You could use anything you like for the mac address bytes, but to avoid clashing with some other real device its best to use something reserved for private use - [see here for those](https://serverfault.com/questions/40712/what-range-of-mac-addresses-can-i-safely-use-for-my-virtual-machines) - so that gives 5 and a half bytes of data to play with! For this I'm using a mac address that starts with 0x36, followed by one byte that identifies my Sonoff switch, which allows for up to 256 Sonoffs, and the last byte is the command to switch it on or off (and three more bytes spare - think of the possibilities!). 

## The Sonoff Arduino sketch

The Sonoff ESP8266 runs a simple Arduino sketch that just listens for the probe requests that contain a mac address that starts with 0x3601 and then switches the relay on or off based on the last byte of the mac address. It has a simple interupt handler on the Sonoff builtin button too, so that you can also switch it on and off by pressing the button. 

The code is [here](/SonoffWifiProbes). 

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
(don't worry about messing up your Pi, this is not retained over power off so switching the Pi off/on returns it to the default mac address)

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
    call("sudo ifconfig wlan0 hw ether 36:01:00:00:00:01", shell=True)
    call("sudo iwlist wlan0 scan > /dev/null", shell=True)

def off():
    call("sudo ifconfig wlan0 hw ether 36:01:00:00:00:02", shell=True)
    call("sudo iwlist wlan0 scan > /dev/null", shell=True)
 
```
(it would be nice to suppress the scan results from the ```iwlist``` command, can anyone work out how to do that?) 

## Thats it

Is this crazy? It seems to work well, its fast, reliable, and gets better range than the cheapo 433 MHz Tx module I've been using on another Pi Zero. 

What do you think? Raise an issue to give me your feedback, please star the Github project if you find it useful.

## Advanced config

Some experimentation shows its possible to further configure this so as to partition the broadcast data by ESSID, and this enables even more posibilities.

The Sonoff device has an SSID defined in the sketch, for example, [here](https://github.com/HarringayMakerSpace/sonoff-adhoc/blob/master/SonoffWifiProbes/SonoffWifiProbes.ino#L31), its "Sonoff1".

You can target a probe request to a particular SSID, and on the Pi you do that with the ```essid``` parameter of the ```iwlist``` command. So now the probe request messages are ignored by all access points except ones with the specified SSID. For example:
```
sudo iwlist wlan0 scan essid "Sonoff1"
```

So using that you could now do things like send the probe request to a specific device, or group the Sonoff's by SSID - "Downstairs", "Upstairs", "lights", or whatever you choose. 

Aditionally, using the targeted iwlist scan also then allows you to hide the Sonoff Access Points so that they don't cluter up the list of available Access Points shown on you Phone, PC, etc. You do that by setting the hidden parameter to "1" on the ```WiFi.softAP``` call, for example, changing the line [here](https://github.com/HarringayMakerSpace/sonoff-adhoc/blob/master/SonoffWifiProbes/SonoffWifiProbes.ino#L31) to: 
```
  WiFi.softAP("Sonoff1", "<notused>", 6, 1, 0);
```
