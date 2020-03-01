
#include "common.h"
#include "race_functions.h"
#include "serialCamera.h"
#include "../ESP8266/ESP8266.h"
#include "../mbed/features/netsocket/NetworkInterface.h"
#include "../mbed-http/source/http_request.h"
#include "../mbed-http/http_parser/http_parser.h"



void printPosNegEdges();
void printEdgesNrAndStatus();
void connectToWifi();

void connectToWifi()
{

  NetworkInterface* network; /* obtain a NetworkInterface object */

  const char body[] = "{\"hello\":\"world\"}";

  

  delete request; // also clears out the response

  ESP8266 esp(PTE0, PTE1, 115200);
  char* ssid = "UPC1371925";
  char* pass = "HHXPEEBU";
  char* serverIP = "192.168.0.19";
  // Global variables
  char snd[255], rcv[10000]; // Strings for sending and receiving commands / data / replies
  char* url = "/?data=123";

  char str[512];

  //PC.printf("START\r\n");

  //PC.printf("Reset ESP\r\n");
  //esp.Reset();
  //esp.RcvReply(rcv, 10000);
  //PC.printf("%s", rcv);
  //esp.SendCMD("AT+CIPSERVER=0");

  //esp.setTransparent();
  //esp.RcvReply(rcv, 10000);

  //PC.printf("Closing TCP connections...\r\n");
  //esp.SendCMD("AT+CIPCLOSE");
  //esp.RcvReply(rcv, 10000);

  //PC.printf("Sending AT\r\n");
  //strcpy(snd, "AT");
  //esp.SendCMD(snd);
  //esp.RcvReply(rcv, 10000);
  //PC.printf("%s", rcv);

  //PC.printf("Setting single conn...\r\n");
  //esp.SetSingle();
  //esp.RcvReply(rcv, 10000);
  //PC.printf("%s", rcv);

  //PC.printf("Set mode to client\r\n");
  //esp.SetMode(3);
  //esp.RcvReply(rcv, 10000);
  //PC.printf("%s", rcv);

  //PC.printf("Connecting to AP\r\n");
  //esp.Join(ssid, pass); // Replace MyAP and MyPasswd with your SSID and password
  //esp.RcvReply(rcv, 10000);
  //PC.printf("%s", rcv);

  PC.printf("Getting IP\r\n");
  esp.GetIP(rcv);
  esp.RcvReply(rcv, 10000);
  PC.printf("%s", rcv);

  HttpRequest* request = new HttpRequest(network, HTTP_POST, "http://httpbin.org/post");
  request->set_header("Content-Type", "application/json");
  HttpResponse* response = request->send(body, strlen(body));
  // if response is NULL, check response->get_error()

  printf("status is %d - %s\n", response->get_status_code(), response->get_status_message());
  printf("body is:\n%s\n", response->get_body_as_string().c_str());

  //PC.printf("Getting Connection Status\r\n");
  //esp.GetConnStatus(rcv);
  //esp.RcvReply(rcv, 10000);
  //PC.printf("%s", rcv);

  //PC.printf("Setting multiple connections...\r\n");
  //esp.SetMultiple();
  //esp.RcvReply(rcv, 10000);
  //PC.printf("%s", rcv);

  //PC.printf("Starting TCP connection...\r\n");
  //esp.startTCPConn("time.jsontest.com", 80);
  //esp.RcvReply(rcv, 10000);
  //PC.printf("%s", rcv);

  //PC.printf("setting uart mode... \r\n");
  //esp.setUARTMode();
  //esp.RcvReply(rcv, 10000);
  //PC.printf("%s", rcv);

  //char* getReq = "GET /?data=123 HTTP/1.0\nHost: localhost:8080\n\r\n";
  //


  //PC.printf("Sending data...\r\n");
  //sprintf(snd, "AT+CIPSEND=%d", strlen(getReq));
  //esp.SendCMD;

  //esp.RcvReply(rcv, 20000);

  //wait(0.5);
  //PC.printf("Sending request...");
  //esp.SendCMD("AT+CIPSEND=60\r\nGET / HTTP/1.1\r\nHost: time.jsontest.com\r\n\r\n");
  //esp.RcvReply(rcv, 20000);
  //PC.printf("%s", rcv);



  PC.printf("FINSIHED");


  //PC.printf("Sending data...\r\n");
  //esp.sendData(rcv);
  //esp.RcvReply(rcv, 10000);

  //wait(3);

  //esp.RcvReply(rcv, 10000);


  //sendCommand("AT+CIPMUX=1", 5, "OK");
  //sendCommand("AT+CIPSTART=0,\"TCP\",\"" + HOST + "\"," + PORT, 16, "OK");
  //sendCommand("AT+CIPSEND=0," + String(getData.length() + 4), 4, ">");
}


void MainRace()
{
  // main timed race
  currentRaceType = RaceType::eMainRace;

  acquireSamplesAndIntensity(); // gets samples and computes avg. light intensity
  derivScanAndFindEdges(&GrabLineScanImage0[0], &DerivLineScanImage0[0]); // computes the derivative and finds edges
  //findEdges_v2(&DerivLineScanImage0[0]);
  decideLineFromEdges();
  decideSteerAndSpeed();
  feedbackLights();
  printEdgesNrAndStatus();
  printPosNegEdges();


  PC.printf("\nCurrent steer setting: %3.10f\n", CurrentSteerSetting);
  PC.printf("kSteerRight: %.3f", kSteerRight);
  PC.printf("kSteerLeft: %.3f", kSteerLeft);

}

void SpeedLimit()
{
  currentRaceType = RaceType::eSpeedLimit;

  // search for a pattern to lower speed
  acquireSamplesAndIntensity();
  derivScanAndFindEdges(&GrabLineScanImage0[0], &DerivLineScanImage0[0]); // computes the derivative and finds edges
  decideLineFromEdges();
  decideSteerAndSpeed();

  feedbackLights();



}

void ObstacleRace()
{
  currentRaceType = RaceType::eObstacleAvoidance;

  // search for a cube from camera. what will be the output of the camera?
}

void EightRace()
{
  currentRaceType = RaceType::eEightRace;

  // normal MainRace() program, maybe we can improve the steering a bit.
}



void chooseAndRunRace()
{

  switch (TFC_GetDIP_Switch())
  {
    case 0x08:
        serialCamera();
        forceFeedbackLights(8);
        break;
        
      

    case 0x09:
      SpeedLimit();
      forceFeedbackLights(9);
      break;

    case 0x0A:
      ObstacleRace();
      forceFeedbackLights(10);
      break;

    case 0x0B:
      EightRace();
      forceFeedbackLights(11);
      break;
      
    case 0x0F:
      MainRace();
      break;

    default:
      printf("Faied to select a program...");
      break;
  }
}


int main()
{
  PC.baud(115200);
   
  TFC_Init();
  connectToWifi();

  dev = true;
  //while (1) {
  //  chooseAndRunRace();
  //  
  //}
}

void printPosNegEdges()
{
  PC.printf("\nNEG EDGES: ");
  for (int i = 0; i < nrDifferentNegEdges; i++)
  {
    PC.printf("%d ", NegEdges[i]);
  }
  PC.printf("\nPOS EDGES: ");
  for (int i = 0; i < nrDifferentPosEdges; i++)
  {
    PC.printf("%d ", PosEdges[i]);
  }
  PC.printf("\n");

}

void printEdgesNrAndStatus() {
  PC.printf("POS:%d NEG:%d STATUS:%s", nrDifferentNegEdges, nrDifferentPosEdges, stringFromTrackStatus(currentTrackStatus));
}

bool serial_find(Serial* serial, char* str_to_find, uint32_t timeout_s)
{
  // Use RTC (instead of interrupt) for timeout detection
  time_t curr = time(NULL);

  bool is_partial_match = false;
  uint16_t match_ix = 0;
  size_t str_to_find_len = strlen(str_to_find);

  while (1) {
    // timeout detection
    if (time(NULL) - curr > timeout_s) return false;

    while (serial->readable()) {
      // also do timeout detection here...
      if (time(NULL) - curr > timeout_s) return false;

      char c = serial->getc();

      if (!is_partial_match) {
        // Find the first character in the stream
        if (c == str_to_find[0]) {
          if (str_to_find_len == 1) return true;

          match_ix = 1;
          is_partial_match = true;
        }
        continue;
      }

      // is_partial_match is correct, check remaining characters
      if (c == str_to_find[match_ix]) {
        if (str_to_find_len == match_ix + 1) return true;

        match_ix++;
      }
      else {
        // not a match anymore, continue
        is_partial_match = false;
      }
    }
  }

  return false;
}