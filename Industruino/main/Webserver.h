#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <SPI.h>
#include <Ethernet2.h>
#include "Datalogger.h"


// MAC address and IP address
// The IP address will be dependent on your local network
byte mac[] = { 0x00, 0x04, 0xA3, 0x4D, 0x6B, 0xA3 };
IPAddress ip(192, 168, 1, 177);

// port 80 is default for HTTP
EthernetServer server(80);

// temperatureData - temperature readings
// humidityData - humidity readings
// timestamp - timestamp of temperature and humidity data
// outputs[0-3] - boolean outputs values
// settings [0-3] - temperature, humidity, temperature hysteresis, humidity hysteresis values

void webserver(bool outputs[], int settings[]) {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    SerialUSB.println("NEW CLIENT");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        SerialUSB.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();

//////////////////////// Web Page //////////////////////////////
////////////////////////////////////////////////////////////////

// HEAD
client.println("<!DOCTYPE html>\n<html>\n  <head>\n    <title>Temperature and humidity regulator</title>\n    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n    <style>\n      body {\n        text-align: center; \n        font-family: Verdana, Geneva, Tahoma, sans-serif;\n      }\n\n      table {\n        margin: 40px;\n        border-collapse: collapse;\n        width: 400px;\n        float: left;\n      }\n      \n      th, td {\n        text-align: left;\n        padding: 8px;\n      }\n      \n      tr:nth-child(even){background-color: #f2f2f2}\n      \n      th {\n        background-color: #4CAF50;\n        color: white;\n      }\n\n      #chartsContainer{\n        width: 100%;\n        margin: auto;\n        overflow: hidden;\n        display: inline-block;\n        text-align:center;\n      }\n\n      #tempDiv, #humDiv {\n        display:inline-block;\n        width:600px;\n        margin: 10px;\n      }\n    </style>\n  </head>");

//BODY
client.println("<body>\n    <div>\n      <h1>Industruino Webserver</h1>\n      <h2>Temperature and humidity regulator</h2>\n    </div>\n    <hr />\n\n    <div style=\"display:inline-block; align-items: center;\">\n      <table>\n        <tr>\n          <th>Devices</th>\n          <th>Status</th>\n        </tr>\n        <tr>\n          <td>Heater</td>\n          <td>");
client.println(outputs[0] ? "ON" : "OFF");
client.println("</td>\n        </tr>\n        <tr>\n          <td>Cooler</td>\n          <td>");
client.println(outputs[1] ? "ON" : "OFF");
client.println("</td>\n        </tr>\n        <tr>\n          <td>Humidity adder</td>\n          <td>");
client.println(outputs[2] ? "ON" : "OFF");
client.println("</td>\n        </tr>\n        <tr>\n          <td>Humidity remover</td>\n          <td>");
client.println(outputs[3] ? "ON" : "OFF");
client.println("</td>\n        </tr>\n      </table>\n\n      <table>\n        <tr>\n          <th>Parameters</th>\n          <th>Values</th>\n        </tr>\n        <tr>\n          <td>Temperature</td>\n          <td>");
client.println(settings[0] / 10.0);
client.println(" C</td>\n        </tr>\n        <tr>\n          <td>Temperature hysteresis</td>\n          <td>");
client.println(settings[2] / 10.0);
client.println(" C</td>\n        </tr>\n        <tr>\n          <td>Humidity</td>\n          <td>");
client.println(settings[1] / 10.0);
client.println(" %</td>\n        </tr>\n        <tr>\n          <td>Humidity hysteresis</td>\n          <td>");
client.println(settings[3] / 10.0);
client.println(" %</td>\n        </tr>\n      </table>\n    </div><div id=\"chartsContainer\">\n      <div id=\"tempDiv\">\n        <canvas id=\"tempChart\"></canvas>\n      </div>\n      <div id=\"humDiv\">\n        <canvas id=\"humChart\"></canvas>\n      </div>\n    </div>\n\n    <script src=\"https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.9.3/Chart.bundle.js\" integrity=\"sha256-8zyeSXm+yTvzUN1VgAOinFgaVFEFTyYzWShOy9w7WoQ=\" crossorigin=\"anonymous\"></script>\n    <script>\n      var ctx = document.getElementById('tempChart').getContext('2d');\n      var ctx2 = document.getElementById('humChart').getContext('2d');\n      \n      var temperatureLineChart = new Chart(ctx, {\n        type: 'line',\n        data: {\n          labels: ");
client.println(timestamp);
client.println(",\n          datasets: [{\n            label: 'Sensors temperature',\n            backgroundColor: 'rgb(255, 0, 0)',\n            borderColor: 'rgb(255, 51, 51)',\n            data: ");
client.println(temperatureData);
client.println(",\n            fill: false,\n\t\t\t\t  }]\n\t\t\t  },\n\n        options: {\n          responsive: true,\n          title: {\n            display: true,\n            text: 'Temperature chart'\n          },\n          tooltips: {\n            mode: 'index',\n            intersect: false,\n          },\n          hover: {\n            mode: 'nearest',\n            intersect: true\n          },\n          scales: {\n            xAxes: [{\n              display: true,\n              scaleLabel: {\n                display: true,\n                labelString: 'Time'\n              }\n            }],\n            yAxes: [{\n              display: true,\n              scaleLabel: {\n                display: true,\n                labelString: 'Temperature (C)'\n              }\n            }]\n          }\n\t\t\t  }\n      });\n\n      var humidityLineChart = new Chart(ctx2, {\n        type: 'line',\n        data: {\n          labels: ");
client.println(timestamp);
client.println(",\n          datasets: [{\n            label: 'Sensors humidity',\n            backgroundColor: 'rgb(0, 0, 255)',\n            borderColor: 'rgb(51, 51, 255)',\n            data: ");
client.println(humidityData);
client.println(",\n            fill: false,\n\t\t\t\t  }]\n\t\t\t  },\n\n        options: {\n          responsive: true,\n          title: {\n            display: true,\n            text: 'Humidity chart'\n          },\n          tooltips: {\n            mode: 'index',\n            intersect: false,\n          },\n          hover: {\n            mode: 'nearest',\n            intersect: true\n          },\n          scales: {\n            xAxes: [{\n              display: true,\n              scaleLabel: {\n                display: true,\n                labelString: 'Time'\n              }\n            }],\n            yAxes: [{\n              display: true,\n              scaleLabel: {\n                display: true,\n                labelString: 'Humidity (%)'\n              }\n            }]\n          }\n\t\t\t  }\n      });\n    </script>\n  </body>\n</html>");

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
          
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    SerialUSB.println("CLIENT DISCONNECTED");
  }
}
        
#endif
