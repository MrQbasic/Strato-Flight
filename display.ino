void render_FlightData(){
	st7735.st7735_write_str(0, 0, "---FLIGHT---", Font_7x10);
    
    //check if flight is started
    if(time_start == -1){
        st7735.st7735_write_str(0, 24, "WAITING FOR READY", Font_7x10);
        return;
    }

    int hour = time_flying / 3600;
    int min  = (time_flying % 3600) / 60; 
    int sec  = (time_flying % 3600 % 60);

    String time_str  = "TIME: " + (String) hour + ":" + (String) min + ":" + (String) sec + "  ";
	st7735.st7735_write_str(0, 12, time_str, Font_7x10);
}


void render_GPS_Data(){
    String time_str  = "TIME: " + (String)GPS.time.hour() + ":" + (String)GPS.time.minute() + ":" + (String)GPS.time.second()+ ":"+(String)GPS.time.centisecond() + "  ";
	String latitude  = "LAT:  " + (String)GPS.location.lat() + "  ";
	String longitude = "LON:  " +  (String)GPS.location.lng() + "  ";
    String altitude  = "ALT:  " + String(GPS.altitude.meters()) + "  ";
    String speed     = "SPD:  " + String(GPS.speed.kmph()) + " km/h  ";
	st7735.st7735_write_str(0,  0, "---GPS---", Font_7x10);
	st7735.st7735_write_str(0, 12, time_str, Font_7x10);
    st7735.st7735_write_str(0, 24, latitude, Font_7x10);
    st7735.st7735_write_str(0, 36, longitude, Font_7x10);
    st7735.st7735_write_str(0, 48, altitude, Font_7x10);
    st7735.st7735_write_str(0, 60, speed, Font_7x10);
    st7735.st7735_write_str(0, 72, "ACC:  " + (String) (GPS.hdop.hdop() * 3.0) + "m   ", Font_7x10);
}

void render_ErrorData(){
  int errorCnt = 0;

  st7735.st7735_write_str(0,  0, "---ERROR---", Font_7x10);
	if(error_GPS_drop){
    errorCnt++;
    st7735.st7735_write_str(0, errorCnt*12, "GPS Signal drop", Font_7x10, ST7735_RED);
  }
}


void render_ErrorCode(const char* string){
  st7735.st7735_fill_screen(ST7735_RED);
  st7735.st7735_write_str(0, 0, "ERROR:", Font_11x18, ST7735_WHITE, ST7735_RED);
  st7735.st7735_write_str(0, 20, string, Font_11x18, ST7735_WHITE, ST7735_RED);
}