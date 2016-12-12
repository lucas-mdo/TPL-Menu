// You can have up to 10 menu items in the mainMenuItems[] array below without having to change the base programming at all. Name them however you'd like. Beyond 10 items, you will have to add additional "cases" in the switch/case
// section of the operateMainMenu() function below. You will also have to add additional void functions (i.e. menuItem11, menuItem12, etc.) to the program.
String mainMenuItems[] = { "Voltage", "Pulses", "Duration", "Delay", "Confirm" };

// Navigation button variables
int readKey;

// Menu control variables
typedef struct {
	int menuPage;
	int maxMenuPages;
	int cursorPosition;
} displayMenu;

typedef struct {
	char title[11]; //10 char + 1 end-string
	char unit[4]; //3 char + 1 end-string
	int storedValue;
	int minValue;
	int maxValue;
	int incValue;
	int pins[6];
} subMenu;

int getMaxMenuPages(displayMenu &d) { return round(((sizeof(mainMenuItems) / sizeof(String)) / 2) + .5); }

int getTitleLength(subMenu &s) { return strlen(s.title); }

int getChosenOption(subMenu &s) { return s.storedValue / s.incValue; }

displayMenu mainMenu = { 0, 0, 0 };

subMenu voltageMenu = { "Voltage: ", " V", 0, 0, 50, 10,{ 43, 45, 47, 49, 51, 53 } };
subMenu pulseMenu = { "Pulses: ", " s", 0, 0, 5, 1 };
subMenu durationMenu = { "Duration: ", " ms", 0, 0, 10, 2 };
subMenu delayMenu = { "Delay: ", " ms", 0, 0, 10, 2 };

// Creates 3 custom characters for the menu display
byte downArrow[8] = {
	0b00100, //   *
	0b00100, //   *
	0b00100, //   *
	0b00100, //   *
	0b00100, //   *
	0b10101, // * * *
	0b01110, //  ***
	0b00100  //   *
};

byte upArrow[8] = {
	0b00100, //   *
	0b01110, //  ***
	0b10101, // * * *
	0b00100, //   *
	0b00100, //   *
	0b00100, //   *
	0b00100, //   *
	0b00100  //   *
};

byte menuCursor[8] = {
	B01000, //  *
	B00100, //   *
	B00010, //    *
	B00001, //     *
	B00010, //    *
	B00100, //   *
	B01000, //  *
	B00000  //
};

#include <Wire.h>
#include <LiquidCrystal.h>

// Setting the LCD shields pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup() {

	// Initializes serial communication
	Serial.begin(9600);

	for (int i = 0; i < sizeof(voltageMenu.pins) / sizeof(voltageMenu.pins[0]); i++) {
		pinMode(voltageMenu.pins[i], OUTPUT);
	}

	// Initializes && clears the LCD screen
	lcd.begin(16, 2);
	lcd.clear();

	// Creates the byte for the 3 custom characters
	lcd.createChar(0, menuCursor);
	lcd.createChar(1, upArrow);
	lcd.createChar(2, downArrow);

	mainMenu.maxMenuPages = getMaxMenuPages(mainMenu);
}

void loop() {
	menuDraw(mainMenu.menuPage, mainMenu.maxMenuPages, mainMenuItems);
	drawCursor(mainMenu.menuPage, mainMenu.cursorPosition);
	operateMainMenu();
}

// This function will generate the 2 menu items that can fit on the screen. They will change as you scroll through your menu. Up && down arrows will indicate your current menu position.
void menuDraw(int page, int maxPages, String itemsArray[]) {
	Serial.print(page);
	lcd.clear();
	lcd.setCursor(1, 0);
	lcd.print(itemsArray[page]);
	lcd.setCursor(1, 1);
	lcd.print(itemsArray[page + 1]);
	if (page == 0) {
		lcd.setCursor(15, 1);
		lcd.write(byte(2));
	}
	else if (page > 0 && page < maxPages) {
		lcd.setCursor(15, 1);
		lcd.write(byte(2));
		lcd.setCursor(15, 0);
		lcd.write(byte(1));
	}
	else if (page == maxPages) {
		lcd.setCursor(15, 0);
		lcd.write(byte(1));
	}
}

// When called, this function will erase the current cursor && redraw it based on the cursorPosition && menuPage variables.
void drawCursor(int page, int pos) {
	for (int x = 0; x < 2; x++) {     // Erases current cursor
		lcd.setCursor(0, x);
		lcd.print(" ");
	}

	// The menu is set up to be progressive (menuPage 0 = Item 1 & Item 2, menuPage 1 = Item 2 & Item 3, menuPage 2 = Item 3 & Item 4), so
	// in order to determine where the cursor should be you need to see if you are at an odd or even menu page && an odd or even cursor position.
	if (page % 2 == 0) {
		if (pos % 2 == 0) {  // If the menu page is even && the cursor position is even that means the cursor should be on line 1
			lcd.setCursor(0, 0);
			lcd.write(byte(0));
		}
		if (pos % 2 != 0) {  // If the menu page is even && the cursor position is odd that means the cursor should be on line 2
			lcd.setCursor(0, 1);
			lcd.write(byte(0));
		}
	}
	if (page % 2 != 0) {
		if (pos % 2 == 0) {  // If the menu page is odd && the cursor position is even that means the cursor should be on line 2
			lcd.setCursor(0, 1);
			lcd.write(byte(0));
		}
		if (pos % 2 != 0) {  // If the menu page is odd && the cursor position is odd that means the cursor should be on line 1
			lcd.setCursor(0, 0);
			lcd.write(byte(0));
		}
	}
}

void operateMainMenu() {
	int activeButton = 0;
	while (activeButton == 0) {
		int button;
		readKey = analogRead(0);
		if (readKey < 790) {
			delay(100);
			readKey = analogRead(0);
		}
		button = evaluateButton(readKey);
		switch (button) {
		case 0: // When button returns as 0 there is no action taken
			break;
		case 1:  // This case will execute if the "forward" button is pressed
			button = 0;
			switch (mainMenu.cursorPosition) { // The case that is selected here is dependent on which menu page you are on && where the cursor is.
			case 0:
				showSubMenu(voltageMenu);
				break;
			case 1:
				showSubMenu(pulseMenu);
				break;
			case 2:
				showSubMenu(durationMenu);
				break;
			case 3:
				showSubMenu(delayMenu);
				break;
			case 4:
				confirmMenu();
				break;
			}
			activeButton = 1;
			menuDraw(mainMenu.menuPage, mainMenu.maxMenuPages, mainMenuItems);
			drawCursor(mainMenu.menuPage, mainMenu.cursorPosition);
			break;
		case 2:
			button = 0;
			moveUp(mainMenu, mainMenuItems);
			activeButton = 1;
			break;
		case 3:
			button = 0;
			moveDown(mainMenu, mainMenuItems);
			activeButton = 1;
			break;
		}
	}
}

// This function is called whenever a button press is evaluated. The LCD shield works by observing a voltage drop across the buttons all hooked up to A0.
int evaluateButton(int x) {
	int result = 0;
	if (x < 50) {
		result = 1; // right
	}
	else if (x < 195) {
		result = 2; // up
	}
	else if (x < 380) {
		result = 3; // down
	}
	else if (x < 790) {
		result = 4; // left
	}
	return result;
}

// If there are common usage instructions on more than 1 of your menu items you can call this function from the sub
// menus to make things a little more simplified. If you don't have common instructions or verbage on multiple menus
// I would just delete this void. You must also delete the drawInstructions()function calls from your sub menu functions.
void drawInstructions() {
	lcd.setCursor(0, 1); // Set cursor to the bottom line
	lcd.print("Use ");
	lcd.write(byte(1)); // Up arrow
	lcd.print("/");
	lcd.write(byte(2)); // Down arrow
	lcd.print(" buttons");
}

//The following couple of functions handles the navigations through menu items vertically (up && down)
void moveUp(displayMenu &d, String itemsArray[]) {
	if (d.menuPage == 0) {
		d.cursorPosition = d.cursorPosition - 1;
		d.cursorPosition = constrain(d.cursorPosition, 0, ((sizeof(itemsArray) / sizeof(String)) - 1));
	}
	if (d.menuPage % 2 == 0 && d.cursorPosition % 2 == 0) {
		d.menuPage = d.menuPage - 1;
		d.menuPage = constrain(d.menuPage, 0, d.maxMenuPages);
	}

	if (d.menuPage % 2 != 0 && d.cursorPosition % 2 != 0) {
		d.menuPage = d.menuPage - 1;
		d.menuPage = constrain(d.menuPage, 0, d.maxMenuPages);
	}

	d.cursorPosition = d.cursorPosition - 1;
	d.cursorPosition = constrain(d.cursorPosition, 0, ((sizeof(itemsArray) / sizeof(String)) - 1));

	menuDraw(d.menuPage, d.maxMenuPages, itemsArray);
	drawCursor(d.menuPage, d.cursorPosition);
}

void moveDown(displayMenu &d, String itemsArray[]) {
	if (d.menuPage % 2 == 0 && d.cursorPosition % 2 != 0) {
		d.menuPage = d.menuPage + 1;
		d.menuPage = constrain(d.menuPage, 0, d.maxMenuPages);
	}

	if (d.menuPage % 2 != 0 && d.cursorPosition % 2 == 0) {
		d.menuPage = d.menuPage + 1;
		d.menuPage = constrain(d.menuPage, 0, d.maxMenuPages);
	}

	d.cursorPosition = d.cursorPosition + 1;
	d.cursorPosition = constrain(d.cursorPosition, 0, ((sizeof(itemsArray) / sizeof(String)) - 1));

	menuDraw(d.menuPage, d.maxMenuPages, itemsArray);
	drawCursor(d.menuPage, d.cursorPosition);
}

void showSubMenu(subMenu &s) { // Function executes when you select the 1st item from main menu
	int activeButton = 0;
	int tempValue = s.storedValue;
	lcd.clear();
	lcd.setCursor(0, 1);
	drawInstructions();
	lcd.setCursor(0, 0);
	lcd.print(s.title);
	lcd.print(tempValue);
	lcd.print(s.unit);
	while (activeButton == 0) {
		int button;
		readKey = analogRead(0);
		if (readKey < 790) {
			delay(100);
			readKey = analogRead(0);
		}
		button = evaluateButton(readKey);
		switch (button) {
		case 1: //Forward: store current value
			button = 0;
			s.storedValue = tempValue;
			lcd.clear();
			lcd.setCursor(2, 0);
			lcd.print("-- VALUES --");
			lcd.setCursor(2, 1);
			lcd.print("-- STORED --");
			delay(1500);
			activeButton = 1;
			break;
		case 2: //Up: plus increment
			button = 0;
			tempValue = tempValue + s.incValue;
			tempValue = constrain(tempValue, s.minValue, s.maxValue);
			lcd.setCursor(getTitleLength(s), 0);
			lcd.print("     ");
			lcd.setCursor(getTitleLength(s), 0);
			lcd.print(tempValue);
			lcd.print(s.unit);
			break;
		case 3: // Down: minus increment
			button = 0;
			tempValue = tempValue - s.incValue;
			tempValue = constrain(tempValue, s.minValue, s.maxValue);
			lcd.setCursor(getTitleLength(s), 0);
			lcd.print("     ");
			lcd.setCursor(getTitleLength(s), 0);
			lcd.print(tempValue);
			lcd.print(s.unit);
			break;
		case 4:  // Back: cancel action
			button = 0;
			activeButton = 1;
			break;
		}
	}
}

void runConfiguration() {
	Serial.println("Running...");
	digitalWrite(voltageMenu.pins[getChosenOption(voltageMenu)], HIGH);
	delay(durationMenu.storedValue * 1000);
	digitalWrite(voltageMenu.pins[getChosenOption(voltageMenu)], LOW);
	delay(delayMenu.storedValue * 1000);
}

void stopConfiguration() {
	digitalWrite(voltageMenu.pins[getChosenOption(voltageMenu)], LOW);
	Serial.println("Stopped.");
}

void confirmMenu() { // Function executes when you select the 5th item from main menu
	Serial.println();
	Serial.println("======================");
	Serial.print("Chosen voltage: ");
	Serial.println(voltageMenu.storedValue);
	Serial.print("Chosen pulse: ");
	Serial.println(pulseMenu.storedValue);
	Serial.print("Chosen duration: ");
	Serial.println(durationMenu.storedValue);
	Serial.print("Chosen delay: ");
	Serial.println(delayMenu.storedValue);
	Serial.println("======================");

	lcd.clear();
	lcd.setCursor(3, 0);
	lcd.print("RUNNING");
	lcd.setCursor(0, 1);
	lcd.print("Use RST to stop");

	//FIRULA 1
	int init = 10;
	int col = init;

	for (int i = 0; i < pulseMenu.storedValue; i++) {
		runConfiguration();

		//FIRULA 2
		if (col < init + 3) {
			lcd.setCursor(col, 0);
			lcd.print('.');
			col++;
		}
		else {
			col = init;
			lcd.setCursor(col, 0);
			lcd.print("   ");
		}
	}
}
