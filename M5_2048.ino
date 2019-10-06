#include <M5StickC.h>

uint16_t dF[16] = {	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
uint16_t field[16];
uint8_t lS = 0;
bool ch, ca, eN = false;
uint16_t mN = 2;
float aX, aY, aZ = 0;

void dH(int y) {
	M5.Lcd.drawLine(1, y, 109, y, TFT_DARKGREY);
}
void dV(int x) {
	M5.Lcd.drawLine(x, 1, x, 77, TFT_DARKGREY);
}
void dR() {
    int x[] = {1, 20, 39, 58, 77};
	M5.Lcd.fillScreen(TFT_BLACK);
	for (int i = 0; i < 5; i++) {
		dH(x[i]);
		dV(x[i] + 8*i);
	}
}
void dT(int n, int x, int y) {
	if (n > 0)
		M5.Lcd.drawString(String(n), x, y, 1);
}
void dTi() {
	for (int i = 0; i < 16; i++) {
		dT(field[i], 15 + (i%4)*27, 11 + (i/4)*19);
	}
    M5.Lcd.drawString("M5-2048", 141, 5, 1);
    M5.Lcd.drawString("Max:", 141, 25, 1);
    M5.Lcd.drawString(String(mN), 141, 35, 1);
    if (eN) {
        M5.Lcd.drawString("KONEC", 141, 70, 1);
    }
}
uint16_t rN() {
    uint16_t i = 0;
    while(!bitRead(mN, i)) { i++; }
	return 1 << random(1, i);
}
bool rA() {
	uint8_t lr[4] = {12, 13, 14, 15};
	uint8_t lr2[4] = {0, 4, 8, 12};
	uint8_t lr3[4] = {0, 1, 2, 3};
	uint8_t lr4[4] = {3, 7, 11, 15};
	if (lS == 1) {
		memcpy(lr, lr2, sizeof lr);
	} else if (lS == 2) {
		memcpy(lr, lr3, sizeof lr);
	} else if (lS == 3) {
		memcpy(lr, lr4, sizeof lr);
	}
	for (byte i = 0; i < 5; i++) {
		byte r = random(0, 4);
		uint8_t x = lr[r];
		lr[r] = lr[0];
		lr[0] = x;
	}
	for (byte i = 0; i < 4; i++) {
		if (field[lr[i]] == 0) {
			field[lr[i]] = rN();
			return true;
		}
	}

	return false;
}
void cN(int m) {
	byte f = 0;
	byte r = 4;
	byte c = 1;
	if (m == 1) { f = 3; r = -1; c = 4; }
	if (m == 2) { f = 12; r = -4; c = 1; }
	if (m == 3) { f = 0; r = 1; c = 4; }
	for (byte i = 0; i < 4; i++) {
		byte column = f + c*i;
        for (byte k = 4; k > 1; k--) {
            for (byte j = 1; j < k; j++) {
                byte row = column + r * j;
                byte row1 = column + r * (j - 1);
                if (field[row1] == 0) {
                    field[row1] = field[row];
                    field[row] = 0;
                } else if (field[row1] == field[row]) {
                    field[row1] = (field[row1] << 1) | 1;
                    field[row] = 0;
                }
            }
        }
	}
    for (byte i = 0; i < 16; i++) {
        if (field[i] > 1) {
            field[i] = (field[i] >> 1) << 1;
            if (field[i] > mN) mN = field[i];
        }
    }
}
void r() {
    memcpy(field, dF, sizeof dF);
    mN = 2;
    ch = false;
    ca = false;
    eN = false;
    dR();
	dTi();
}

void setup() {
	M5.begin();
	M5.Lcd.setRotation(3);
	M5.Axp.ScreenBreath(8);
    M5.MPU6886.Init();
	randomSeed(analogRead(0));
	
	M5.Lcd.setTextDatum(MC_DATUM);
	M5.Lcd.setTextSize(1);
    r();
}
void loop() {
    if(digitalRead(M5_BUTTON_HOME) == LOW){
        while(digitalRead(M5_BUTTON_HOME) == LOW);
        r();
    }
    M5.MPU6886.getAccelData(&aX,&aY,&aZ);
    if (aX > .6 || aY < -.6 || aX < -.6 || aY > .6 ) {
        if (ca) {
            ch = true;
            if (aX > .5) { lS = 0; }
            else if (aY < -.5) { lS = 1; }
            else if (aX < -.5) { lS = 2; }
            else if (aY > .5) { lS = 3; }
        }
        ca = false;
    } else if (aX < .4 && aX > -.4 && aY < .4 && aY > -.4 ) {
        ca = true;
    }
	if (ch) {
        dR();
		cN(lS);
		if (!rA()) {
			eN = true;
		}
		dTi();
		ch = false;
	}
}