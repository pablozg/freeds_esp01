void requestSolaxData(void)
{ 
  HTTPClient clientHttp;
      
  clientHttp.setTimeout(3000);

  clientHttp.begin("http://5.8.8.8/?optType=ReadRealTimeData");
  clientHttp.addHeader("Host", "5.8.8.8");
  clientHttp.addHeader("Content-Length", "0");
  clientHttp.addHeader("Accept", "/*/");
  clientHttp.addHeader("Content-Type", "application/x-www-form-urlencoded");
  clientHttp.addHeader("X-Requested-With", "com.solaxcloud.starter");

  if (clientHttp.POST("") == 200)
  {
    String response = clientHttp.getString();
    if (config.version == 2) { parseJsonv2local(response); }
    else { parseJsonv3local(response); }
    //Serial.printf("Json: %s\n",response.c_str());
  } else {
    Serial.printf("Request failed\n");
  }

  clientHttp.end();
}

void parseJsonv2local(String json)
{
  DeserializationError error = deserializeJson(root, json);

  if (error) {
    Serial.printf("deserializeJson() failed: %s\n", error.c_str());
  } else {
    inverter.pv1c = root["Data"][0];     // Corriente string 1
    inverter.pv2c = root["Data"][1];     // Corriente string 2
    inverter.pv1v = root["Data"][2];     // Tension string 1
    inverter.pv2v = root["Data"][3];     // Tension string 2
    inverter.gridv = root["Data"][5];    // Tension de red
    inverter.wsolar = root["Data"][6];   // Potencia solar
    inverter.temperature = root["Data"][7];   // Potencia Temperatura
    inverter.wtoday = root["Data"][8];   // Potencia solar diaria
    inverter.wgrid = root["Data"][10];   // Potencia de red (Negativo: de red - Positivo: a red)
    inverter.pw1 = root["Data"][11];     // Potencia string 1
    inverter.pw2 = root["Data"][12];     // Potencia string 2
    inverter.wtogrid = root["Data"][41]; // Potencia diaria enviada a red

    sendResponse();
  }
}

void parseJsonv3local(String json)
{
  DeserializationError error = deserializeJson(root, json);

  if (error) {
    Serial.printf("deserializeJson() failed: %s\n", error.c_str());
  } else {
    inverter.gridv = root["Data"][0];    // Tension de red
    inverter.wsolar = root["Data"][2];   // Potencia solar
    inverter.pv1v = root["Data"][3];     // Tension string 1
    inverter.pv2v = root["Data"][4];     // Tension string 2
    inverter.pv1c = root["Data"][5];     // Corriente string 1
    inverter.pv2c = root["Data"][6];     // Corriente string 2
    inverter.pw1 = root["Data"][7];     // Potencia string 1
    inverter.pw2 = root["Data"][8];     // Potencia string 2
    inverter.wtoday = root["Data"][11];   // Potencia solar diaria
    inverter.temperature = root["Data"][39];   // Temperatura
    inverter.wgrid = root["Data"][48];   // Potencia de red (Negativo: de red - Positivo: a red)
    //inverter.wtogrid = root["Data"][41]; // Potencia diaria enviada a red

    sendResponse();
  }
}

void sendResponse(void)
{
  String response = "{\"Data\":[" + String(inverter.pv1c) + "," + String(inverter.pv2c) + "," + String(inverter.pv1v) + "," 
                      + String(inverter.pv2v) + "," + String(inverter.pw1) + "," + String(inverter.pw2) + "," + String(inverter.gridv) + ","
                      + String(inverter.wsolar) + "," + String(inverter.temperature) + "," + String(inverter.wtoday) + "," + String(inverter.wgrid) + ","
                      + String(inverter.wtogrid) + "]}";
  Serial.println(response);
}
