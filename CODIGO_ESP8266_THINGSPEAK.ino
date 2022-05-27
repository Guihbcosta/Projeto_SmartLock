#include <PubSubClient.h>
#include <ESP8266WiFi.h>

char ssid[] = "#######";                   // Change to your network SSID (name).
char pass[] = "########";          // Change to your network password.
const char* server = "mqtt3.thingspeak.com";
char mqttUserName[] = "#######w0xHjk7LSQ";  // Change to your MQTT device username.
char mqttPass[] = "#########lZzgO";      // Change to your MQTT device password.
char clientID[] = "##########jk7LSQ";     // Change to your MQTT device clientID.
long writeChannelID = #####75;

WiFiClient client;                                 // Initialize the Wi-Fi client library. Uncomment for nonsecure connection.
PubSubClient mqttClient( client );                    // Initialize the PuBSubClient library.

int fieldsToPublish[8] = {1, 0, 0, 0, 0, 0, 0, 0};    // Change to allow multiple fields.
float dataToPublish[8];                               // Holds your field data.

// Generate a unique client ID and connect to MQTT broker.
void mqttConnect()
{
  // Loop until connected.
  while ( !mqttClient.connected() )
  {
    //Serial.println(String( mqttUserName) + " , " + mqttPass + " , " + clientID);

    // Connect to the MQTT broker.
    //Serial.print( "Attempting MQTT connection..." );
    if ( mqttClient.connect( clientID, mqttUserName, mqttPass ) )
    {
      //Serial.println( "Connected with Client ID:  " + String( clientID ) + " User " + String( mqttUserName ) + " Pwd " + String( mqttPass ) );

    } else
    {
      //Serial.print( "failed, rc = " );
      // See https://pubsubclient.knolleary.net/api.html#state for the failure code explanation.
      //Serial.print( mqttClient.state() );
      //Serial.println( " Will try again in 5 seconds" );
      delay( 5000 );
    }
  }
}

// Subscribe to a field or feed from a ThingSpeak channel.
int mqttSubscribe( long subChannelID, int field, int unsubSub ) {
  String myTopic;

  // There is no field zero, so if field 0 is sent to subscribe to, then subscribe to the whole channel feed.
  if (field == 0) {
    myTopic = "channels/" + String( subChannelID ) + "/subscribe";
  }
  else {
    myTopic = "channels/" + String( subChannelID ) + "/subscribe/fields/field" + String( field );
  }

  //Serial.println( "Subscribing to " + myTopic );
  //Serial.println( "State= " + String( mqttClient.state() ) );

  if ( unsubSub == 1 ) {
    return mqttClient.unsubscribe(myTopic.c_str());
  }
  return mqttClient.subscribe( myTopic.c_str() , 0 );
}

//Publicar em um canal
void mqttPublish(long pubChannelID, float dataArray[], int fieldArray[]) {
  int index = 0;
  String dataString = "";

  while (index < 8) {

    // Look at the field array to build the posting string to send to ThingSpeak.
    if (fieldArray[ index ] > 0) {

      dataString += "&field" + String( index + 1 ) + "=" + String( dataArray [ index ] );
    }
    index++;
  }

  //Serial.println( dataString );

  // Create a topic string and publish data to ThingSpeak channel feed.
  String topicString = "channels/" + String( pubChannelID ) + "/publish";
  mqttClient.publish( topicString.c_str(), dataString.c_str() );
  //Serial.println( "Published to channel " + String( pubChannelID ) );
}

// Publish messages to a channel feed.

// Connect to a given Wi-Fi SSID.
void connectWifi() {
  //WiFi.mode(WIFI_STA); //ativa o modo de estacao
  //Serial.print("Connecting to ");
  WiFi.begin(ssid, pass);
  //Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }

  //Serial.println();
  //Serial.print("Connected, IP address: ");
  //Serial.println(WiFi.localIP());
  Serial.write("ESP ok");
}

void setup() {
  Serial.begin(9600);
  //Serial.println( "Start" );
  int status = WL_IDLE_STATUS; // Set temporary Wi-Fi status.

  connectWifi();                                        // Connect to Wi-Fi network.
  mqttClient.setServer( server, 1883 );              // Set the MQTT broker details, nonsecure port. Uncomment for nonsecure connection.
  //mqttClient.setServer( server, 8883 );                 // Set the MQTT broker details, secure port. Uncomment for secure connection.
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    connectWifi();
  }

  if (!mqttClient.connected())
  {

    mqttConnect(); // Connect if MQTT client is not connected.

    if (mqttSubscribe( writeChannelID, 1, 0 ) == 1 ) {
      //Serial.println( " Subscribed " );
    }
  }

  mqttClient.loop(); // Call the loop to maintain connection to the server.

  String IncomingString = "";

  while (Serial.available()) {
    IncomingString = Serial.readString();
    int num = IncomingString.toInt();
    Serial.println(num);
    if (num == 1) {
      dataToPublish[0] = 1; 
      delay(1100);                       // Wait for ThingSpeak to publish.
      mqttPublish( writeChannelID, dataToPublish, fieldsToPublish );
      //Serial.println("aberto");
    }
    if (num == 2) {
      dataToPublish[0] = 0;
      delay(1100);                       // Wait for ThingSpeak to publish.
      mqttPublish( writeChannelID, dataToPublish, fieldsToPublish );
      //Serial.println("fechado");
    }
}
