void webUpdateSpiffs() {
  String refresh = "<html><head><meta http-equiv=\"refresh\" content=\"1;http://";
  refresh += WiFi.localIP().toString();
  refresh += "\"></head></html>";
  HTTP.send(200, "text/html", refresh);
  //t_httpUpdate_return ret = ESPhttpUpdate.updateSpiffs("http://backup.privet.lv/rgb_spiffs_1m_256k.bin");
  saveConfig();
}

// Перезагрузка модуля
void handle_restart() {
  String restart = HTTP.arg("device");
  if (restart == "ok") {                         // Если значение равно Ок
    HTTP.send(200, "text/plain", "Reset OK"); // Oтправляем ответ Reset OK
    ESP.restart();                                // перезагружаем модуль
  } else {                                        // иначе
    HTTP.send(200, "text/plain", "No Reset"); // Oтправляем ответ No Reset
  }
}

// Установка языка
void handle_leng() {
  Language = HTTP.arg("set");
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}
// Сохраняет все настройки в файле
void handle_save_config() {
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}



void HTTP_init(void) {
  // Добавляем функцию Update для перезаписи прошивки по Wi-Fi при 1М(256K SPIFFS) и выше
  httpUpdater.setup(&HTTP);
  // Кешировать файлы для быстрой работы
  HTTP.serveStatic("/css/", SPIFFS, "/css/", "max-age=31536000"); // кеширование на 1 год
  HTTP.serveStatic("/js/", SPIFFS, "/js/", "max-age=31536000"); // кеширование на 1 год
  HTTP.serveStatic("/img/", SPIFFS, "/img/", "max-age=31536000"); // кеширование на 1 год
  //HTTP.serveStatic("/lang/", SPIFFS, "/lang/", "max-age=31536000"); // кеширование на 1 год
  // HTTP.on("/webupdatespiffs", webUpdateSpiffs);                // запустить загрузку fs для обнавления
  HTTP.on("/save", handle_save_config);      // Сохранить настройки в файл
  HTTP.on("/config.live.json", handle_ConfigJSON); // формирование config_xml страницы для передачи данных в web интерфейс
  HTTP.on("/devices.list.json", handle_ip_list);  // формирование iplocation_xml страницы для передачи данных в web интерфейс
  HTTP.on("/restart", handle_restart);               // Перезагрузка модуля
  HTTP.on("/lang", handle_leng);               // Установить язык
  HTTP.on("/lang.list.json", handle_leng_list);               // Установить язык
  HTTP.on("/modules.json", handle_modules);               // Узнать какие модули есть в устройстве
  // Запускаем HTTP сервер
  HTTP.begin();
}



void handle_ConfigJSON() {
  String root = "{}";  // Формировать строку для отправки в браузер json формат
  //{"SSDP":"SSDP-test","ssid":"home","ssidPass":"i12345678","ssidAP":"WiFi","ssidApPass":"","ip":"192.168.0.101"}
  // Резервируем память для json обекта буфер может рости по мере необходимти, предпочтительно для ESP8266
  DynamicJsonBuffer jsonBuffer;
  //  вызовите парсер JSON через экземпляр jsonBuffer
  JsonObject& json = jsonBuffer.parseObject(root);
  // Заполняем поля json
  // Заполняем поля json
  json["ddns"] = ddns;  // Имя ddns
  json["ddnsName"] = ddnsName;  // Имя ddnsName
  json["ddnsPort"] = ddnsPort;  // Имя ddnsPort
  String temp = "";
  temp += dht.getTemperature();
  json["Temperature"] = temp;
  temp = "";
  temp += dht.getHumidity();
  json["Humidity"] = temp;
  json["SSDP"] = ssdpName;
  json["ssidAP"] = ssidApName;
  json["ssidApPass"] = ssidApPass;
  json["ssid"] = ssidName;
  json["ssidPass"] = ssidPass;
  json["timeZone"] = timezone;
  json["timeSonoff"] = timeSonoff; //  Время работы

  json["ip"] = WiFi.localIP().toString();
  json["time"] = GetTime(); // Текущее время
  json["lang"] = Language;  // Язык
  json["pirTime"] = pirTime;  //
  json["state"] = state0;
  // Помещаем созданный json в переменную root
  root = "";
  json.printTo(root);
  HTTP.send(200, "text/json", root);
}

void handle_ip_list() {
  HTTP.send(200, "text/json", "[" + modules() + Devices + "]");
}

void handle_modules() {
  HTTP.send(200, "text/json", modules());
}

String modules() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["ip"] = WiFi.localIP().toString();
  json["SSDP"] = ssdpName;
  JsonArray& data = json.createNestedArray("module");
  for (int i = 0; i < sizeof(module) / sizeof(module[0]); i++) {
    data.add(module[i]);
  }
  String root;
  json.printTo(root);
  return root;
}

void handle_leng_list() {
  HTTP.send(200, "text/json", Lang);
}
