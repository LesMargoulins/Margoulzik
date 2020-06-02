#include "MargoulError.h"

bool MargoulError::_state = false;


MargoulError::MargoulError() {

}

MargoulError::MargoulError(bool initState) {
	_state = initState;
}

void MargoulError::throwError() {
	/*
	LiquidCrystal lcd(10, 5, 6, 7, 8, 9);
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("An error occured");*/
	_state = true;
}

bool MargoulError::getState() {
	return _state;
}

void MargoulError::setState(bool state) {
	this->_state = state;
}