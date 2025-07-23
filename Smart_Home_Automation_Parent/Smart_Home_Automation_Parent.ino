#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ESPmDNS.h>
#include <time.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <DHT.h>

const char *ssid = "POCO M3";
const char *password = "********";

char website[] PROGMEM = R"=====(

<!DOCTYPE html>
<html>
	<head>
		<meta charset="UTF-8">
		<title>Home Connect</title>
		<style>
			body {
			font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
			background: #1a1a1a;
			color: #ffffff;
			margin: 0;
			padding: 20px;
			display: flex;
			justify-content: center;
			}
			.container {
			max-width: 1000px;
			width: 90%;
			display: flex;
			flex-wrap: wrap;
			gap: 20px;
			padding: 20px;
			background: #252525;
			border-radius: 20px;
			box-shadow: 0 10px 20px rgba(0, 0, 0, 0.5);
			}
			.card {
			background: rgba(40, 40, 40, 0.9);
			padding: 20px;
			border-radius: 15px;
			flex: 1 1 calc(33.333% - 20px);
			box-shadow: 0 8px 16px rgba(0, 0, 0, 0.4);
			color: #ffffff;
			transition: transform 0.2s ease;
			}
			.card:hover {
			transform: scale(1.03);
			}
			.date-card {
			flex: 1 1 100%;
			max-width: 100%;
			text-align: center;
			}
			h1 {
			text-align: center;
			color: #76c7c0;
			width: 100%;
			font-size: 2.5em;
			font-weight: bold;
			}
			h2 {
			font-size: 1.5em;
			border-bottom: 2px solid #76c7c0;
			padding-bottom: 10px;
			margin-bottom: 15px;
			color: #76c7c0;
			}
			.switch {
			position: relative;
			display: inline-block;
			width: 70px;
			height: 38px;
			}
			.switch input {
			opacity: 0;
			width: 0;
			height: 0;
			}
			.ontimer {
			position: relative;
			width: 140px;
			display: flex;
			justify-content: center;
			align-items: center;
			height: 38px;
			background-color: #464545;
			font-weight: bold;
			border-radius: 5px;
			text-align: center;
			-webkit-user-select: none;
			-moz-user-select: none;
			-ms-user-select: none;
			user-select: none;
			}
			.timer {
			position: relative;
			padding: 0 1rem;
			display: flex;
			justify-content: center;
			align-items: center;
			height: 38px;
			background-color: #666;
			font-weight: bold;
			border-radius: 5px;
			text-align: center;
			cursor: pointer;
			transition: transform 0.1s, background-color 0.3s;
			-webkit-user-select: none;
			-moz-user-select: none;
			-ms-user-select: none;
			user-select: none;
			}
			.timer:hover {
			background-color: #76c7c0;
			color: white;
			}
			.timer:active {
			transform: scale(0.95);
			}
			.slider {
			position: absolute;
			cursor: pointer;
			top: 0;
			left: 0;
			right: 0;
			bottom: 0;
			background-color: #666;
			transition: 0.4s;
			border-radius: 38px;
			}
			.slider:before {
			position: absolute;
			content: "";
			height: 30px;
			width: 30px;
			left: 4px;
			bottom: 4px;
			background-color: white;
			transition: 0.4s;
			border-radius: 50%;
			}
			input:checked+.slider {
			background-color: #76c7c0;
			}
			input:checked+.slider:before {
			transform: translateX(32px);
			}
			.reading {
			display: flex;
			justify-content: space-between;
			align-items: center;
			padding: 10px;
			background: rgba(255, 255, 255, 0.05);
			margin: 10px 0;
			border-radius: 10px;
			color: #cccccc;
			}
			.value {
			font-weight: bold;
			color: #76c7c0;
			}
			.threshold-table {
			width: 100%;
			border-collapse: collapse;
			}
			.slider-label {
			color: #76c7c0;
			font-weight: bold;
			padding: 10px;
			width: 200px;
			}
			.slider-cell {
			padding: 10px;
			width: 100%;
			}
			input[type="range"] {
			width: 100%;
			height: 8px;
			appearance: none;
			background: #444;
			border-radius: 5px;
			outline: none;
			}
			input[type="range"]::-webkit-slider-thumb {
			appearance: none;
			width: 20px;
			height: 20px;
			background-color: #76c7c0;
			border-radius: 50%;
			cursor: pointer;
			transition: background-color 0.3s;
			}
			input[type="range"]::-moz-range-thumb {
			width: 20px;
			height: 20px;
			background-color: #76c7c0;
			border-radius: 50%;
			cursor: pointer;
			transition: background-color 0.3s;
			}
			.value {
			color: #76c7c0;
			font-weight: bold;
			text-align: right;
			padding: 10px;
			min-width: 70px;
			}
			.alarm {
			padding: 10px;
			border-radius: 5px;
			text-align: center;
			font-weight: bold;
			margin-top: 15px;
			color: white;
			background-color: #28a745;
			transition: background-color 0.5s ease;
			}
			.alarm.active {
			background-color: #ff4444;
			animation: blink 1s infinite;
			}
			@keyframes blink {
			50% {
			opacity: 0.5;
			}
			}
			@media (max-width: 768px) {
			.card {
			flex: 1 1 100%;
			}
			.slider-container {
			max-width: 100%;
			}
			}
		</style>
	</head>
	<body>
		<div class="container">
			<h1>Home Connect</h1>
			<div class="card date-card">
				<h1 id="dateTime">Date & Time</h1>
			</div>
			<div class="card">
				<h2>Sensor Readings</h2>
				<div class="reading">
					<span>Temperature</span><span class="value"><span id="temperature">-</span>°C</span>
				</div>
				<div class="reading">
					<span>Humidity</span><span class="value"><span id="humidity">-</span>%</span>
				</div>
				<div class="reading">
					<span>Heat Index</span><span class="value"><span id="heatIndex">-</span>°C</span>
				</div>
				<div class="reading">
					<span>Gas Level</span><span class="value"><span id="gas">-</span> Rs/R0</span>
				</div>
				<div class="reading">
					<span>Light Intensity</span><span class="value"><span id="light">-</span>%</span>
				</div>
				<div class="reading">
					<span>Soil Moisture</span><span class="value"><span id="soil">-</span>%</span>
				</div>
				<div id='gasAlarm' class='alarm inactive'>Gas Status: Normal</div>
				<div id='securityAlarm' class='alarm inactive'>Security Status: Normal</div>
				<div id='laundryAlarm' class='alarm inactive'>Laundry Status: Idle</div>
			</div>
			<div class="card">
				<h2>Thresholds</h2>
				<table class="threshold-table">
					<tr>
						<td class="slider-label">Temperature Threshold</td>
						<td class="slider-cell">
							<input type="range" id="tempThreshold" min="0" max="40" value="20"
								oninput="updateDisplayedValue(1, this.value)" onchange="updateThreshold(1, this.value)" />
						</td>
						<td class="value" id="tempThresholdValue">20°C</td>
					</tr>
					<tr>
						<td class="slider-label">Humidity Threshold</td>
						<td class="slider-cell">
							<input type="range" id="humidityThreshold" min="0" max="100" value="50"
								oninput="updateDisplayedValue(2, this.value)" onchange="updateThreshold(2, this.value)" />
						</td>
						<td class="value" id="humidityThresholdValue">50%</td>
					</tr>
					<tr>
						<td class="slider-label">Light Intensity Threshold</td>
						<td class="slider-cell">
							<input type="range" id="lightThreshold" min="0" max="100" value="50"
								oninput="updateDisplayedValue(3, this.value)" onchange="updateThreshold(3, this.value)" />
						</td>
						<td class="value" id="lightThresholdValue">50%</td>
					</tr>
					<tr>
						<td class="slider-label">Soil Moisture Threshold</td>
						<td class="slider-cell">
							<input type="range" id="soilThreshold" min="0" max="100" value="40"
								oninput="updateDisplayedValue(4, this.value)" onchange="updateThreshold(4, this.value)" />
						</td>
						<td class="value" id="soilThresholdValue">40%</td>
					</tr>
				</table>
			</div>
			<div class="card">
				<h2>Controls</h2>
				<div class="reading">
					<span>Fan Control</span>
					<span class="value">
						<div class="ontimer" id="fan-ontimer">00 : 00 : 00</div>
					</span>
					<label class="switch"><input type="checkbox" id="fanSwitch"
						onchange="toggleDevice(1,this.checked)"/><span class="slider"></span></label>
				</div>
				<div class="reading">
					<span>Light Control </span>
					<span class="value">
						<div class="ontimer" id="light-ontimer">00 : 00 : 00</div>
					</span>
					<label class="switch"><input type="checkbox" id="lightSwitch"
						onchange="toggleDevice(2,this.checked)"/><span class="slider"></span></label>
				</div>
				<div class="reading">
					<span>Motor Control </span>
					<span class="value">
						<div class="ontimer" id="motor-ontimer">00 : 00 : 00</div>
					</span>
					<label class="switch"><input type="checkbox" id="motorSwitch"
						onchange="toggleDevice(3,this.checked)" /><span class="slider"></span></label>
				</div>
				<div class="reading">
					<span>Geyser Control</span>
					<span class="value">
						<div class="ontimer" id="geyser-ontimer">00 : 00 : 00</div>
					</span>
					<label class="switch"><input type="checkbox" id="geyserSwitch"
						onchange="toggleDevice(4,this.checked)" /><span class="slider"></span></label>
				</div>
				<div class="reading">
					<span>Motor timer</span>
					<span class="value">
						<div class="timer" onclick="setTimer(2)" id="motor-timer">Set Timer</div>
					</span>
				</div>
				<div class="reading">
					<span>Light Timer</span>
					<span class="value">
						<div class="timer" onclick="setTimer(1)" id="light-timer">Set Timer</div>
					</span>
				</div>
			</div>
			<div class="card">
				<h2>Outside Conditions</h2>
				<div class="reading">
					<span>Temperature</span><span class="value"><span id="outsideTemperature">-</span>°C</span>
				</div>
				<div class="reading">
					<span>Humidity</span><span class="value"><span id="outsideHumidity">-</span>%</span>
				</div>
				<div class="reading">
					<span>Air Quality</span><span class="value"><span id="outsideAQI">-</span></span>
				</div>
				<!-- <div class="reading">
					<span>UV index</span><span class="value">Normal<span id="uv">
					</div>
					
					<div class="reading">
					<span>Chances of Rain</span><span class="value"><span id="rain">-</span>%</span>
					</div> -->
			</div>
		</div>
		<script>
			let timerIntervals = {};
			function updateTimer(startepochTime, timerSelected) {
			    function pad(num) {
			        return num.toString().padStart(2, '0');
			    }
			
			    function updateDateTime() {
			        const now = new Date();
			        const diffSeconds = Math.floor((now.getTime() / 1000) - startepochTime);
			
			        const hours = Math.floor(diffSeconds / 3600);
			        const minutes = Math.floor((diffSeconds % 3600) / 60);
			        const seconds = diffSeconds % 60;
			
			        const formattedTime = `${pad(hours)} : ${pad(minutes)} : ${pad(seconds)}`;
			        document.getElementById(timerSelected).innerText = formattedTime;
			    }
			
			    updateDateTime();
			    if (timerIntervals[timerSelected]) {
			        clearInterval(timerIntervals[timerSelected]);
			    }
			    timerIntervals[timerSelected] = setInterval(updateDateTime, 1000);
			}
			
			function stopTimer(timerSelected) {
			    if (timerIntervals[timerSelected]) {
			        clearInterval(timerIntervals[timerSelected]);
			        delete timerIntervals[timerSelected];
			        document.getElementById(timerSelected).innerText = "00 : 00 : 00";
			    }
			}
			
			var Socket;
			function init() {
			    Socket = new WebSocket("ws://" + window.location.hostname + ":81/");
			    Socket.onmessage = function (event) {
			        processCommand(event);
			    };
			    Socket.onclose = function () {
			        setTimeout(init, 2000);
			    };
			}
			function toggleDevice(device, state) {
			    if (Socket.readyState === WebSocket.OPEN) {
			        Socket.send(String.fromCharCode(device, state ? 1 : 0));
			        
			    }
			}
			function processCommand(event) {
			    try {
			        let full_data = event.data;
			        console.log("Received data" + full_data);
			        var data = JSON.parse(full_data);
			
			        document.getElementById("temperature").innerHTML = data.temperature.toFixed(2);
			        document.getElementById("humidity").innerHTML = data.humidity;
			        document.getElementById("heatIndex").innerHTML = data.heatIndex.toFixed(2);
			        document.getElementById("gas").innerHTML = data.gas.toFixed(2);
			        document.getElementById("light").innerHTML = data.lightIntensity;
			        document.getElementById("soil").innerHTML = data.soilMoisture;
			        document.getElementById('fanSwitch').checked = data.fanON;
			        document.getElementById('lightSwitch').checked = data.lightON;
			        document.getElementById('motorSwitch').checked = data.motorON;
			        document.getElementById('geyserSwitch').checked = data.geyserON;
			        document.getElementById('tempThresholdValue').innerHTML = data.temperatureThreshold + "°C";
			        document.getElementById('humidityThresholdValue').innerHTML = data.humidityThreshold + "%";
			        document.getElementById('lightThresholdValue').innerHTML = data.lightThreshold + "%";
			        document.getElementById('soilThresholdValue').innerHTML = data.soilThreshold + "%";
			        document.getElementById('tempThreshold').value = data.temperatureThreshold;
			        document.getElementById('humidityThreshold').value = data.humidityThreshold;
			        document.getElementById('lightThreshold').value = data.lightThreshold;
			        document.getElementById('soilThreshold').value = data.soilThreshold;
			
			        document.getElementById('outsideTemperature').innerHTML = data.outsideTemperature.toFixed(2);
			        document.getElementById('outsideHumidity').innerHTML = data.outsideHumidity;
			        document.getElementById('outsideAQI').innerHTML = data.outsideAQI;
			
			        if ((data.light.stopTimeHours - data.light.startTimeHours) || Math.abs(data.light.startTimeMinutes - data.light.stopTimeMinutes)) {
			            document.getElementById('light-timer').innerHTML = data.light.startTimeHours+" : "+ data.light.startTimeMinutes+ " to " + data.light.stopTimeHours+" : "+ data.light.stopTimeMinutes ;
			        }
			        else{
			            document.getElementById('light-timer').innerHTML = "Set Timer"
			        }
			        
			        if((data.motor.startTimeHours - data.motor.stopTimeHours) || Math.abs(data.motor.startTimeMinutes - data.motor.stopTimeMinutes) ){
			            document.getElementById('motor-timer').innerHTML = data.motor.startTimeHours+" : "+ data.motor.startTimeMinutes+ " to " + data.motor.stopTimeHours+" : "+ data.motor.stopTimeMinutes ;
			        }
			        else{
			            document.getElementById('motor-timer').innerHTML = "Set Timer"
			        }
			        
			        if(data.laundryRunning === 1){
			            document.getElementById('laundryAlarm').innerHTML = 'Laundry Status : Running';
			        }else{
			            document.getElementById('laundryAlarm').innerHTML ='Laundry Status : Done';
			        }
			
			        let gasAlarm = document.getElementById('gasAlarm');
			        if (data.gasLeaked === 1) {
			            gasAlarm.className = 'alarm active';
			            gasAlarm.innerHTML = 'WARNING: Gas Leak Detected!';
			        } else {
			            gasAlarm.className = 'alarm inactive';
			            gasAlarm.innerHTML = 'Gas Status: Normal';
			        }
			
			        if (data.securityBreach === 1) {
			            document.getElementById('securityAlarm').className = 'alarm active';
			            document.getElementById('securityAlarm').innerHTML = 'WARNING: Security Breached!';
			        } else {
			            document.getElementById('securityAlarm').className = 'alarm inactive';
			            document.getElementById('securityAlarm').innerHTML = 'Security Status: Normal';
			        }
			
			        if (data.geyserONTimer == 0) {
			            stopTimer("geyser-ontimer");
			        } else {
			            updateTimer(data.geyserONTimer, "geyser-ontimer");
			        }
			        if (data.motorONTimer == 0) {
			            stopTimer("motor-ontimer");
			        } else {
			            updateTimer(data.motorONTimer, "motor-ontimer");
			        }
			        if (data.fanONTimer == 0) {
			            stopTimer("fan-ontimer");
			        } else {
			            updateTimer(data.fanONTimer, "fan-ontimer");
			        }
			        if (data.lightONTimer == 0) {
			            stopTimer("light-ontimer");
			        } else {
			            updateTimer(data.lightONTimer, "light-ontimer");
			        }
			    } catch (e) {
			        console.error("Error parsing data:", e);
			    }
			}
			window.onload = function () {
			    init();
			};
			function updateDisplayedValue(type, value) {
			    switch (type) {
			        case 1:
			            document.getElementById("tempThresholdValue").innerHTML = value + "°C";
			            break;
			        case 2:
			            document.getElementById("humidityThresholdValue").innerHTML = value + "%";
			            break;
			        case 3:
			            document.getElementById("lightThresholdValue").innerHTML = value + "%";
			            break;
			        case 4:
			            document.getElementById("soilThresholdValue").innerHTML = value + "%";
			            break;
			    }
			}
			function updateThreshold(type, value) {
			    if (Socket.readyState === WebSocket.OPEN) {
			        Socket.send(String.fromCharCode(type + 10, parseInt(value) & 0xff, (parseInt(value) >> 8) & 0xff));
			    }
			}
			
			function updateDateTime() {
			    const now = new Date();
			    document.getElementById("dateTime").innerText = now.toLocaleString();
			}
			
			setInterval(updateDateTime, 1000);
			
			function setTimer(type) {
			    const popup = document.createElement('div');
			    popup.style.position = 'fixed';
			    popup.style.top = '50%';
			    popup.style.left = '50%';
			    popup.style.transform = 'translate(-50%, -50%)';
			    popup.style.background = '#333';
			    popup.style.color = '#fff';
			    popup.style.padding = '20px';
			    popup.style.borderRadius = '8px';
			    popup.style.boxShadow = '0px 0px 15px rgba(0, 0, 0, 0.5)';
			    popup.style.zIndex = '1000';
			    popup.style.width = '300px';
			
			    popup.innerHTML = `
			        <h3>Set Timer</h3>
			        <div style="margin-bottom: 15px;">
			            <h4>Start Time</h4>
			            <label for="start-hours">Hours:</label>
			            <input type="number" id="start-hours" min="0" max="23" placeholder="0" style="margin-bottom: 5px;"><br>
			            <label for="start-minutes">Minutes:</label>
			            <input type="number" id="start-minutes" min="0" max="59" placeholder="0" style="margin-bottom: 5px;"><br>
			            
			        </div>
			        <div style="margin-bottom: 15px;">
			            <h4>Stop Time</h4>
			            <label for="stop-hours">Hours:</label>
			            <input type="number" id="stop-hours" min="0" max="23" placeholder="0" style="margin-bottom: 5px;"><br>
			            <label for="stop-minutes">Minutes:</label>
			            <input type="number" id="stop-minutes" min="0" max="59" placeholder="0" style="margin-bottom: 5px;"><br>
			            
			        </div>
			        <button onclick="confirmTimer()">Set</button>
			        <button onclick="closePopup()">Cancel</button>
			    `;
			
			    document.body.appendChild(popup);
			
			    window.confirmTimer = function () {
			        const startHours = document.getElementById('start-hours').value || 0;
			        const startMinutes = document.getElementById('start-minutes').value || 0;
			       
			
			        const stopHours = document.getElementById('stop-hours').value || 0;
			        const stopMinutes = document.getElementById('stop-minutes').value || 0;
			        
			
			        if (
			            isNaN(startHours) || isNaN(startMinutes)|| isNaN(stopHours) || isNaN(stopMinutes)
			        ) {
			            alert('Please enter valid numbers.');
			        } else {
			            alert(`Start Time: ${startHours}h ${startMinutes}m \nStop Time: ${stopHours}h ${stopMinutes}m `);
			            closePopup();
			            if (Socket.readyState === WebSocket.OPEN) {
			                Socket.send(String.fromCharCode(type + 20, parseInt(startHours) & 0xff, parseInt(startMinutes) & 0xff, parseInt(stopHours) & 0xff, parseInt(stopMinutes) & 0xff));
			            }
			        }
			    };
			
			    window.closePopup = function () {
			        document.body.removeChild(popup);
			    };
			}
		</script>
	</body>
</html>

)=====";

int interval = 1000;
unsigned long previousMillis = 0;

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void webSocketEvent(byte num, WStype_t type, uint8_t *payload, size_t length);

void reconnectWiFi();

const char *ntpServer = "pool.ntp.org";
uint32_t epochTime;
struct tm timeinfo;

uint32_t getTime();

String openWeatherMapApiKey = "69b605ad3a4ca540a82b7e989927e883";

String city = "Allahabad";
String countryCode = "IN";

unsigned long weatherTime = 0;
unsigned long weatherTimeDelay = 600000;

String jsonBuffer;

String httpGETRequest(const char *serverName);
void getWeatherData();

float outsideTemperature;
float outsideHumidity;
float outsideAQI;

bool autoFanControl = true;
bool autoLightControl = true;
bool autoMotorControl = true;

uint8_t fanON = false;
uint8_t lightON = false;
uint8_t motorON = false;
uint8_t geyserON = false;
uint32_t fanONTimer;
uint32_t lightONTimer;
uint32_t motorONTimer;
uint32_t geyserONTimer;

struct timedONOFF
{
    uint8_t startTimeHours;
    uint8_t startTimeMinutes;
    uint8_t stopTimeHours;
    uint8_t stopTimeMinutes;
    uint8_t userPreferedStateON;
} light, motor;

float temperatureHysteresis = 2.0;
float humidityHysteresis = 5.0;
uint16_t lightIntensityHysteresis = 5;
uint16_t soilMoistureHysteresis = 5;

float temperature, humidity, heatIndex;
uint16_t lightIntensity;
uint16_t soilMoisture;

#define DHTPIN 32
#define DHTTYPE DHT11
#define FAN 25

DHT dht(DHTPIN, DHTTYPE);

float temperatureThreshold = 20;
float humidityThreshold = 50;

#define GAS 34
#define BUZZER 33

#define GAS_THRESHOLD 6.0

float R0 = 0.43;
float ratio;
uint8_t gasLeaked = false;

#define LDR 39
#define LIGHT 26

uint16_t LIGHT_INTENSITY_THRESHOLD = 50;

#define SOILMOISTURE 35
#define MOTOR 14

uint16_t SOILMOISTURE_THRESHOLD = 40;

#define GEYSER 27

uint8_t laundryRunning = false;
uint8_t securityBreach = false;

void setup()
{
    Serial.begin(115200);

    WiFi.begin(ssid, password);
    Serial.println("Establishing connection to WiFi with SSID: " + String(ssid));

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }
    Serial.print("Connected to network with IP address: ");
    Serial.println(WiFi.localIP());

    if (!MDNS.begin("homeconnect"))
    {
        Serial.println("Error setting up MDNS responder!");
        while (1)
        {
            delay(1000);
        }
    }
    Serial.println("mDNS responder started");

    server.on("/", []()
              { server.send_P(200, "text/html", website); });
    server.on("/update", []()
              {
                  if (server.hasArg("light"))
                  {
                      lightIntensity = server.arg("light").toInt();
                      server.send(200, "text/plain", "light updated to: " + lightIntensity);
                  }
                  if (server.hasArg("laundry"))
                  {
                      laundryRunning = server.arg("laundry").toInt();
                      server.send(200, "text/plain", "laundry updated to: " + laundryRunning);
                  }
                  if (server.hasArg("securityBreach"))
                  {
                      securityBreach = server.arg("securityBreach").toInt();
                      server.send(200, "text/plain", "securityBreach updated to: " + securityBreach);
                  } });
    server.begin();

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    getWeatherData();

    configTime(19800, 0, ntpServer);

    dht.begin();
    pinMode(FAN, OUTPUT);
    digitalWrite(FAN, !LOW);

    pinMode(GAS, INPUT);
    pinMode(BUZZER, OUTPUT);
    digitalWrite(BUZZER, LOW);

    pinMode(LDR, INPUT);
    pinMode(LIGHT, OUTPUT);
    digitalWrite(LIGHT, !LOW);

    pinMode(SOILMOISTURE, INPUT);
    pinMode(MOTOR, OUTPUT);
    digitalWrite(MOTOR, !LOW);

    pinMode(GEYSER, OUTPUT);
    digitalWrite(GEYSER, !LOW);

    light.userPreferedStateON = true;
    motor.userPreferedStateON = true;
}

void loop()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        reconnectWiFi();
    }
    server.handleClient();
    webSocket.loop();

    unsigned long now = millis();
    if ((unsigned long)(now - previousMillis) >= interval)
    {
        epochTime = getTime();
        Serial.println(epochTime);

        if ((millis() - weatherTime) > weatherTimeDelay)
        {
            getWeatherData();
            weatherTime = millis();
        }

        humidity = dht.readHumidity();
        temperature = dht.readTemperature();

        if (isnan(humidity) || isnan(temperature))
        {
            if (isnan(humidity))
                humidity = 0;
            if (isnan(temperature))
                temperature = 0;
            Serial.println(F("Failed to read from DHT sensor!"));
        }

        heatIndex = dht.computeHeatIndex(temperature, humidity, false);
        if (autoFanControl)
        {
            if (!fanON && (temperature > temperatureThreshold + temperatureHysteresis || humidity > humidityThreshold + humidityHysteresis))
            {
                fanON = true;
                digitalWrite(FAN, !HIGH);
                fanONTimer = epochTime;
            }
            else if (fanON && (temperature < temperatureThreshold - temperatureHysteresis && humidity < humidityThreshold - humidityHysteresis))
            {
                fanON = false;
                digitalWrite(FAN, !LOW);
                fanONTimer = 0;
            }
        }
        else
        {
            if ((temperature > temperatureThreshold || humidity > humidityThreshold) && fanON == true)
            {
                autoFanControl = true;
            }
            else if ((temperature <= temperatureThreshold && humidity <= humidityThreshold) && fanON == false)
            {
                autoFanControl = true;
            }
        }

        float sensor_volt = ((float)analogRead(GAS) / 4096) * 3.3;
        float RS_gas = (3.3 - sensor_volt) / sensor_volt;

        ratio = RS_gas / R0;
        ratio = ratio == INFINITY ? 10000.0 : ratio;
        if (gasLeaked && ratio > GAS_THRESHOLD)
        {
            gasLeaked = false;
            digitalWrite(BUZZER, LOW);
        }
        else if (!gasLeaked && ratio <= GAS_THRESHOLD)
        {
            gasLeaked = true;
            digitalWrite(BUZZER, HIGH);
        }

        lightIntensity = constrain(analogRead(LDR) * 0.044414, 0, 100);
        if (autoLightControl)
        {
            if (light.userPreferedStateON)
            {
                if (!lightON && lightIntensity < LIGHT_INTENSITY_THRESHOLD - lightIntensityHysteresis)
                {
                    lightON = true;
                    digitalWrite(LIGHT, !HIGH);
                    lightONTimer = epochTime;
                }
                else if (lightON && lightIntensity > LIGHT_INTENSITY_THRESHOLD + lightIntensityHysteresis)
                {
                    lightON = false;
                    digitalWrite(LIGHT, !LOW);
                    lightONTimer = 0;
                }
            }
            if (light.startTimeHours - light.stopTimeHours != 0 || light.startTimeMinutes - light.stopTimeMinutes != 0)
            {
                if (!lightON && timeinfo.tm_hour == light.startTimeHours && timeinfo.tm_min == light.startTimeMinutes)
                {
                    light.userPreferedStateON = true;
                    // lightON = true;
                    // digitalWrite(LIGHT, !HIGH);
                    lightONTimer = epochTime;
                }
                else if (lightON && timeinfo.tm_hour == light.stopTimeHours && timeinfo.tm_min == light.stopTimeMinutes)
                {
                    light.userPreferedStateON = false;
                    lightON = false;
                    digitalWrite(LIGHT, !LOW);
                    lightONTimer = 0;
                }
            }
        }
        else
        {
            if (lightIntensity < LIGHT_INTENSITY_THRESHOLD && lightON == true)
            {
                autoLightControl = true;
            }
            else if (lightIntensity >= LIGHT_INTENSITY_THRESHOLD && lightON == false)
            {
                autoLightControl = true;
            }
        }

        soilMoisture = 100.00 - analogRead(SOILMOISTURE) * 0.024414;
        if (autoMotorControl)
        {
            if (motor.userPreferedStateON)
            {
                if (!motorON && soilMoisture < SOILMOISTURE_THRESHOLD - soilMoistureHysteresis)
                {
                    motorON = true;
                    digitalWrite(MOTOR, !HIGH);
                    motorONTimer = epochTime;
                }
                else if (motorON && soilMoisture > SOILMOISTURE_THRESHOLD + soilMoistureHysteresis)
                {
                    motorON = false;
                    digitalWrite(MOTOR, !LOW);
                    motorONTimer = 0;
                }
            }
            if (motor.startTimeHours - motor.stopTimeHours != 0 || motor.startTimeMinutes - motor.stopTimeMinutes != 0)
            {
                if (!motorON && timeinfo.tm_hour == motor.startTimeHours && timeinfo.tm_min == motor.startTimeMinutes)
                {
                    motor.userPreferedStateON = true;
                    // motorON = true;
                    // digitalWrite(MOTOR, !HIGH);
                    motorONTimer = epochTime;
                }
                else if (motorON && timeinfo.tm_hour == motor.stopTimeHours && timeinfo.tm_min == motor.stopTimeMinutes)
                {
                    motor.userPreferedStateON = false;
                    motorON = false;
                    digitalWrite(MOTOR, !LOW);
                    motorONTimer = 0;
                }
            }
        }
        else
        {
            if (soilMoisture < SOILMOISTURE_THRESHOLD && motorON == true)
            {
                autoMotorControl = true;
            }
            else if (soilMoisture >= SOILMOISTURE_THRESHOLD && motorON == false)
            {
                autoMotorControl = true;
            }
        }

        JSONVar jsonData;

        jsonData["temperature"] = temperature;
        jsonData["humidity"] = humidity;
        jsonData["heatIndex"] = heatIndex;
        jsonData["gas"] = ratio;
        jsonData["lightIntensity"] = lightIntensity;
        jsonData["soilMoisture"] = soilMoisture;
        jsonData["fanON"] = fanON;
        jsonData["lightON"] = lightON;
        jsonData["motorON"] = motorON;
        jsonData["geyserON"] = geyserON;
        jsonData["fanONTimer"] = fanONTimer;
        jsonData["lightONTimer"] = lightONTimer;
        jsonData["motorONTimer"] = motorONTimer;
        jsonData["geyserONTimer"] = geyserONTimer;

        jsonData["light"]["startTimeHours"] = light.startTimeHours;
        jsonData["light"]["startTimeMinutes"] = light.startTimeMinutes;
        jsonData["light"]["stopTimeHours"] = light.stopTimeHours;
        jsonData["light"]["stopTimeMinutes"] = light.stopTimeMinutes;

        jsonData["motor"]["startTimeHours"] = motor.startTimeHours;
        jsonData["motor"]["startTimeMinutes"] = motor.startTimeMinutes;
        jsonData["motor"]["stopTimeHours"] = motor.stopTimeHours;
        jsonData["motor"]["stopTimeMinutes"] = motor.stopTimeMinutes;

        jsonData["gasLeaked"] = gasLeaked;
        jsonData["laundryRunning"] = laundryRunning;
        jsonData["securityBreach"] = securityBreach;
        jsonData["temperatureThreshold"] = temperatureThreshold;
        jsonData["humidityThreshold"] = humidityThreshold;
        jsonData["lightThreshold"] = LIGHT_INTENSITY_THRESHOLD;
        jsonData["soilThreshold"] = SOILMOISTURE_THRESHOLD;
        jsonData["outsideTemperature"] = outsideTemperature;
        jsonData["outsideHumidity"] = outsideHumidity;
        jsonData["outsideAQI"] = outsideAQI;

        String JSON_Data = JSON.stringify(jsonData);

        Serial.println(JSON_Data);
        webSocket.broadcastTXT(JSON_Data);
        previousMillis = now;
    }
}

void webSocketEvent(byte num, WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        Serial.println("Client " + String(num) + " disconnected");
        break;
    case WStype_CONNECTED:
        Serial.println("Client " + String(num) + " connected");
        break;
    case WStype_TEXT:
        if (length >= 2)
        {
            switch ((int)payload[0])
            {
            case 1:
                autoFanControl = false;
                fanON = (bool)payload[1];
                if (fanON && !fanONTimer)
                    fanONTimer = epochTime;
                else if (!fanON && fanONTimer)
                    fanONTimer = 0;
                digitalWrite(FAN, !fanON);
                Serial.print("Fan: ");
                Serial.println(fanON);
                break;
            case 2:
                autoLightControl = false;
                lightON = (bool)payload[1];
                if (lightON && !lightONTimer)
                    lightONTimer = epochTime;
                else if (!lightON && lightONTimer)
                    lightONTimer = 0;
                digitalWrite(LIGHT, !lightON);
                Serial.print("Light: ");
                Serial.println(lightON);
                break;
            case 3:
                autoMotorControl = false;
                motorON = (bool)payload[1];
                if (motorON && !motorONTimer)
                    motorONTimer = epochTime;
                else if (!motorON && motorONTimer)
                    motorONTimer = 0;
                digitalWrite(MOTOR, !motorON);
                Serial.print("Motor: ");
                Serial.println(motorON);
                break;
            case 4:
                geyserON = (bool)payload[1];
                digitalWrite(GEYSER, !geyserON);
                if (geyserON && !geyserONTimer)
                    geyserONTimer = epochTime;
                else if (!geyserON && geyserONTimer)
                    geyserONTimer = 0;
                Serial.print("Geyser: ");
                Serial.println(geyserON);
                break;
            case 11:
                if (length >= 3)
                {
                    temperatureThreshold = (float)((payload[2] << 8) | payload[1]);
                    Serial.print("New temperature threshold: ");
                    Serial.println(temperatureThreshold);
                }
                break;
            case 12:
                if (length >= 3)
                {
                    humidityThreshold = (float)((payload[2] << 8) | payload[1]);
                    Serial.print("New humidity threshold: ");
                    Serial.println(humidityThreshold);
                }
                break;
            case 13:
                if (length >= 3)
                {
                    LIGHT_INTENSITY_THRESHOLD = (uint16_t)((payload[2] << 8) | payload[1]);
                    Serial.print("New light intensity threshold: ");
                    Serial.println(LIGHT_INTENSITY_THRESHOLD);
                }
                break;
            case 14:
                if (length >= 3)
                {
                    SOILMOISTURE_THRESHOLD = (uint16_t)((payload[2] << 8) | payload[1]);
                    Serial.print("New soil moisture threshold: ");
                    Serial.println(SOILMOISTURE_THRESHOLD);
                }
                break;
            case 21:
                if (length >= 5)
                {
                    light.startTimeHours = (uint8_t)payload[1];
                    light.startTimeMinutes = (uint8_t)payload[2];
                    light.stopTimeHours = (uint8_t)payload[3];
                    light.stopTimeMinutes = (uint8_t)payload[4];
                    // Serial.print("New temperature threshold: ");
                    // Serial.println(temperatureThreshold);
                }
                break;
            case 22:
                if (length >= 5)
                {
                    motor.startTimeHours = (uint8_t)payload[1];
                    motor.startTimeMinutes = (uint8_t)payload[2];
                    motor.stopTimeHours = (uint8_t)payload[3];
                    motor.stopTimeMinutes = (uint8_t)payload[4];
                    // Serial.print("New temperature threshold: ");
                    // Serial.println(temperatureThreshold);
                }
                break;
            default:
                Serial.println("unknown data");
                break;
            }
            break;
        }
    }
}

String httpGETRequest(const char *serverName)
{
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName);

    int httpResponseCode = http.GET();

    String payload = "{}";

    if (httpResponseCode > 0)
    {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        payload = http.getString();
    }
    else
    {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    http.end();

    return payload;
}

void getWeatherData()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;

        jsonBuffer = httpGETRequest(serverPath.c_str());
        JSONVar myObject = JSON.parse(jsonBuffer);

        if (JSON.typeof(myObject) == "undefined")
        {
            Serial.println("Parsing input failed!");
            return;
        }

        // Convert temperature from Kelvin to Celsius
        outsideTemperature = JSON.stringify(myObject["main"]["temp"]).toFloat() - 273.15;
        outsideHumidity = JSON.stringify(myObject["main"]["humidity"]).toFloat();

        serverPath = "http://api.openweathermap.org/data/2.5/air_pollution?lat=25.45&lon=81.83&appid=" + openWeatherMapApiKey;

        jsonBuffer = httpGETRequest(serverPath.c_str());
        myObject = JSON.parse(jsonBuffer);

        if (JSON.typeof(myObject) == "undefined")
        {
            Serial.println("Parsing input failed!");
            return;
        }

        outsideAQI = JSON.stringify(myObject["list"][0]["main"]["aqi"]).toFloat();
    }
    else
    {
        Serial.println("WiFi Disconnected");
    }
}

uint32_t getTime()
{
    time_t now;
    if (!getLocalTime(&timeinfo))
    {
        // Serial.println("Failed to obtain time");
        return (0);
    }
    time(&now);
    return now;
}

void reconnectWiFi()
{
    Serial.println("WiFi connection lost. Attempting to reconnect...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nReconnected to WiFi");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("\nFailed to reconnect to WiFi");
    }
}
