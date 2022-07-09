#define datain 21
#define shclk 23
#define stclk 22
#define ebene0 4
#define ebene1 5
#define ebene2 18
#define ebene3 19

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "diedreistendrei";
const char* password = "AlterWirSind4";

const char* PARAM_INPUT = "animation";

String animation = "cubeBounce";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>LED cube</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 6px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
    input:checked+.slider {background-color: #b30000}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>LED cube</h2>
  %BUTTONPLACEHOLDER%
<script>function toggleCheckbox(element) {
  if(element.checked){
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/update?animation="+element.id, true);
    xhr.send();
    }
}
</script>
</body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "BUTTONPLACEHOLDER"){
    String buttons = "";
    buttons += "<h4>Cube Bounce</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"cubeBounce\" " + animationState("cubeBounce") + "><span class=\"slider\"></span></label>";
    buttons += "<h4>Plane Bounce</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"planeBounce\" " + animationState("planeBounce") + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  return String();
}

String animationState(String animationName){
  if(animationName == animation){
    return "checked";
  }
  else {
    return "";
  }
}

//Numbers for layer patterns
uint16_t 
leftHalf = 33231,
rightHalf = 32304,
topHalf = 3135,
bottomHalf = 62400,
full = 65535,
QTopLeft = 15,
QTopRight = 3120,
QBotLeft = 33216,
QBotRight = 29184,
ring = 64855,
center = 680,
corners = 9474,
cross = 56061, //= cornersInv
leftColumn = 323,
leftColumnInv = 65212,
secLeftColumn = 32908,
secLeftColumnInv = 32627,
secRightColumn = 16944,
secRightColumnInv = 48591,
rightColumn = 15360,
rightColumnInv = 50175,
topRow = 1046,
topRowInv = 64489,
secTopRow = 2089,
secTopRowInv = 63446,
secBotRow = 4800,
seBotRowInv = 60735,
botRow = 57600,
botRowInv = 7935,
O = 55381,
X = 10154
;



uint16_t  //pictures consist of four numbers, one for each layer. Numbers can be calculated with excel sheet "LED_Cube_PinOut".
leer[4] = {0,0,0,0}, 
innerBlock[4] = {0, center, center, 0},
innerBlockInv[4] = {full, ring, ring, full},

block1[4] = {QTopLeft, QTopLeft, 0, 0},
block2[4] = {QTopRight, QTopRight, 0, 0},
block3[4] = {QBotLeft, QBotLeft, 0, 0},
block4[4] = {QBotRight, QBotRight, 0, 0},

block5[4] = {0, QTopLeft, QTopLeft, 0},
block6[4] = {0, QTopRight, QTopRight, 0},
block7[4] = {0, QBotLeft, QBotLeft, 0},
block8[4] = {0, QBotRight, QBotRight, 0},

block9[4] = {0, 0, QTopLeft, QTopLeft},
block10[4] = {0, 0, QTopRight, QTopRight},
block11[4] = {0, 0, QBotLeft, QBotLeft},
block12[4] = {0, 0, QBotRight, QBotRight},

plane0[4] = {full, 0, 0, 0},
plane1[4] = {0, full, 0, 0},
plane2[4] = {0, 0, full, 0},
plane3[4] = {0, 0, 0, full}

;

int Time = 100;

void setup() {
  Serial.begin(115200);
  
  pinMode(datain, OUTPUT);
  pinMode(shclk, OUTPUT);
  pinMode(stclk, OUTPUT);
  
  pinMode(ebene0, OUTPUT);
  pinMode(ebene1, OUTPUT);
  pinMode(ebene2, OUTPUT);
  pinMode(ebene3, OUTPUT);
  //turn all layers off
  digitalWrite(ebene0, LOW);
  digitalWrite(ebene1, LOW);
  digitalWrite(ebene2, LOW);
  digitalWrite(ebene3, LOW);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/update?output=<inputMessage>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/update?output=<inputMessage>
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      animation = inputMessage;
    }
    else {
      inputMessage = "No message sent";
    }
    Serial.print("animation set to: ");
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  server.begin();
}

void loop() {
  if(animation == "cubeBounce"){
    //top left falls down
    picture(block9, Time);
    picture(block5, Time);
    picture(block1, Time);
  
    //top right goes up
    picture(block2, Time);
    picture(block6, Time);
    picture(block10, Time);
  
    //bottom right falls down
    picture(block12, Time);
    picture(block8, Time);
    picture(block4, Time);
  
    //bottom left goes up
    picture(block3, Time);
    picture(block7, Time);
    picture(block11, Time);
  } else if (animation == "planeBounce"){
    picture(plane0, Time);
    picture(plane1, Time);
    picture(plane2, Time);
    picture(plane3, Time);
    picture(plane2, Time);
    picture(plane1, Time);
  } else {
    Serial.println("invalid animation name");
  }
}

//function holds a picture given by an array of four numbers, each for one layer, for the given amount of milliseconds (4ms for one run, plus shifting time)
void picture(uint16_t *pointer, uint16_t milliseconds) {
  for(int i = 0; i < milliseconds; i+=4) {
  shwifty(*pointer); //shift binary code for first layer
  digitalWrite(ebene0, HIGH); //turn on first layer
  delay(1); //wait a millisecond
  digitalWrite(ebene0, LOW); //turn off first layer
  
  shwifty(*(pointer+1)); //shift second array entry
  digitalWrite(ebene1, HIGH); //turn second layer on
  delay(1);
  digitalWrite(ebene1, LOW);
  
  shwifty(*(pointer+2));
  digitalWrite(ebene2, HIGH);
  delay(1);
  digitalWrite(ebene2, LOW);

  shwifty(*(pointer+3));
  digitalWrite(ebene3, HIGH);
  delay(1);
  digitalWrite(ebene3, LOW);

  }
}

//function shifts out two bytes. and takes care of the two other controlpins of the shiftregister
void shwifty (uint16_t input) {

uint8_t i;

digitalWrite(stclk,HIGH); //register mode inverted

delayMicroseconds(1);

for (i = 0; i < 16; i++)  {
        //if (bitOrder == LSBFIRST)
            //digitalWrite(datain, !(input & (1 << i)));
        //else    
            digitalWrite(datain, !(input & (1 << (15 - i))));

        digitalWrite(shclk, LOW);
        delayMicroseconds(1);
        digitalWrite(shclk, HIGH);
        delayMicroseconds(1);        
    }
    
digitalWrite(stclk,LOW); //negative edge for enabling outputs
//after this function a period of time should be waited before shifting another pattern
}
