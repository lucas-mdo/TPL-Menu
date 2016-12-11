// You can have up to 10 menu items in the mainMenuItems[] array below without having to change the base programming at all. Name them however you'd like. Beyond 10 items, you will have to add additional "cases" in the switch/case
// section of the operateMainMenu() function below. You will also have to add additional void functions (i.e. menuItem11, menuItem12, etc.) to the program.
String mainMenuItems[] = { "Voltage", "Pulses", "Duration", "Delay", "Confirm" };

// Navigation button variables
int readKey;

// DEBUG LEDS ----------------------------
int led0pin = 43;
int led1pin = 45;
int led2pin = 47;
int led3pin = 49;
int led4pin = 51;
int led5pin = 53;

typedef struct {
	char title[11]; //10 char + 1 end-string
	char unit[4]; //3 char + 1 end-string
	int storedValue;
	int minValue;
	int maxValue;
	int incValue;
} subMenu;

// Menu control variables
typedef struct {
	int menuPage;
	int maxMenuPages;
	int cursorPosition;
} displayMenu;

int getMaxMenuPages(displayMenu &d) { return round(((sizeof(mainMenuItems) / sizeof(String)) / 2) + .5); }

int getTitleLength(subMenu &s) { return strlen(s.title); }

displayMenu mainMenu = { 0, 0, 0 };

subMenu voltageMenu = { "Voltage: ", " V", 0, 0, 50, 10 };
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

	//DEBUG LEDS -----
	pinMode(led0pin, OUTPUT);
	pinMode(led1pin, OUTPUT);
	pinMode(led2pin, OUTPUT);
	pinMode(led3pin, OUTPUT);
	pinMode(led4pin, OUTPUT);
	pinMode(led5pin, OUTPUT);

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
				menuItem2();
				break;
			case 2:
				menuItem3();
				break;
			case 3:
				menuItem4();
				break;
			case 4:
				menuItem5();
				break;
			case 5:
				menuItem6();
				break;
			case 6:
				menuItem7();
				break;
			case 7:
				menuItem8();
				break;
			case 8:
				menuItem9();
				break;
			case 9:
				menuItem10();
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
	lcd.clear();
	lcd.setCursor(0, 1);
	drawInstructions();
	lcd.setCursor(0, 0);
	lcd.print(s.title);
	lcd.print(s.storedValue);
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
		case 2:
			button = 0;
			s.storedValue = s.storedValue + s.incValue;
			s.storedValue = constrain(s.storedValue, s.minValue, s.maxValue);
			lcd.setCursor(getTitleLength(s), 0);
			lcd.print("     ");
			lcd.setCursor(getTitleLength(s), 0);
			lcd.print(s.storedValue);
			lcd.print(s.unit);
			break;
		case 3:
			button = 0;
			s.storedValue = s.storedValue - s.incValue;
			s.storedValue = constrain(s.storedValue, s.minValue, s.maxValue);
			lcd.setCursor(getTitleLength(s), 0);
			lcd.print("     ");
			lcd.setCursor(getTitleLength(s), 0);
			lcd.print(s.storedValue);
			lcd.print(s.unit);
			break;
		case 4:  // This case will execute if the "back" button is pressed
			button = 0;
			lcd.clear();
			lcd.setCursor(2, 0);
			lcd.print("-- VALUES --");
			lcd.setCursor(2, 1);
			lcd.print("-- STORED --");
			delay(1500);
			activeButton = 1;
			break;
		}
	}
}

void menuItem2() { // Function executes when you select the 2nd item from main menu
	int activeButton = 0;

	lcd.clear();
	lcd.setCursor(3, 0);
	lcd.print("Sub Menu 2");

	while (activeButton == 0) {
		int button;
		readKey = analogRead(0);
		if (readKey < 790) {
			delay(100);
			readKey = analogRead(0);
		}
		button = evaluateButton(readKey);
		switch (button) {
		case 4:  // This case will execute if the "back" button is pressed
			button = 0;
			activeButton = 1;
			break;
		}
	}
}

void menuItem3() { // Function executes when you select the 3rd item from main menu
	int activeButton = 0;

	lcd.clear();
	lcd.setCursor(3, 0);
	lcd.print("Sub Menu 3");

	while (activeButton == 0) {
		int button;
		readKey = analogRead(0);
		if (readKey < 790) {
			delay(100);
			readKey = analogRead(0);
		}
		button = evaluateButton(readKey);
		switch (button) {
		case 4:  // This case will execute if the "back" button is pressed
			button = 0;
			activeButton = 1;
			break;
		}
	}
}

void menuItem4() { // Function executes when you select the 4th item from main menu
	int activeButton = 0;

	lcd.clear();
	lcd.setCursor(3, 0);
	lcd.print("Sub Menu 4");

	while (activeButton == 0) {
		int button;
		readKey = analogRead(0);
		if (readKey < 790) {
			delay(100);
			readKey = analogRead(0);
		}
		button = evaluateButton(readKey);
		switch (button) {
		case 4:  // This case will execute if the "back" button is pressed
			button = 0;
			activeButton = 1;
			break;
		}
	}
}

void menuItem5() { // Function executes when you select the 5th item from main menu
	Serial.print("Chosen voltage: ");
	Serial.println(voltageMenu.storedValue);

	switch (voltageMenu.storedValue) {
	case 0:
		Serial.println("Turning on led 0");
		digitalWrite(led0pin, HIGH);
		break;
	case 1:
		Serial.println("Turning on led 1");
		digitalWrite(led1pin, HIGH);
		break;
	case 2:
		Serial.println("Turning on led 2");
		digitalWrite(led2pin, HIGH);
		break;
	case 3:
		Serial.println("Turning on led 3");
		digitalWrite(led3pin, HIGH);
		break;
	case 4:
		Serial.println("Turning on led 4");
		digitalWrite(led4pin, HIGH);
		break;
	case 5:
		Serial.println("Turning on led 5");
		digitalWrite(led5pin, HIGH);
		break;
	}

}

void menuItem6() { // Function executes when you select the 6th item from main menu
	int activeButton = 0;

	lcd.clear();
	lcd.setCursor(3, 0);
	lcd.print("Sub Menu 6");

	while (activeButton == 0) {
		int button;
		readKey = analogRead(0);
		if (readKey < 790) {
			delay(100);
			readKey = analogRead(0);
		}
		button = evaluateButton(readKey);
		switch (button) {
		case 4:  // This case will execute if the "back" button is pressed
			button = 0;
			activeButton = 1;
			break;
		}
	}
}

void menuItem7() { // Function executes when you select the 7th item from main menu
	int activeButton = 0;

	lcd.clear();
	lcd.setCursor(3, 0);
	lcd.print("Sub Menu 7");

	while (activeButton == 0) {
		int button;
		readKey = analogRead(0);
		if (readKey < 790) {
			delay(100);
			readKey = analogRead(0);
		}
		button = evaluateButton(readKey);
		switch (button) {
		case 4:  // This case will execute if the "back" button is pressed
			button = 0;
			activeButton = 1;
			break;
		}
	}
}

void menuItem8() { // Function executes when you select the 8th item from main menu
	int activeButton = 0;

	lcd.clear();
	lcd.setCursor(3, 0);
	lcd.print("Sub Menu 8");

	while (activeButton == 0) {
		int button;
		readKey = analogRead(0);
		if (readKey < 790) {
			delay(100);
			readKey = analogRead(0);
		}
		button = evaluateButton(readKey);
		switch (button) {
		case 4:  // This case will execute if the "back" button is pressed
			button = 0;
			activeButton = 1;
			break;
		}
	}
}

void menuItem9() { // Function executes when you select the 9th item from main menu
	int activeButton = 0;

	lcd.clear();
	lcd.setCursor(3, 0);
	lcd.print("Sub Menu 9");

	while (activeButton == 0) {
		int button;
		readKey = analogRead(0);
		if (readKey < 790) {
			delay(100);
			readKey = analogRead(0);
		}
		button = evaluateButton(readKey);
		switch (button) {
		case 4:  // This case will execute if the "back" button is pressed
			button = 0;
			activeButton = 1;
			break;
		}
	}
}

void menuItem10() { // Function executes when you select the 10th item from main menu
	int activeButton = 0;

	lcd.clear();
	lcd.setCursor(3, 0);
	lcd.print("Sub Menu 10");

	while (activeButton == 0) {
		int button;
		readKey = analogRead(0);
		if (readKey < 790) {
			delay(100);
			readKey = analogRead(0);
		}
		button = evaluateButton(readKey);
		switch (button) {
		case 4:  // This case will execute if the "back" button is pressed
			button = 0;
			activeButton = 1;
			break;
		}
	}
}