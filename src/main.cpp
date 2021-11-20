#include "main.h"

bool running = false;
int phase = 0;
float torqueSummation = 0;
float currentSummation = 0;
float tempSummation = 0;
float count = 0;
int startTime = 0;

okapi::Motor testedMotor(1, false, okapi::AbstractMotor::gearset::green, okapi::AbstractMotor::encoderUnits::rotations);

bool rightButtonPressed = false;
bool leftButtonPressed = false;
bool centerButtonPressed = false;

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(0, "THE GREAT MOTOR TESTER");
	pros::lcd::set_text(1, "----------------------");
	pros::lcd::set_text(2, "Press the left button to start");
	pros::lcd::print(7, "START");
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	while (true) {
		if(phase == 0){
			startTime = pros::millis();
			phase++;
			leftButtonPressed = false;
		}
		if(phase == 1){
			torqueSummation += testedMotor.getTorque(); // nm
			currentSummation += testedMotor.getCurrentDraw(); // mA
			tempSummation += testedMotor.getTemperature(); // C
			count++;
			pros::lcd::print(0, "Warming up (%i)...", (60000 - pros::millis() + startTime) / 1000 + 1);
			pros::lcd::print(1, "Average torque(J):%f\n", torqueSummation / count);
			pros::lcd::print(2, "Average current(mA):%f\n", currentSummation / count);
			pros::lcd::print(3, "Average temperature(C):%f\n", tempSummation / count);
			pros::lcd::print(4, "Total Revolutions:%f\n", testedMotor.getPosition());
			pros::lcd::clear_line(5);
			pros::lcd::clear_line(7);
			testedMotor.moveVelocity(270);
			if(pros::millis() - startTime > 10000) phase = 2; // SHOULD BE 60000 TODO
		}
		if(phase == 2){
			startTime = pros::millis();
			testedMotor.tarePosition();
			phase++;
			torqueSummation = 0;
			currentSummation = 0;
			tempSummation = 0;
			count = 0;
		}
		if(phase == 3){
			pros::lcd::print(0, "Collecting data (%i)...", (10000 - pros::millis() + startTime) / 1000 + 1);
			pros::lcd::clear_line(1);
			pros::lcd::clear_line(2);
			pros::lcd::clear_line(3);
			pros::lcd::clear_line(4);
			torqueSummation += testedMotor.getTorque(); // nm
			currentSummation += testedMotor.getCurrentDraw(); // mA
			tempSummation += testedMotor.getTemperature(); // C
			count++;
			if(pros::millis() - startTime > 10000) phase = 4;
		}
		if(phase == 4){
			testedMotor.moveVelocity(0);
			pros::lcd::print(0, "Results:");
			pros::lcd::print(1, "Average torque(J):%f\n", torqueSummation / count);
			pros::lcd::print(2, "Average current(mA):%f\n", currentSummation / count);
			pros::lcd::print(3, "Average temperature(C):%f\n", tempSummation / count);
			pros::lcd::print(4, "Total Revolutions:%f\n", testedMotor.getPosition());
			pros::lcd::print(5, "J/A = %f", (torqueSummation * 1000 / currentSummation));
			pros::lcd::print(7, "RESTART");
			while(pros::lcd::read_buttons() != 4){
				pros::delay(5);
			}
			phase = 0;
		}
		pros::delay(5);
	}
}
