#include "src/Painless_Mesh/painlessMesh.h"
#include "src/ArduinoJson/ArduinoJson.h"
#include "src/easyWebServer/easyWebServer.h"
#include "src/easyWebSocket/easyWebSocket.h"
#include "animations.h"

#define   MESH_PREFIX     "IoTmeshToy"
#define   MESH_PASSWORD   "estudiohacker"
#define   MESH_PORT       5555

// globals
painlessMesh  mesh;
extern NeoPixelAnimator animations; // NeoPixel animation management object
extern AnimationController controllers[]; // array of add-on controllers for my animations
os_timer_t  yerpTimer;

void setup() {
  Serial.begin( 74880 );

  // Let's limit the analogWrite range to 255, so RGB values will make more sense.
  analogWriteRange(255);

  pinMode(ledPinR, OUTPUT);
  pinMode(ledPinG, OUTPUT);
  pinMode(ledPinB, OUTPUT);
  digitalWrite(ledPinR, HIGH);
  digitalWrite(ledPinG, HIGH);
  digitalWrite(ledPinB, HIGH);
  
  // setup mesh
//  mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE | APPLICATION ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP | APPLICATION );  // set before init() so that you can see startup messages
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT);
  mesh.onReceive( &receivedCallback );
  mesh.onNewConnection( &newConnectionCallback );

  // setups webServer
  webServerInit();

  // setup webSocket
  webSocketInit();
  webSocketSetReceiveCallback( &wsReceiveCallback );
  webSocketSetConnectionCallback( &wsConnectionCallback );

  mesh.debugMsg( STARTUP, "\nIn setup() my NodeId=%d\n", mesh.getNodeId());

  animationsInit();

  os_timer_setfn( &yerpTimer, yerpCb, NULL );
  os_timer_arm( &yerpTimer, 1000, 1 );
}

void loop() {
  mesh.update();

  static uint16_t previousConnections;
  uint16_t numConnections = mesh.getNodeList().size();
  if( countWsConnections() > 0 )
    numConnections++;

  if ( previousConnections != numConnections ) {
    mesh.debugMsg( GENERAL, "loop(): numConnections=%d\n", numConnections);

    if ( numConnections == 0 ) {
      controllers[smoothIdx].nextAnimation = searchingIdx;
      controllers[searchingIdx].nextAnimation = searchingIdx;
      controllers[searchingIdx].hue[0] = 0.0f;
    } else {
      controllers[searchingIdx].nextAnimation = smoothIdx;
      controllers[smoothIdx].nextAnimation = smoothIdx;
    }

    sendWsControl();

    previousConnections = numConnections;
  }

  animations.UpdateAnimations();
}

void yerpCb( void *arg ) {
  static int yerpCount;
  int connCount = 0;

  String msg = "Yerp=";
  msg += yerpCount++;

  mesh.debugMsg( APPLICATION, "msg-->%s<-- stationStatus=%u numConnections=%u nodeTime=%u\n",
    msg.c_str(), wifi_station_get_connect_status(), mesh.getNodeList().size(), mesh.getNodeTime() );

  // send ping to webSockets
  String ping("ping");
  broadcastWsMessage(ping.c_str(), ping.length(), OPCODE_TEXT);
  sendWsControl();
}

void newConnectionCallback( uint32_t nodeId ) {
  mesh.debugMsg( APPLICATION, "newConnectionCallback():\n");
  
  String control = buildControl();
  mesh.sendBroadcast( control );
}

void receivedCallback( uint32_t from, String &msg ) {
  mesh.debugMsg( APPLICATION, "receivedCallback():\n");

  DynamicJsonBuffer jsonBuffer(50);
  JsonObject& control = jsonBuffer.parseObject( msg );

  broadcastWsMessage(msg.c_str(), msg.length(), OPCODE_TEXT);

  mesh.debugMsg( APPLICATION, "control=%s\n", msg.c_str());

  for ( int i = 0; i < ( mesh.getNodeList().size() + 1 ); i++) {
    float hue = control[String(i)];
    controllers[smoothIdx].hue[i] = hue;
  }
}

void wsConnectionCallback( void ) {
  mesh.debugMsg( APPLICATION, "wsConnectionCallback():\n");
}

void wsReceiveCallback( char *payloadData ) {
  mesh.debugMsg( APPLICATION, "wsReceiveCallback(): payloadData=%s\n", payloadData );

  String msg( payloadData );
  mesh.sendBroadcast( msg );

  if ( strcmp( payloadData, "wsOpened") == 0) {  // hack to give the browser time to get the ws up and running
    mesh.debugMsg( APPLICATION, "wsReceiveCallback(): received wsOpened\n" );
    sendWsControl();
    return;
  }

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& control = jsonBuffer.parseObject(payloadData);

  if (!control.success()) {   // Test if parsing succeeded.
    mesh.debugMsg( APPLICATION, "wsReceiveCallback(): parseObject() failed. payload=%s<--\n", payloadData);
    return;
  }
  
  uint16_t blips = mesh.getNodeList().size() + 1;
  if ( blips > MAX_BLIPS )
    blips = MAX_BLIPS;
    
  for ( int i = 0; i < blips; i++) {
    String temp(i);
    float hue = control[temp];
    controllers[smoothIdx].hue[i] = hue;
  }
}

void sendWsControl( void ) {
  mesh.debugMsg( APPLICATION, "sendWsControl():\n");
  
  String control = buildControl();
  broadcastWsMessage(control.c_str(), control.length(), OPCODE_TEXT);
}

String buildControl ( void ) {
  uint16_t blips = mesh.getNodeList().size() + 1;
  mesh.debugMsg( APPLICATION, "buildControl(): blips=%d\n", blips);

  if ( blips > MAX_BLIPS ) {
    mesh.debugMsg( APPLICATION, " blips out of range =%d\n", blips);
    blips = MAX_BLIPS;
  }

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& control = jsonBuffer.createObject();  
  for (int i = 0; i < blips; i++ ) {
    control[String(i)] = String(controllers[smoothIdx].hue[i]);
  }

  String ret;
  control.printTo(ret);
  return ret;
}
