
# UDP Server BME280 

The application creates UDP socket with the specified port number and waits for commands to be received.

## Commands

GET - returns json with measurement data after performing measurement.  

Get Response:

```
["OK", {"temperature":23.0, "humidity":43.122, "pressure":99980.3144}]
```
* temperature - Celcius
* humidity - Percent
* pressure - Decapascals

Invalid Command Response:

```
["FAIL",]
```


## How to use example

In order to create UDP client that communicates with UDP server example, choose one of the following options.

There are many host-side tools which can be used to interact with the UDP/TCP server/client. 
One command line tool is [netcat](http://netcat.sourceforge.net) which can send and receive many kinds of packets. 
Note: please replace `192.168.0.167 3333` with desired IPV4/IPV6 address (displayed in monitor console) and port number in the following commands.

In addition to those tools, simple Python scripts can be found under sockets/scripts directory. Every script is designed to interact with one of the examples.

### Send UDP packet via netcat
```
echo "GET" | nc -w1 -u 192.168.0.167 3333
```

### Receive UDP packet via netcat
```
echo "GET" | nc -w1 -u 192.168.0.167 3333
```

### UDP client using netcat
```
nc -u 192.168.0.167 3333
```

## Hardware Required

This example can be run on any commonly available ESP32 development board.

## Configure the project

```
idf.py menuconfig
```

Set following parameters under Example Configuration Options:

* Set `IP version` of the example to be IPV4 or IPV6.

* Set `Port` number that represents remote port the example will create.

To configure the example to use Wi-Fi, Ethernet or both connections, open the project configuration menu (`idf.py menuconfig`) and navigate to "Example Connection Configuration" menu. Select either "Wi-Fi" or "Ethernet" or both in the "Connect using" choice.

When connecting using Wi-Fi, enter SSID and password of your Wi-Fi access point into the corresponding fields. If connecting to an open Wi-Fi network, keep the password field empty.

When connecting using Ethernet, set up PHY type and configuration in the provided fields. If using Ethernet for the first time, it is recommended to start with the [Ethernet example readme](../ethernet/basic/README.md), which contains instructions for connecting and configuring the PHY. Once Ethernet example obtains IP address successfully, proceed to the protocols example and set the same configuration options.

## Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.