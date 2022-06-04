#include "it8951.h"
#include "img.h"
#include "text.h"

// Screen size: 1200 x 825
IT8951 epd(CS, RESET, HRDY);

uint16_t overlay_color = (uint16_t) 0xdcdcdc;

bool print(const char * text, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const char * font = "DejaVuSans12", enum Text::ALIGNMENT align = Text::JUSTIFY, bool invert = false){
  const int bpp = 4; // 4 Bits per Pixel

  // memory align
  x = x - x % (16/bpp);
  width = width - width % (16/bpp);


  // reserve Buffer  (beware of out-of-memory!)
  size_t len = width * height / bpp; // 4 Bits per Pixel

  void * buf=malloc(len * 2);  // char size!

  // Set to black (0x00) or white (0xff)
  memset(buf, invert ? 0x00 : 0xff, len * 2);

  // Create Text Object
  Text out((uint16_t*)buf, width, height, bpp);

  // Select font
  if (!out.setFont(font))
    Serial.println("Font not found, using default");

  // Optional configuration
  out.setAlginment(align);

  // Generate Text in buffer
  out.generate(text, invert);

  // Load buffer to epaper
  if (!epd.load((uint16_t*)buf, len, x, y, width, height)){
    Serial.println("Loading buffer failed");
    free(buf);
    return false;
  }
  free(buf);

  // Partial displaying 
  if (!epd.display(x, y, width, height)){
    Serial.println("Partial display failed");

    return false;
  }

  return true;
}

uint16_t testA[1000] = { 0 };
/*uint16_t testB[1000] = { 0 };
uint16_t testC[1000] = { 0 };
uint16_t testD[1000] = { 0 };*/

void setup() {
  Serial.begin(115200);
  Serial.println("Boot");
  Serial.println("Init");

  epd.begin(4000000);
  Serial.print("Device Info:\n Panel ");
  Serial.print(epd.width());
  Serial.print(" x ");
  Serial.println(epd.height());
  Serial.print(" Address 0x");
  Serial.println(epd.defaultImageBuffer(), HEX);
  Serial.print(" Firmware: ");
  Serial.println(epd.getFW());
  Serial.print(" LUT: ");
  Serial.println(epd.getLUT());

  for (int i = 0; i < 1000; i++){
    testA[i] = 0x0000;
    // testB[i] = 0x8888;
    // testC[i] = 0x5555;
    // testD[i] = 0x2222;
  }

  Serial.print("Activating screen...");
  if (epd.active())
    Serial.println("active");

  Serial.print("Clearing screen...");
  if (epd.clear(true))
    Serial.println("cleared");

  Serial.print("Full refresh... ");
  Serial.println(epd.display(0, 0, epd.width(), epd.height()));

  Serial.print("Drawing top bar... ");
  epd.fill(overlay_color, 0, 0,      1200, 50); // 1200x50
  epd.waitForDisplay();
  Serial.println("ok");
  Serial.print("Drawing bottom bar... ");
  epd.fill(overlay_color, 0, 825 - 50, 1200, 50);
  epd.waitForDisplay();
  Serial.println("ok");
  
  Serial.print("Full refresh... ");
  Serial.println(epd.display(0, 0, epd.width(), epd.height()) ? "okay" : "failed");
  epd.waitForDisplay();

  Serial.println("Loading arrow... ");
  epd.load(epd_bitmap_sharp_play_arrow_black_48dp, 48*48, 0, 0, 48, 48);
  // epd.load(testA, 1000, 0, 100, 30, 30);
  epd.waitForDisplay();
  
  Serial.print("Displaying arrow... ");
  epd.display(0, 100, 30, 30);
  epd.waitForDisplay();

  print("Welcome!", 100, 75, 300, 52, "Inter_Medium52");
  epd.waitForDisplay();

  Serial.print("Overlaying top bar... ");
  epd.fill((uint16_t) 0xffffff, 0, 0, 600, 50); // 1200x50
  epd.waitForDisplay();
  Serial.println("ok");
  Serial.print("Displaying... ");
  Serial.println(epd.display(0, 0, 600, 50) ? "okay" : "failed");
  epd.waitForDisplay();
}

int x = 0;
int y = 0;
int n = 0;
int c = 0;
void loop() {
  /*Serial.print("Loop ");
  Serial.println(n);
  // if (!epd.active()){
  //   Serial.print("Reset ");
  //   epd.reset();
  // }
  x += 50;
  if (x > 1150){
    x = 0;
    y+= 80;
  }
  if (y > 745){
    y = 0;
  }
  epd.waitForDisplay();  
  Serial.print("Load ");
  switch(c++) {
    case 0:
    Serial.println(epd.load(testA, 1000, x, y, 50, 80));
    break;
    case 1:
    Serial.println(epd.load(testB, 1000, x, y, 50, 80));
    break;
    case 2:
    Serial.println(epd.load(testC, 1000, x, y, 50, 80));
    break;
    case 3:
    Serial.println(epd.load(testD, 1000, x, y, 50, 80));
    break;
  }
  if(c > 3)
    c = 0;
  epd.waitForDisplay();  
  // if (n++ % 5 == 0){
  //   Serial.print("Full refresh ");
  //   Serial.println(epd.display(0, 0, 2000, 2000));
  // } else {
    Serial.print("Partial refresh ");
    Serial.println(epd.display(x,y, 50, 80));
  // }
  epd.waitForDisplay();
  // Serial.print("Sleep ");
  // Serial.println(epd.sleep());
  // Serial.println();
  /* esp_sleep_enable_timer_wakeup(1000000);
  if (esp_light_sleep_start() != ESP_OK){
     delay(1000);
  }*/
  delay(10000);
}
