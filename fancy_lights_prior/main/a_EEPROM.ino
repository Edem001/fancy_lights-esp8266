void EEPROM_clear(){
  for(int i = 0; i < EEPROM.length(); i++)
    EEPROM[i] = 255;
}
void EEPROM_print_all(){
  for(int i = 0; i< EEPROM.length(); i++){
    byte value = EEPROM.read(i);
    
    Serial.print(i);
    Serial.print("\t");
    Serial.print(value);
    Serial.println();
  }
}
