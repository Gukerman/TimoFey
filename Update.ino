void webUpdate() {

//bin = "http://www.wk-ufa.ru/TimoFey.ino.bin"
/*
  // отключаем перезагрузку после обновления FS
    ESPhttpUpdate.rebootOnUpdate(false);
  //Обнавляем FS
  t_httpUpdate_return ret = ESPhttpUpdate.updateSpiffs("http://backup.privet.lv/esp/sonoff/spiffs.0xBB000_flash_size_1Mb.256Kb_2017.10.11.bin");
  // включаем перезагрузку после прошивки
  digitalWrite(LED_PIN, HIGH);
  ESPhttpUpdate.rebootOnUpdate(true);
  // Перепрошиваем модуль
*/
  t_httpUpdate_return ret1 = ESPhttpUpdate.update(bin);
}
