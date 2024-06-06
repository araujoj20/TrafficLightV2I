#include "ssd1306_i2c.h"

void main() {

	ssd1306_begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
	ssd1306_clearDisplay();

	delay(1000);

	char arrayChar[MAX_DISP_LINES][MAX_DISP_CHARS];
    char texto[MAX_DISP_CHARS*MAX_DISP_LINES]; 
    char* ptrTexto = texto;

	texto[0] = '\0';

    strcpy(arrayChar[0], "Frase 1.");
    strcpy(arrayChar[1], "Frase 2.");
    strcpy(arrayChar[2], "Frase 3.");
    strcpy(arrayChar[3], "Frase 4.");
    strcpy(arrayChar[4], "Frase 5.");
    strcpy(arrayChar[5], "Frase 6.");
    strcpy(arrayChar[6], "Frase 7.");
	strcpy(arrayChar[7], "Frase 8.");

    for (int i = 0; i < MAX_DISP_LINES; i++) {
        strcat(ptrTexto, arrayChar[i]);
        strcat(ptrTexto, "\n");
    }
    printf("%s", ptrTexto);

	ssd1306_drawString(ptrTexto);
	ssd1306_display();
}
