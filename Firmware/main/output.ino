

CRGB* activeLEDs( void ){
  // // should be used while in control of the output_lock mutex
  // CRGB* leds = (buf) ? ledbuf1 : ledbuf2;
  // assert(leds);
  // return leds;

  return ledbuf1;
}

void setLEDs(size_t start, CRGB* buf, size_t len){

  CRGB* current = activeLEDs();
  for(size_t idx = 0; idx < len; idx++){
    *(current + idx) = *(buf + idx);
  }
  
}

void setLED(size_t idx, CRGB val){
  setLEDs(idx, &val, 1);
}


void output_task(void* args){
  while(1){
    vTaskDelay((MS_PER_S/OUTPUT_RATE_HZ)/portTICK_PERIOD_MS);

    // take output lock
    xSemaphoreTake(output_lock, portMAX_DELAY);

    // copy current buffer to leds and output
    CRGB* current = activeLEDs();
    if(!current){
      ESP_LOGE(OUTPUT_TAG, "no output buffer");
      continue;
    }
    for (int idx = 0; idx < NUM_LEDS; idx++){
      output[idx] = *(current + idx);
    }

    // give output lock
    xSemaphoreGive(output_lock);

    // change active buffer
    buf = !buf;

    // push the output data out
    FastLED.show();
  }
}
