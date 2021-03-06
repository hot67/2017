#include "WPILib.h"
#include "CANTalon.h"
#include <iostream>
#include "RobotUtils/RobotUtils.h"
#include "CameraServer.h"
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/core/core.hpp>

#include "Drivetrain.h"
#include "CameraHandler.h"
#include "Shooter.h"
#include "Gear.h"
#include "Intake.h"
//Auto Selector -> get
//Select Auto -> put

/*
	 ***** Driver Joystick Mapping
	 *
	 * BUTTONS
	 * 		A - run shoot full blast
	 * 		B - run shoot 3400 rpm
	 * 		X - climb out
	 * 		Y -
	 *
	 * DPAD
	 * 		UP -
	 * 		DOWN -
	 * 		LEFT -
	 * 		RIGHT -
	 *
	 * JOYSTICK
	 * 		LEFT - up/down drive
	 * 		RIGHT - left/right drive
	 *
	 * BUMPERS
	 * 		LEFT - gear shifter (hold)
	 * 		RIGHT -
	 *
	 * TRIGGERS
	 * 		LEFT - paddle out
	 * 		RIGHT - paddle in
	 *
	 * OTHER
	 * 		START -
	 * 		BACK -
	 *
	 */

/*
	 ***** Operator Joystick Mapping
	 *
	 * BUTTONS
	 * 		A - gear to ground
	 * 		B - gear secondary place
	 * 		X - gear initial place
	 * 		Y - gear to package
	 *
	 * DPAD
	 * 		UP - pickup out, hopper out
	 * 		DOWN - pickup in, hopper in
	 * 		LEFT -
	 * 		RIGHT - hopper in
	 *
	 * JOYSTICK
	 * 		LEFT - manual gear arm movement
	 * 		RIGHT -
	 *
	 * BUMPERS
	 * 		LEFT - gear out roller
	 * 		RIGHT - gear in roller
	 *
	 * TRIGGERS
	 * 		LEFT - fuel out roller
	 * 		RIGHT - fuel in roller
	 *
	 * OTHER
	 * 		START - shooter up //not yet because it is currently on driver for testing until the encoder is in
	 * 		BACK -
	 *
	 */
/*
enum autonType {
	kDoNothing = 0,
	kBlueLeftGear = 1,
	kBlueCenterGear = 2,
	kBlueRightGear = 3,
	kRedLeftGear = 4,
	kRedCenterGear = 5,
	kRedRightGear = 6,
	kDriveStraight5Ft = 7,
	kAutonCase = 8
};
*/
class barrybbenson: public HotBot {
private:

	HotJoystick* m_driver;
	HotJoystick* m_operator;
	Timer m_timer;

	PowerDistributionPanel m_pdp;

	Shooter m_shoot;
	Drivetrain m_drivetrain;
	Gear m_gear;
	Intake m_intake;
	CameraHandler m_cameraHandler;

	//autonType m_autonType;

	Timer m_currentTimer;
	Timer m_rollTimer;
	Timer m_autonCaseTimeout;

	double autonType;


	double totalDriveCurrent;

	double m_autonInitialDistance;
	double m_autonSecondaryDistance;

	double m_autonPegAngle;
	double m_autonBoilerAngle;

	double m_autonBackUpAngle;
	double m_autonBackUpDistance;

	double m_autonBackUpDistanceInitial;
	double m_autonBackUpDistanceSecondary;

	double m_autonBoilerStartAngle;

	double m_autonDropOffDistance;
	double m_autonAwayFromCenterGearPeg;
	double m_autonLineAngle;
	double m_autonCrossLine;
	double m_autonCrossLineAngle;
	bool m_placeGear;

	bool m_autonShoot;

	int placeGear = 0;
	unsigned m_autonCase = 0;
	bool m_autonCenterGear = 0;
	double m_arlinaInches = 0; // Inches actually moved per assigned inch

public:

	double shooterSpeed;
	bool isFeederOn = false;

	barrybbenson() : m_pdp(0) {
		m_driver = new HotJoystick(0);
		m_operator = new HotJoystick(1);

		autonType = 0;
	}


    static void VisionThread() {
        cs::UsbCamera m_camera = CameraServer::GetInstance()->StartAutomaticCapture();
        cs::UsbCamera m_camera2 = CameraServer::GetInstance()->StartAutomaticCapture();
        std::cout << "Camera Capture Started" << std::endl;
        m_camera.SetResolution(160, 120);
        m_camera.SetExposureHoldCurrent();
        m_camera.SetBrightness(2);
        m_camera.SetFPS(15);

        m_camera2.SetResolution(160, 120);
        m_camera2.SetExposureHoldCurrent();
        m_camera2.SetBrightness(10);
        m_camera2.SetFPS(15);
        while(true) {
        	m_camera.SetExposureManual(SmartDashboard::GetNumber("m_exposure", 10));
        	m_camera.SetBrightness(SmartDashboard::GetNumber("m_brightness", 2));
        	m_camera2.SetExposureManual(SmartDashboard::GetNumber("m_exposure", 10));
        	m_camera2.SetBrightness(SmartDashboard::GetNumber("m_brightness", 2));
        }
    }

	void RobotInit() {
		SmartDashboard::PutNumber("m_exposure", 10);
        std::thread visionThread(VisionThread);
        visionThread.detach();
        //m_gear.GameStartGearArmPosition();
        m_gear.ZeroGearArmPosition();
        m_shoot.ZeroShootEncoder();
        m_drivetrain.zeroDriveEncoders();
        autonType = 0;
        m_autonShoot = false;
	}

	void DisabledPeriodic() {

		m_drivetrain.flashLightOn(false);

		if (autonType == 1) { //blue left gear
			m_autonInitialDistance = -88;//-8
			m_autonBackUpAngle = 60;
			m_autonBackUpDistance = -74.0;//-64.0; //-61.0

			m_autonBackUpDistanceInitial = -53;
			m_autonBackUpDistanceSecondary = -18;

			m_autonDropOffDistance = 10;

			m_autonLineAngle = 55.0;
			m_autonCrossLine = 65.0;
			placeGear = true;
			m_autonShoot = true;
		}
		else if (autonType == 2) { //blue center gear
			m_autonInitialDistance = -88.0;

			m_autonBackUpAngle = 0;
			m_autonBackUpDistance = 0;
			m_autonDropOffDistance = 10;
			m_autonAwayFromCenterGearPeg = 0;
			m_autonLineAngle = 70;
			m_autonCrossLine = 90;
			placeGear = true;
			m_autonCenterGear = true;

		}
		else if (autonType == 3) { //blue right gear
			m_autonInitialDistance = -84;//-103.0; // 86.0 for comp bot, 77.0 for pactice
			m_autonBackUpAngle = -60;
			m_autonBackUpDistance = -70.0;//-64.0; //-61.0

			m_autonBackUpDistanceInitial = -53;
			m_autonBackUpDistanceSecondary = -18;

			m_autonDropOffDistance = 20;

			m_autonLineAngle = 45.0;
			m_autonCrossLine = -80.0;
			placeGear = true;
			m_autonShoot = false;

		}
		else if (autonType == 4) { //red left gear

			m_autonInitialDistance = -84;//-103.0; // 86.0 for comp bot, 77.0 for pactice
			m_autonBackUpAngle = 60;
			m_autonBackUpDistance = -70.0;//-64.0; //-61.0

			m_autonBackUpDistanceInitial = -53;
			m_autonBackUpDistanceSecondary = -18;

			m_autonDropOffDistance = 20;

			m_autonLineAngle = -45.0;
			m_autonCrossLine = -80.0;
			placeGear = true;
			m_autonShoot = false;
		}
		else if (autonType == 5) { //red center gear
			m_autonInitialDistance = -88.0;

			m_autonBackUpAngle = 0;
			m_autonBackUpDistance = 0;
			m_autonDropOffDistance = 10;
			m_autonAwayFromCenterGearPeg = 0;
			m_autonLineAngle = -70;
			m_autonCrossLine = 90;
			placeGear = true;
			m_autonCenterGear = true;

		}
		else if (autonType == 6) { //red right gear
			m_autonInitialDistance = -88;//-103.0; // 86.0 for comp bot, 77.0 for pactice
			m_autonBackUpAngle = -60;
			m_autonBackUpDistance = -74.0;//-64.0; //-61.0

			m_autonBackUpDistanceInitial = -53;
			m_autonBackUpDistanceSecondary = -18;

			m_autonDropOffDistance = 10;

			m_autonLineAngle = -55.0;
			m_autonCrossLine = 65.0;
			placeGear = true;
			m_autonShoot = true;
		}
		else if (autonType == 7) { //drive straight
			m_autonInitialDistance = -150.0; //-150
			m_autonBackUpAngle = 0;
			m_autonBackUpDistance = 0;
			placeGear = false;
			m_autonShoot = false;

		}
		else if (autonType == 8) {
			m_autonInitialDistance = -69.0;
			m_autonBackUpAngle = 0;
			m_autonBackUpDistance = 0;
			placeGear = false;
			m_autonShoot = false;
		}
		else {
			m_autonInitialDistance = 0;
			m_autonBackUpAngle = 0;
			m_autonBackUpDistance = 0;
			placeGear = false;
			m_autonShoot = false;
		}




		SmartDashboard::PutNumber("Angle", m_drivetrain.getYaw());
		autonType = SmartDashboard::GetNumber("Auto Selector", 0);

		//SmartDashboard::PutNumber("* Test", SmartDashboard::GetNumber("Auto Selector", 0));
		SmartDashboard::PutNumber("Auton Type", autonType);

		SmartDashboard::PutNumber("m_exposure", 10);

	}

	void AutonomousInit() {
		m_autonCase = 0;

		//m_autonCenterGear = false;
		m_drivetrain.setShift(false);
		m_drivetrain.resetGyro();

		m_drivetrain.ResetAnglePIDController();
		m_drivetrain.ResetDrivePIDController();

		m_drivetrain.zeroDriveEncoders();
		m_gear.GameStartGearArmPosition();
		m_rollTimer.Reset();
		m_gear.resetRoller();
		m_gear.SetGearRollerSpeed(0.0);

		m_drivetrain.setDistancePIDSpeed(0.9);
		m_drivetrain.setTurnPIDSpeed(0.55);

		m_shoot.SetShootMode(true);
		m_shoot.RunShoot(0);

		m_shoot.RunPaddle(0.0);

		//m_intake.SetHopper(true);

		m_drivetrain.flashLightOn(false);

		m_autonCaseTimeout.Stop();
		m_autonCaseTimeout.Reset();
		m_autonCaseTimeout.Start();

		}

	void AutonOutput() {
		SmartDashboard::PutNumber("AUTON_CASE", m_autonCase);
		SmartDashboard::PutNumber("AUTON_ANGLE ERROR", m_drivetrain.GetAnglePIDError());
		SmartDashboard::PutNumber("AUTON_ANGLE", m_drivetrain.getYaw());
		SmartDashboard::PutNumber("AUTON_ANGLE SETPOINT", m_drivetrain.GetAnglePIDSetpoint());
		SmartDashboard::PutNumber("AUTON_XPEG", m_cameraHandler.GetPegTargetNormalizedCenter());
		SmartDashboard::PutNumber("AUTON_BOILER X", m_cameraHandler.GetBoilerTargetNormalizedCenter());
		SmartDashboard::PutNumber("AUTON_TIME", m_autonCaseTimeout.Get());
	}

	void AutonomousPeriodic() {

		/*SmartDashboard::PutNumber("Auton Case", m_autonCase);
		SmartDashboard::PutNumber("Auton Type", autonType);*/

		//m_drivetrain.setShift(true);
		//SmartDashboard::PutBoolean("Drive PID Enabled", m_drivetrain.IsPIDEnabled());

		//SmartDashboard::PutBoolean("Drive PID Enabled", m_drivetrain.IsPIDEnabled());
		//SmartDashboard::PutNumber("Drive Rotation Error", m_drivetrain.GetRotationPIDError());
		//SmartDashboard::PutNumber("Drive Distance Error", m_drivetrain.GetDistancePIDError());
		//SmartDashboard::PutNumber("Drive Angle Error", m_drivetrain.GetAnglePIDError());

		//SmartDashboard::PutNumber("Drive Angle Setpoint", m_drivetrain.GetAnglePIDSetpoint());
		//SmartDashboard::PutNumber("Drive Distance Setpoint", m_drivetrain.GetDistancePIDSetpoint());
		//SmartDashboard::PutBoolean("Drive Angle PID Is Enabled", m_drivetrain.AnglePIDIsEnabled());

		/*SmartDashboard::PutNumber("Peg X", SmartDashboard::GetNumber("xPeg", 0));
		SmartDashboard::PutNumber("Boiler X", SmartDashboard::GetNumber("xBoiler", 0));*/

		//SmartDashboard::PutNumber("Drive Left Front Talon Get", m_drivetrain.getLeftEncoder());
		//SmartDashboard::PutNumber("Drive Right Front Talon Get", m_drivetrain.getRightEncoder());
		//SmartDashboard::PutNumber("Angle", m_drivetrain.getYaw());
		//SmartDashboard::PutNumber("Tune Angle", m_cameraHandler.GetPegAngle());
		//SmartDashboard::PutNumber("Angle - Boiler", m_cameraHandler.GetBoilerAngle());


		if (m_autonCenterGear == true) {
			switch(m_autonCase) {
			case 0:
				if(autonArmFinished(GEAR_PLACE_FIRST) == true) {
					std::cout << "autonCenterGearFinished" << std::endl;
					m_autonCase++;
					AutonOutput();
				}
				break;
			case 1:
				if(autonDriveFinished(m_autonInitialDistance, 0)) {
					std::cout << "autonDriveFinished" << std::endl;
					m_drivetrain.zeroDriveEncoders();
					m_autonCase++;
					AutonOutput();
				}
				break;
			case 2:
				if (autonReleaseGear() == true) {
					m_autonCase++;
					AutonOutput();
					m_drivetrain.setDistancePIDSpeed(0.5);
					m_gear.SetGearRollerSpeed(-0.7);
					m_drivetrain.zeroDriveEncoders();
					m_timer.Reset();
					m_timer.Start();
				}
				break;
			case 3:
				if (autonPauseFinished(0.75) == true) {
					m_gear.SetGearArmPosition(0.0);
					m_autonCase++;
					AutonOutput();
					//m_drivetrain.resetGyro();
				}
				break;
			case 4:
				if (autonDriveFinished(m_autonDropOffDistance, 0) == true) {
					m_autonCase++;
					AutonOutput();
					//m_drivetrain.resetGyro();
					m_gear.SetGearRollerSpeed(0.0);
					m_drivetrain.zeroDriveEncoders();
					m_drivetrain.setDistancePIDSpeed(0.9);
					m_drivetrain.setAngleP(0.4);

					m_drivetrain.setTurnPIDSpeed(0.7);

				}
				break;
			case 5:
//shoot
				m_shoot.SetShootMode(true);
				m_shoot.RunShoot(3300);

				if (autonDriveFinished(m_autonAwayFromCenterGearPeg, m_autonBackUpAngle) == true) {
					//m_drivetrain.resetGyro();
					m_autonCase++;
					AutonOutput();
				}
				break;
			case 6:
				if (autonTurnFinished(m_autonLineAngle) == true) {
					m_autonCase++;
					AutonOutput();
					//m_drivetrain.resetGyro();
					m_drivetrain.zeroDriveEncoders();
					m_drivetrain.setTurnPIDSpeed(0.4);
				}
				break;
			case 7:
				if (autonDriveFinished(m_autonCrossLine, m_autonLineAngle) == true) {
					m_drivetrain.zeroDriveEncoders();
					m_autonCase++;
					AutonOutput();
				}
				break;
			case 8:
				if (autonArmFinished(GEAR_PACKAGE) == true) {
					m_autonCase++;
					AutonOutput();
					/*SmartDashboard::PutNumber("TUNE8_ANGLE", m_drivetrain.getYaw());
					SmartDashboard::PutNumber("TUNE8_ANGLE SETPOINT", m_drivetrain.GetAnglePIDSetpoint());*/
					m_autonBoilerStartAngle = m_cameraHandler.GetBoilerAngle() + m_drivetrain.getYaw();
				}
				break;
			case 9:
				/*SmartDashboard::PutNumber("TUNE9_ANGLE ERROR", m_drivetrain.GetAnglePIDError());
				SmartDashboard::PutNumber("TUNE9_ANGLE", m_drivetrain.getYaw());
				SmartDashboard::PutNumber("TUNE9_ANGLE SETPOINT", m_drivetrain.GetAnglePIDSetpoint());*/

				if (autonTurnFinished(m_autonBoilerStartAngle) == true) {
					m_autonCase++;
					AutonOutput();
					//m_drivetrain.resetGyro();
					m_drivetrain.zeroDriveEncoders();
					m_autonBoilerStartAngle = m_cameraHandler.GetBoilerAngle() + m_drivetrain.getYaw();

				}
				break;
			case 10:
				/*SmartDashboard::PutNumber("TUNE10_ANGLE", m_drivetrain.getYaw());
				SmartDashboard::PutNumber("TUNE10_ANGLE SETPOINT", m_drivetrain.GetAnglePIDSetpoint());
				SmartDashboard::PutNumber("TUNE10_ANGLE ERROR", m_drivetrain.GetAnglePIDError());*/

				if ((autonTurnFinished(m_autonBoilerStartAngle) == true) || (m_autonCaseTimeout.Get() > 12.0)) {
					m_autonCase++;
					AutonOutput();
					//m_drivetrain.resetGyro();
					m_drivetrain.zeroDriveEncoders();
					m_drivetrain.setAngleP(0.4);

					m_autonBoilerStartAngle = m_cameraHandler.GetBoilerAngle() + m_drivetrain.getYaw();
				}
				break;
			case 11:
				m_drivetrain.SetPIDSetpoint(m_drivetrain.GetAverageDistance(), m_autonBoilerStartAngle);

				/*SmartDashboard::PutNumber("TUNE11_ANGLE", m_drivetrain.getYaw());
				SmartDashboard::PutNumber("TUNE11_ANGLE SETPOINT", m_drivetrain.GetAnglePIDSetpoint());
				SmartDashboard::PutNumber("TUNE11_ANGLE ERROR", m_drivetrain.GetAnglePIDError());*/

				if ((fabs(m_drivetrain.GetAnglePIDError()) < 2.5) || (m_autonCaseTimeout.Get() > 12.0)) {
					m_drivetrain.zeroDriveEncoders();
					m_drivetrain.DisablePID();
					m_drivetrain.resetAnglePID();
					m_drivetrain.resetDistancePID();
					m_autonCase++;
					AutonOutput();
				} else {
					m_drivetrain.EnablePID();
				}
				break;
			case 12:
				m_shoot.RunPaddle(-1.0);
				m_autonCase++;
				AutonOutput();
			}
		} else {
			m_drivetrain.SetAngleAbsoluteTolerance(0.0);

			//SmartDashboard::PutNumber("Auton Peg Angle", m_autonPegAngle);
			//SmartDashboard::PutNumber("Angle Setpoint", m_autonPegAngle);

			switch (m_autonCase){
			case 0:
				m_drivetrain.setDistancePIDSpeed(0.9);
				//m_autonCase++;
				if (autonDriveFinished(m_autonInitialDistance, 0) == true) {
					m_autonCase++;
					AutonOutput();
					m_drivetrain.setTurnPIDSpeed(0.6);
					/*SmartDashboard::PutNumber("TUNE0_ANGLE", m_drivetrain.getYaw());
					SmartDashboard::PutNumber("TUNE0_DISTANCE", m_drivetrain.GetAverageDistance());
					SmartDashboard::PutNumber("TUNE0_DISTANCE_ERROR", m_drivetrain.GetDistancePIDError());*/
					m_drivetrain.zeroDriveEncoders();

					m_timer.Stop();
					m_timer.Reset();
					m_timer.Start();
				}
				break;
			case 1:
				if ((autonTurnFinished(m_autonBackUpAngle) == true) && (placeGear == true)) {
					/*SmartDashboard::PutNumber("TUNE1_ANGLE", m_drivetrain.getYaw());
					SmartDashboard::PutNumber("TUNE1_DISTANCE", m_drivetrain.GetAverageDistance());
					SmartDashboard::PutNumber("TUNE1_XPEG", SmartDashboard::GetNumber("xPeg", 0));
					SmartDashboard::PutNumber("TUNE1_ANGLE_ERROR", m_drivetrain.GetAnglePIDError());*/
					m_drivetrain.zeroDriveEncoders();
					m_drivetrain.DisablePID();
					//m_drivetrain.resetGyro();
					m_autonCase++;
					AutonOutput();

					m_timer.Stop();
					m_timer.Reset();
					m_timer.Start();
				}
				break;
			case 2:
				if (autonPauseFinished(0.25) == true) {
					m_autonPegAngle = (m_drivetrain.getYaw() + m_cameraHandler.GetPegAngle());

					/*SmartDashboard::PutNumber("TUNE2_ANGLE", m_drivetrain.getYaw());
					SmartDashboard::PutNumber("TUNE2_DISTANCE", m_drivetrain.GetAverageDistance());
					SmartDashboard::PutNumber("TUNE2_XPEG", SmartDashboard::GetNumber("xPeg", 0));
					SmartDashboard::PutNumber("TUNE2_XPEG_OFFSET", m_cameraHandler.GetPegTargetNormalizedCenter());
					SmartDashboard::PutNumber("TUNE2_ANGLE-ERROR", m_drivetrain.GetAnglePIDError());
					SmartDashboard::PutNumber("TUNE2_ANGLE_SETPOINT", m_drivetrain.GetAnglePIDSetpoint());*/

					m_autonCase++;
					AutonOutput();
					//m_drivetrain.resetGyro();
				}
				break;
			case 3:
				if (autonTurnFinished(m_autonPegAngle) == true) {
					/*SmartDashboard::PutNumber("TUNE3_ANGLE", m_drivetrain.getYaw());
					SmartDashboard::PutNumber("TUNE3_DISTANCE", m_drivetrain.GetAverageDistance());
					SmartDashboard::PutNumber("TUNE3_XPEG", SmartDashboard::GetNumber("xPeg", 0));
					SmartDashboard::PutNumber("TUNE3_XPEG_OFFSET", m_cameraHandler.GetPegTargetNormalizedCenter());
					SmartDashboard::PutNumber("TUNE3_ANGLE-ERROR", m_drivetrain.GetAnglePIDError());
					SmartDashboard::PutNumber("TUNE3_ANGLE_SETPOINT", m_drivetrain.GetAnglePIDSetpoint());*/

					m_autonCase++;
					AutonOutput();
					m_drivetrain.zeroDriveEncoders();
					m_autonPegAngle = (m_drivetrain.getYaw() + m_cameraHandler.GetPegAngle());

					m_timer.Stop();
					m_timer.Reset();
					m_timer.Start();
				}
				break;
			case 4:
				if (autonPauseFinished(0.3) == true) {
					m_autonPegAngle = (m_drivetrain.getYaw() + m_cameraHandler.GetPegAngle());

					/*SmartDashboard::PutNumber("TUNE4_ANGLE", m_drivetrain.getYaw());
					SmartDashboard::PutNumber("TUNE4_DISTANCE", m_drivetrain.GetAverageDistance());
					SmartDashboard::PutNumber("TUNE4_XPEG", SmartDashboard::GetNumber("xPeg", 0));
					SmartDashboard::PutNumber("TUNE4_XPEG_OFFSET", m_cameraHandler.GetPegTargetNormalizedCenter());
					SmartDashboard::PutNumber("TUNE4_ANGLE-ERROR", m_drivetrain.GetAnglePIDError());
					SmartDashboard::PutNumber("TUNE4_ANGLE_SETPOINT", m_drivetrain.GetAnglePIDSetpoint());*/

					m_autonCase++;
					AutonOutput();
					//m_drivetrain.resetGyro();
				}
				break;
			case 5:
				if (autonTurnFinished(m_autonPegAngle) == true) {
					/*SmartDashboard::PutNumber("TUNE5_ANGLE", m_drivetrain.getYaw());
					SmartDashboard::PutNumber("TUNE5_DISTANCE", m_drivetrain.GetAverageDistance());
					SmartDashboard::PutNumber("TUNE5_XPEG", SmartDashboard::GetNumber("xPeg", 0));
					SmartDashboard::PutNumber("TUNE5_XPEG_OFFSET", m_cameraHandler.GetPegTargetNormalizedCenter());
					SmartDashboard::PutNumber("TUNE5_ANGLE-ERROR", m_drivetrain.GetAnglePIDError());
					SmartDashboard::PutNumber("TUNE5_ANGLE_SETPOINT", m_drivetrain.GetAnglePIDSetpoint());*/

					m_autonCase++;
					AutonOutput();
					m_drivetrain.zeroDriveEncoders();
					m_autonPegAngle = (m_drivetrain.getYaw() + m_cameraHandler.GetPegAngle());
				}
				break;
			case 6:
				if (autonTurnFinished(m_autonPegAngle) == true) {
					m_autonCase++;
					AutonOutput();
					m_drivetrain.zeroDriveEncoders();
					//m_autonPegAngle = (m_drivetrain.getYaw() + m_cameraHandler.GetPegAngle());
				}
				break;
			case 7:
//for the higher peg, remove at competition the +2
				if (autonArmFinished(GEAR_PLACE_FIRST) == true) {
					m_autonCase++;
					AutonOutput();

					m_timer.Stop();
					m_timer.Reset();
					m_timer.Start();

					m_drivetrain.setTurnPIDSpeed(0.5);
				}
				break;
			case 8:
				m_drivetrain.SetPIDSetpoint(m_autonBackUpDistance, m_autonPegAngle);
				if ((fabs(m_drivetrain.GetDistancePIDError()) < 4) && (fabs(m_drivetrain.GetAnglePIDError()) < 1.0)) {
					//m_drivetrain.zeroDriveEncoders();
					m_drivetrain.DisablePID();
					m_drivetrain.resetAnglePID();
					m_drivetrain.resetDistancePID();
					m_autonCase++;
					AutonOutput();

					m_drivetrain.setAngleP(ANGLE_P);
					m_drivetrain.zeroDriveEncoders();

					m_timer.Stop();
					m_timer.Reset();
					m_timer.Start();
				}
				else {
					m_drivetrain.EnablePID();
				}
				break;
			case 9:
				if (autonPauseFinished(0.3) == true) {
					m_autonCase++;
					AutonOutput();
					//m_drivetrain.resetGyro();
				}
				break;
			case 10:
				if (autonReleaseGear() == true) {
					m_autonCase++;
					AutonOutput();
					//m_drivetrain.setDistancePIDSpeed(0.45);
					m_drivetrain.zeroDriveEncoders();
					m_gear.SetGearRollerSpeed(-1.0);
					m_timer.Stop();
					m_timer.Reset();
					m_timer.Start();
				}
				break;
			case 11:
				if (autonPauseFinished(0.7) == true) {
					m_autonCase++;
					AutonOutput();
					m_gear.SetGearRollerSpeed(0);
					m_drivetrain.setDistancePIDSpeed(0.5);
				}
				break;
			case 12:
				if (autonDropOff() == true) {
					m_autonCase++;
					AutonOutput();
					m_drivetrain.zeroDriveEncoders();
					m_drivetrain.setDistancePIDSpeed(0.9);
					m_drivetrain.setAngleP(ANGLE_P);
				}
				break;
			case 13:
				if (m_autonShoot == true) {
					m_shoot.SetShootMode(true);
					m_shoot.RunShoot(3350);
				}
				if (autonTurnFinished(m_autonLineAngle) == true) {
					m_autonCase++;
					AutonOutput();
					m_drivetrain.zeroDriveEncoders();
				}
				break;
			case 14:
				if (autonDriveFinished(m_autonCrossLine, m_autonLineAngle) == true) {

					/*SmartDashboard::PutNumber("TUNE14_ANGLE", m_drivetrain.getYaw());
					SmartDashboard::PutNumber("TUNE14_ANGLE SETPOINT", m_drivetrain.GetAnglePIDSetpoint());
					SmartDashboard::PutNumber("TUNE14_ANGLE ERROR", m_drivetrain.GetAnglePIDError());*/

					m_autonCase++;
					AutonOutput();
					m_drivetrain.zeroDriveEncoders();

				}
				break;
			case 15:
				if (autonArmFinished(GEAR_PACKAGE) == true) {
					m_timer.Stop();
					m_timer.Reset();
					m_timer.Start();

					m_autonCase++;
					AutonOutput();
				}
				break;
			case 16:
				m_autonBoilerAngle = m_drivetrain.getYaw() + m_cameraHandler.GetBoilerAngle();

				if (m_timer.Get() < 0.3) {

					/*SmartDashboard::PutNumber("TUNE16_ANGLE", m_drivetrain.getYaw());
					SmartDashboard::PutNumber("TUNE16_ANGLE SETPOINT", m_drivetrain.GetAnglePIDSetpoint());
					SmartDashboard::PutNumber("TUNE16_ANGLE ERROR", m_drivetrain.GetAnglePIDError());
					SmartDashboard::PutNumber("TUNE16_ANGLE BOILER", m_autonBoilerAngle);
					SmartDashboard::PutNumber("TUNE16_BOILER X", m_cameraHandler.GetBoilerTargetNormalizedCenter());*/

					m_autonCase++;
					AutonOutput();
				}
				break;
			case 17:
				m_drivetrain.SetPIDSetpoint(m_drivetrain.GetAverageDistance(), m_autonBoilerAngle);

				/*SmartDashboard::PutNumber("TUNE17_ANGLE", m_drivetrain.getYaw());
				SmartDashboard::PutNumber("TUNE17_ANGLE SETPOINT", m_drivetrain.GetAnglePIDSetpoint());
				SmartDashboard::PutNumber("TUNE17_ANGLE ERROR", m_drivetrain.GetAnglePIDError());
				SmartDashboard::PutNumber("TUNE17_ANGLE BOILER", m_autonBoilerAngle);
				SmartDashboard::PutNumber("TUNE17_BOILER X", m_cameraHandler.GetBoilerTargetNormalizedCenter());*/

				if (fabs(m_drivetrain.GetAnglePIDError()) < 2.5  || (m_autonCaseTimeout.Get() > 12.0)) {
					m_drivetrain.zeroDriveEncoders();
					m_drivetrain.DisablePID();
					m_drivetrain.resetAnglePID();
					m_drivetrain.resetDistancePID();
					m_autonBoilerAngle = m_drivetrain.getYaw() + m_cameraHandler.GetBoilerAngle();
					m_autonCase++;
					AutonOutput();
				} else {
					m_drivetrain.EnablePID();
				}
				break;
			case 18:
				m_drivetrain.SetPIDSetpoint(m_drivetrain.GetAverageDistance(), m_autonBoilerAngle);

				/*SmartDashboard::PutNumber("TUNE18_ANGLE", m_drivetrain.getYaw());
				SmartDashboard::PutNumber("TUNE18_ANGLE SETPOINT", m_drivetrain.GetAnglePIDSetpoint());
				SmartDashboard::PutNumber("TUNE18_ANGLE ERROR", m_drivetrain.GetAnglePIDError());
				SmartDashboard::PutNumber("TUNE18_ANGLE BOILER", m_autonBoilerAngle);
				SmartDashboard::PutNumber("TUNE18_BOILERX", m_cameraHandler.GetBoilerTargetNormalizedCenter());*/

				if (fabs(m_drivetrain.GetAnglePIDError()) < 2.5  || (m_autonCaseTimeout.Get() > 12.0)) {
					m_drivetrain.zeroDriveEncoders();
					m_drivetrain.DisablePID();
					m_drivetrain.resetAnglePID();
					m_drivetrain.resetDistancePID();
					m_autonCase++;
					AutonOutput();
				} else {
					m_drivetrain.EnablePID();
				}
				break;
			case 19:
				if (m_autonShoot == true) {
					m_shoot.RunPaddle(-1.0);
				}
				m_autonCase++;
				AutonOutput();
			}
		}

	}

	bool autonDriveFinished(double distance, double angle) {
		m_drivetrain.SetPIDSetpoint(distance, angle);//m_drivetrain.getYaw());

		if (fabs(m_drivetrain.GetDistancePIDError()) < 4) {
			//m_drivetrain.zeroDriveEncoders();
			m_drivetrain.DisablePID();
			m_drivetrain.resetAnglePID();
			m_drivetrain.resetDistancePID();
			return true;
		}
		else {
			m_drivetrain.EnablePID();
			return false;
		}
	}

	bool autonTurnFinished(double angle) {
		m_drivetrain.SetPIDSetpoint(m_drivetrain.GetAverageDistance(), angle);

		if (fabs(m_drivetrain.GetAnglePIDError()) < 3) {
			m_drivetrain.zeroDriveEncoders();
			m_drivetrain.DisablePID();
			m_drivetrain.resetAnglePID();
			m_drivetrain.resetDistancePID();
			return true;
		} else {
			m_drivetrain.EnablePID();
			return false;
		}
	}

	/*bool autonBackUpFinished(double distance) {
		m_drivetrain.SetPIDSetpoint(distance, 0);

		if (fabs(m_drivetrain.GetDistancePIDError()) < 4) {
			m_drivetrain.DisablePID();
			m_drivetrain.zeroDriveEncoders();
			m_drivetrain.resetAnglePID();
			m_drivetrain.resetDistancePID();
			return true;
		} else {
			m_drivetrain.EnablePID();
			return false;
		}
	}*/

	bool autonTuneAngle() {
		m_drivetrain.SetPIDSetpoint(m_drivetrain.GetAverageDistance(), (m_cameraHandler.GetPegAngle() + m_drivetrain.getYaw())); // (:-20,:)

		if (fabs(m_drivetrain.GetAnglePIDError()) < 4) {
			m_drivetrain.zeroDriveEncoders();
			m_drivetrain.DisablePID();
			m_drivetrain.resetAnglePID();
			m_drivetrain.resetDistancePID();
			return true;
		} else {
			m_drivetrain.EnablePID();
			return false;
		}
	}

	bool autonArmFinished(double angle) {
		m_gear.SetGearMode(true);
		m_gear.SetGearArmPosition(angle);
		/*if (m_gear.GetGearError() < 5) {
			return true;
		}*/
		return true;
	}

	bool autonRollOutFinished() {
		m_gear.SetGearRollerSpeed(-0.7);
		if (m_rollTimer.Get() > 1.5) {
			std::cout << "Roller Stopped" << std::endl;
			//m_gear.SetGearRollerSpeed(0.0);
			//m_rollTimer.Stop();
			return true;
		}
		//m_gear.SetGearRollerSpeed(-0.7);
		return false;
	}

	bool autonDropOff() {
		m_drivetrain.SetPIDSetpoint(m_autonDropOffDistance, m_autonPegAngle);

		if (fabs(m_drivetrain.GetDistancePIDError()) < 6) {
			m_gear.SetGearRollerSpeed(0.0);
			m_drivetrain.DisablePID();
			m_drivetrain.zeroDriveEncoders();
			m_drivetrain.resetAnglePID();
			m_drivetrain.resetDistancePID();
			return true;
		} else {
			m_drivetrain.EnablePID();
			return false;
		}
	}

	bool autonPauseFinished(double length) {
		if (m_timer.Get() >= length) {
			return true;
		} else {
			return false;
		}
	}

	bool autonReleaseGear() {
		m_gear.openGearIntake(true);
		return true;
	}

	void TeleopInit() {
		m_drivetrain.DisablePID();
		m_drivetrain.zeroDriveEncoders();
		m_drivetrain.resetGyro();
		SmartDashboard::PutNumber("m_exposure", 80);
		SmartDashboard::PutNumber("m_brightness", 50);

		m_shoot.SetShootMode(true);
		m_shoot.RunShoot(0);

		m_shoot.RunPaddle(0.0);

		m_drivetrain.flashLightOn(true);
	}

	void TeleopPeriodic() {
		AutonOutput();
		/*
		SmartDashboard::PutBoolean("Drive PID Enabled", m_drivetrain.IsPIDEnabled());
		SmartDashboard::PutNumber("Drive Rotation Error", m_drivetrain.GetRotationPIDError());
		SmartDashboard::PutNumber("Drive Distance Error", m_drivetrain.GetDistancePIDError());
		SmartDashboard::PutNumber("Drive Angle Error", m_drivetrain.GetAnglePIDError());

		SmartDashboard::PutNumber("Drive Angle Setpoint", m_drivetrain.GetAnglePIDSetpoint());
		SmartDashboard::PutBoolean("Drive Angle PID Is Enabled", m_drivetrain.AnglePIDIsEnabled());

		SmartDashboard::PutNumber("Peg X", SmartDashboard::GetNumber("xPeg", 0));
		SmartDashboard::PutNumber("Boiler X", SmartDashboard::GetNumber("xBoiler", 0));
		SmartDashboard::PutNumber("Peg X 2", m_cameraHandler.GetTargetNormalizedCenter());
		SmartDashboard::PutNumber("Tune Angle", m_cameraHandler.GetAngle());
		SmartDashboard::PutNumber("Peg Distance", SmartDashboard::GetNumber("dPeg", 0.0));

		*/

		SmartDashboard::PutNumber("Angle", m_drivetrain.getYaw());
		TeleopDrive();
		TeleopShoot();
		TeleopGear();
		TeleopIntake();

		if (m_operator->ButtonBack()) {
			m_gear.GameStartGearArmPosition();
			m_shoot.ZeroShootEncoder();
			m_drivetrain.zeroDriveEncoders();
		}
	}

	void TeleopShoot() {
		if (m_driver->AxisLT() > 0.2) {
			m_shoot.RunPaddle(1.0);
		}
		else if (m_driver->AxisRT() > 0.2) {
			m_shoot.RunPaddle(-1.0);
		}
		else {
			m_shoot.RunPaddle(0.0);
		}

		if (m_operator->ButtonStart()) {
			m_shoot.SetShootMode(true);
			m_shoot.RunShoot(3300); //3200
		} else {
			m_shoot.SetShootMode(false);
			m_shoot.RunShoot(0.0);
		}

		/*SmartDashboard::PutNumber("Left Shooter Encoder", m_shoot.getLeftShoot());
		SmartDashboard::PutNumber("Right Shooter Encoder", m_shoot.getRightShoot());
		SmartDashboard::PutNumber("Average Shooter Speed", m_shoot.GetAverageShootSpeed());

		SmartDashboard::PutNumber("Right Shooter Raw", m_shoot.GetRRawShooter());
		SmartDashboard::PutNumber("Left Shooter Raw", m_shoot.GetLRawShooter());

		SmartDashboard::PutBoolean("Shooter Mode", m_shoot.GetShootMode());
		SmartDashboard::PutNumber("ShooterSpeed", shooterSpeed);
		SmartDashboard::PutNumber("Get Shoot", m_shoot.GetShoot());

		SmartDashboard::PutBoolean("Ready to Shoot", m_shoot.ReturnShooterAtSpeed());

		SmartDashboard::PutNumber("Get Paddle", m_shoot.GetPaddle());

		SmartDashboard::PutNumber("Shoot Error", m_shoot.GetShootError());


		*/

	}

	void TeleopDrive() {

		/*SmartDashboard::PutNumber("Angle", m_drivetrain.getYaw());
		SmartDashboard::PutNumber("Drivetrain Average Distance", m_drivetrain.GetAverageDistance());
		SmartDashboard::PutNumber("* Angle", m_drivetrain.GetGyroAngle());
		SmartDashboard::PutNumber("Axis RX", -m_driver->AxisRX());
		SmartDashboard::PutBoolean("Shift", m_drivetrain.getShift());
		SmartDashboard::PutNumber("Drivetrain Gyro Angle", m_drivetrain.getYaw());

		SmartDashboard::PutNumber("Drivetrain Distance to Setpoint", m_drivetrain.GetDistanceToSetpoint());
		SmartDashboard::PutNumber("Drivetrain Distance Setpoint", m_drivetrain.GetDistancePIDSetpoint());*/



		//SmartDashboard::PutNumber("Driver LY", m_driver->AxisLY());
		//SmartDashboard::PutNumber("Driver RX", m_driver->AxisRX());
		//SmartDashboard::PutNumber("Drive Left Front Talon Get", m_drivetrain.getLeftEncoder());
		//SmartDashboard::PutNumber("Drive Right Front Talon Get", m_drivetrain.getRightEncoder());

		if(fabs(m_driver->AxisLY()) > 0.2 && m_driver->ButtonB()) {
			m_drivetrain.ArcadeDrive(-m_driver->AxisLY() * 0.4, -m_driver->AxisRX() * 0.85);
		} else if (fabs(m_driver->AxisLY()) > 0.2 || fabs(m_driver->AxisRX()) > 0.2) {
			m_drivetrain.ArcadeDrive(-m_driver->AxisLY(), -m_driver->AxisRX() * 0.85);
		}

		if (m_driver->ButtonRB()) {
			m_drivetrain.setClimbShift(true);
		}
		else {
			m_drivetrain.setClimbShift(false);
		}

		if (m_driver->ButtonLB()) {
			m_drivetrain.setShift(true);
		} else {
			m_drivetrain.setShift(false);
		}

		//SmartDashboard::PutNumber("Forward and Backward", m_drivetrain.getSpeed());
		//SmartDashboard::PutNumber("Turning", m_drivetrain.getAngle());

		/*
		SmartDashboard::PutNumber("Drivetrain Left Drive Encoder", m_drivetrain.getLeftEncoder());
		SmartDashboard::PutNumber("Drivetrain Right Drive Encoder", m_drivetrain.getRightEncoder());

		SmartDashboard::PutNumber("Drivetrain Right Front Talon Get", m_drivetrain.getFrontRightTalon());
		SmartDashboard::PutNumber("Drivetrain Right Mini Talon Get", m_drivetrain.getMiniRightTalon());
		SmartDashboard::PutNumber("Drivetrain Right Rear Talon Get", m_drivetrain.getBackRightTalon());

		SmartDashboard::PutNumber("Drivetrain Left Front Talon Get", m_drivetrain.getFrontLeftTalon());
		SmartDashboard::PutNumber("Drivetrain Left Mini Talon Get", m_drivetrain.getMiniLeftTalon());
		SmartDashboard::PutNumber("Drivetrain Left Rear Talon Get", m_drivetrain.getBackLeftTalon());
		*/

		/*if (totalDriveCurrent >= 2.5) {
			m_currentTimer.Start();
		} else {
			m_currentTimer.Stop();
			m_currentTimer.Reset();
		}*/

		/*SmartDashboard::PutNumber("Left Drive Current - Front", m_pdp.GetCurrent(7));
		SmartDashboard::PutNumber("Left Drive Current - Mini", m_pdp.GetCurrent(5));
		SmartDashboard::PutNumber("Left Drive Current - Rear", m_pdp.GetCurrent(6));
		SmartDashboard::PutNumber("Right Drive Current - Front", m_pdp.GetCurrent(2));
		SmartDashboard::PutNumber("Right Drive Current - Mini", m_pdp.GetCurrent(4));
		SmartDashboard::PutNumber("Right Drive Current - Rear", m_pdp.GetCurrent(3));*/
		//SmartDashboard::PutNumber("Drivetrain Total Current", m_drivetrain.getTotalDriveCurrent());
	}

	//intake is ready for test
	void TeleopIntake() {
		if (m_operator->GetPOV() == 0.0) {
			//if (m_gear.GetGearArmPosition() > 75) { //checks to see if gear arm is in
				m_intake.SetHopper(true);
				//m_intake.SetIntakeArm(true);
			//}
		}
		else if (m_operator->GetPOV() == 180.0) {
			//m_intake.SetIntakeArm(false);
			m_intake.SetHopper(false);
		}
		/*

		if (m_operator->AxisLT() > 0.2) {
			m_intake.SetIntakeRoller(1.0);
		}
		else if (m_operator->AxisRT() > 0.2) {
			m_intake.SetIntakeRoller(-1.0);
		}
		else {
			m_intake.SetIntakeRoller(0.0);
		}

		*/
		//SmartDashboard::PutNumber("Intake Roller", m_intake.GetIntakeRoller());
		//SmartDashboard::PutNumber("Intake POV", m_operator->GetPOV());
		//SmartDashboard::PutBoolean("Hopper", m_intake.GetHopper());
	}

	//gear is done !!
	void TeleopGear() {
		/*SmartDashboard::PutNumber("Gear Position", m_gear.GetGearArmPosition());
		SmartDashboard::PutNumber("Gear Commanded", m_gear.GetGearCommandedSpeed());*/

		/*
		SmartDashboard::PutNumber("Gear Timer", m_rollTimer.Get());
		SmartDashboard::PutBoolean("Gear Mode", m_gear.GetGearMode());
		SmartDashboard::PutNumber("Gear Raw Position", m_gear.GetRawGearArmPosition());
		SmartDashboard::PutNumber("Gear Error", m_gear.GetGearError());
		SmartDashboard::PutNumber("Gear Place", placeGear);
		SmartDashboard::PutNumber("Gear Roller Speed", m_gear.GetGearRollerCommandedSpeed());
		*/
		if (fabs(m_operator->AxisLY()) > 0.2) {
			m_gear.SetGearMode(false);
			m_gear.SetGearArmPosition(m_operator->AxisLY());
		}
		else if (m_operator->ButtonA()) {
			m_gear.SetGearMode(true);
			m_gear.SetGearArmPosition(GEAR_GROUND);
		}
		else if (m_operator->ButtonB()){
			switch (placeGear){
				case 0:
					m_gear.SetGearMode(true);
					m_gear.SetGearArmPosition(GEAR_PLACE_FIRST);
					if (m_gear.GetGearError() < 5) {
						placeGear++;
						m_rollTimer.Stop();
						m_rollTimer.Reset();
						m_rollTimer.Start();
					}
					break;
				case 1:
					//m_gear.SetGearRollerSpeed(-0.7);
					if (m_rollTimer.Get() > 3.0) {
						//m_gear.SetGearRollerSpeed(0.0);
						placeGear++;
					}
					break;
				case 2:
					m_gear.SetGearArmPosition(GEAR_PLACE_SECOND);
					if (m_gear.GetGearError() < 5) {
						placeGear++;
					}
			}

		}
		else if (m_operator->ButtonX()) {
			m_gear.SetGearMode(true);
			m_gear.SetGearArmPosition(GEAR_PLACE_FIRST);
		}
		else if (m_operator->ButtonY()) {
			m_gear.SetGearMode(true);
			m_gear.SetGearArmPosition(GEAR_PACKAGE);
		}else {
			m_gear.SetGearMode(false);
			m_gear.SetGearArmPosition(0.0);
			//m_gear.SetGearRollerSpeed(0.0);
			placeGear = 0;
		}

		if (m_operator->ButtonRB()) {
			m_gear.SetGearRollerSpeed(1.0);
		}
		else if (m_operator->ButtonLB()) {
			m_gear.SetGearRollerSpeed(-1.0);
		}
		else {
			m_gear.SetGearRollerSpeed(0.0);
		}

		if (m_operator->GetPOV() == 90.0) {
			m_gear.openGearIntake(true);
		} else {
			m_gear.openGearIntake(false);
		}

	}

	void TestPeriodic() {
	}

};

START_ROBOT_CLASS(barrybbenson)
