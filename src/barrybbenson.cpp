#include "WPILib.h"
#include "RobotUtils/RobotUtils.h"
#include "Gear.h"

class barrybbenson: public HotBot {
private:

	HotJoystick* m_driver;
	HotJoystick* m_operator;

	Gear m_gear;

	double speed;

	bool previousAButton;
	bool previousXButton;

public:
	barrybbenson() {
		m_driver = new HotJoystick(0);
		m_operator = new HotJoystick(1);

	}
	void RobotInit() {
	}

	void DisabledPeriodic() {
		SmartDashboard::PutNumber("Gear Commanded", m_gear.GetGearCommandedSpeed());
		SmartDashboard::PutNumber("Gear Roller Commanded Speed", m_gear.GetGearRollerCommandedSpeed());
		SmartDashboard::PutNumber("Gear Position", m_gear.GetGearArmPosition());
		SmartDashboard::PutNumber("Gear Setpoint", m_gear.GetGearSetpoint());
		SmartDashboard::PutNumber("Gear Get", m_gear.GearGet());
	}

	void AutonomousInit() {
		m_gear.SetGearMode(false);
		m_gear.ZeroGearArmPosition();
	}

	void AutonomousPeriodic() {
	}

	void TeleopInit() {
	}

	void TeleopPeriodic() {
		TeleopGear();
	}

	void TeleopGear() {
		if (fabs(m_driver->AxisLY()) > 0.2) {
			m_gear.SetGearMode(false);
			m_gear.SetGearArmPosition(m_driver->AxisLY());
		}
		else if (m_driver->ButtonA()) {
			m_gear.SetGearMode(true);
			m_gear.SetGearArmPosition(20);
		}
		else if (m_driver->ButtonB()) {
			m_gear.ZeroGearArmPosition();
		}
		else {
			//m_gear.SetGearMode(false);
			m_gear.SetGearArmPosition(0.0);
		}

		if (fabs(m_driver->AxisRY()) > 0.2){
			m_gear.SetGearRollerSpeed(m_driver->AxisRY());
		}
		else {
			m_gear.SetGearRollerSpeed(0.0);
		}

		SmartDashboard::PutNumber("Gear Commanded", m_gear.GetGearCommandedSpeed());
		SmartDashboard::PutNumber("Gear Position", m_gear.GetGearArmPosition());
		SmartDashboard::PutNumber("Gear Setpoint", m_gear.GetGearSetpoint());
		SmartDashboard::PutBoolean("Gear Mode", m_gear.GetGearMode());

		SmartDashboard::PutNumber("Gear P", TALON_GEAR_P);
		SmartDashboard::PutNumber("Gear I", TALON_GEAR_I);
		SmartDashboard::PutNumber("Gear D", TALON_GEAR_D);

		SmartDashboard::PutNumber("Gear Current", m_gear.GetGearTalonCurrent());

		SmartDashboard::PutNumber("Gear Roller Commanded Speed", m_gear.GetGearRollerCommandedSpeed());

	}

	void TestPeriodic() {
	}
};

START_ROBOT_CLASS(barrybbenson)
