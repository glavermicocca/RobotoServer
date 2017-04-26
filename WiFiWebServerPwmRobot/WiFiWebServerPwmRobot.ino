/*
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a GPIO pin depending on the request
      http://server_ip/gpio/0 will set the GPIO2 low,
      http://server_ip/gpio/1 will set the GPIO2 high
    server_ip is the IP address of the ESP8266 module, will be
    printed to Serial when the module is connected.
*/

#include <ESP8266WiFi.h>

const char* ssid = "Mittelab_Members";
const char* password = "Mittelabsuperpassword";

//const char* ssid = "Thomas";
//const char* password = "viapiccardi47thomasdelpup01";

#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define D7 13

const int button = D2;
int buttonState = 0;

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);

  // prepare GPIO2
  pinMode(0, OUTPUT);
  //digitalWrite(0, 0);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());

  ESP.wdtDisable();
  ESP.wdtEnable(WDTO_8S);

  pinMode(D2, INPUT);
}

int globale = 0;
int differenza = 50;

void loop() {

  int value = digitalRead(D2);    // Read state of PushButton
  if (value == HIGH) {
     if(differenza != 0) //sono fermo
     {
       Serial.println("HIGHT");
       differenza++;
       Serial.println(differenza);
     }
  } else {
    if(differenza != 0) //sono fermo
    {
       Serial.println("LOW");
       differenza--;
       Serial.println(differenza);
    }
  }
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  ESP.wdtFeed();

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val;
  if (req.indexOf("/start") != -1)
  {
    client.flush();
    // Prepare the response
    String s;
    s += "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><head>";
    s += "<script src='https://code.jquery.com/jquery-3.1.1.slim.min.js' integrity='sha256-/SIrNqv8h6QGKDuNoLGA4iret+kyesCkHGzVUUV0shc='crossorigin='anonymous'></script>";
    s += "<style>.floating-box {display: inline-block;}</style>";
    s += "</head>";
    s += "<body>";
    s += "<form class='floating-box ' action='/gpio/0'>";
    s += "<input type='submit' value='+'>";
    s += "</form>";
    s += "<form class='floating-box ' action='/gpio/0'>";
    s += "<input type='submit' value='-'>";
    s += globale;
    s += "</form>";
    s += "</body></html>\n";

    s = "";
    s += "<html> <head> <meta charset='utf-8'> <meta name='viewport' content='width=device-width'> <title>JS Bin</title> <style> .floating-box { display: inline-block; } </style> <script src='https://code.jquery.com/jquery-3.0.0.js'></script> <script type='text/javascript'> $(document).ready(function() { $('#plus').click(function() { $.get('/gpio/0', function(data) { console.log(data); }); }); $('#minus').click(function() { $.get('/gpio/1', function(data) { console.log(data); }); }); $(document).keydown(function(e) { switch(e.which) { case 37: $.get('/gpio/left', function(data) { console.log(data); }); break; case 38: var k = $('#k').val(); $.get('/gpio/up/' + k, function(data) { console.log(data); }); break; case 39: $.get('/gpio/right', function(data) { console.log(data); }); break; case 40: $.get('/gpio/down', function(data) { console.log(data); }); break; case 32: $.get('/gpio/space', function(data) { console.log(data); }); break; default: return; } e.preventDefault(); }); }); </script> </head> <body> <div> <div class='floating-box'> <button id='plus' type='button'>+ </button> </div> <div class='floating-box'> <button id='minus' type='button'>- </button> </div> <div class='floating-box'> <input id='k' type='text' value ='70'/> </div> </div> </body> </html>";

    // Send the response to the client
    client.print(s);
    client.stop();
    delay(1);
    differenza = 0;
    return;
  }
  else if (req.indexOf("/gpio/up") != -1) {
    String value = req.substring(req.lastIndexOf("/up/") + 4);
    value = value.substring(0, value.lastIndexOf("/") - 5);

    int k = value.toInt();

    if (400 + k + globale > 950) globale == 0;
    else globale += 50;

    analogWrite(D8, 400 + globale);
    analogWrite(D5, 400 + k + globale);
    analogWrite(D6, 5);
    analogWrite(D7, 5);
    client.flush();
    // Send the response to the client
    client.print("up");
    differenza = 1;
  }
  else if (req.indexOf("/gpio/down") != -1) {
    globale = 0;
    analogWrite(D8, 5);
    analogWrite(D5, 5);
    analogWrite(D6, 600);
    analogWrite(D7, 670);
    delay(200);
    client.flush();
    // Send the response to the client
    client.print("down");
  }
  else if (req.indexOf("/gpio/space") != -1) {
    globale = 0;
    analogWrite(D8, 5);
    analogWrite(D5, 5);
    analogWrite(D6, 5);
    analogWrite(D7, 5);
    client.flush();
    // Send the response to the client
    client.print("space");
  }
  else if (req.indexOf("/gpio/right") != -1) {
    globale = 0;
    analogWrite(D8, 870);
    analogWrite(D5, 5);
    analogWrite(D6, 5);
    analogWrite(D7, 5);

    delay(200);
    analogWrite(D8, 5);
    client.flush();
    client.print("right");
  }
  else if (req.indexOf("/gpio/left") != -1) {
    globale = 0;
    analogWrite(D8, 0);
    analogWrite(D5, 980);
    analogWrite(D6, 5);
    analogWrite(D7, 5);
    
    delay(200);
    analogWrite(D5, 5);
    client.flush();
    client.print("left");
  }
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  Serial.println("Client disonnected");
  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}

