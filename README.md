# Introduction to IoTmeshToy
The IoTmeshToy is a demonstration of the ESP8266 network capability. It creates a true mesh network, meaning that no-planning, central controller or router is required. Any system of one or more nodes will self-organize into fully functional mesh.

## How it works

Any node consists of a ESP8266 with a RGB LED connected to digital pins D1, D2 and D3 respectively. When the first node is turned on, it will do the turn-on animation for 2 seconds and after it, it will start searching for other nodes on the network. If any other node is found, it will join the existing network, otherwise it will continue to search undefinitly. When the second node is turned on, it will find the first node and they will negotiate the mesh network connection. Then they will start blinking two different colors using a fade-in and fade-out transition. After it, any new node that join the mesh network will have their own color and the nodes will blink according to the number of nodes presented on the network.

It is possible to change the colors if you connect to the wifi network created by the mesh network. There is a web server running on the router address.

Here is a video of the demo:

[IoTmeshToy](https://youtu.be/bqFQZIYITko)

This project is based on [demoToy example of the easyMesh library](https://github.com/Coopdis/easyMesh/tree/master/examples/demoToy), which unfortunately doesn't appear to be supported any more. To a better understanding of how it works, see the [demoToy video](https://www.youtube.com/watch?v=hqjOY8YHdlM). The concept are exactly the same.

## Dependencies

The following library are required for this project:
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
- [easyWebServer](https://github.com/Coopdis/easyWebServer)
- [easyWebSocket](https://github.com/Coopdis/easyWebSocket)
- [NeoPixelBus](https://github.com/Makuna/NeoPixelBus)
- [painlessMesh](https://gitlab.com/BlackEdder/painlessMesh)

## Uploading files to file system
If you need instructions on how to upload the files in data directory to the ESP8266, follow the [file system documentation](https://github.com/esp8266/Arduino/blob/master/doc/filesystem.md#uploading-files-to-file-system).
