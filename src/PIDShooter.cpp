/*
 * PIDShooter.cpp
 *
 *  Created on: Feb 4, 2017
 *      Author: ROBO-PGM06
 */

#include "PIDShooter.h"

PIDShooter::PIDShooter( double p, double i, double d ) : p(p), i(i), d(d){
	// TODO Auto-generated constructor stub
	m_shooterL = new CANTalon(10);
	m_shooterR = new CANTalon(11);
	m_feeder = new CANTalon(9);
	m_shooterL->SetFeedbackDevice( CANTalon::CtreMagEncoder_Relative );

	m_shooterL->SetControlMode(CANTalon::kSpeed);
	//m_shooterL->SetPID( SHOOTER_P, SHOOTER_I, SHOOTER_D );
	m_shooterL->SetP(SHOOTER_P);
	m_shooterL->SetI(SHOOTER_I);
	m_shooterL->SetD(SHOOTER_D);
	m_shooterL->SetF(SHOOTER_F);
	m_shooterL->Enable();
	m_shooterL->SetAllowableClosedLoopErr(0);
	m_shooterL->SetSensorDirection(true);
	m_shooterL->SetClosedLoopOutputDirection(false);
	m_shooterL->ConfigNominalOutputVoltage(+0.0F, -0.0F);
	m_shooterL->ConfigPeakOutputVoltage(+12.0F, -12.0F);
	m_shooterL->SetCloseLoopRampRate(65);

	m_shooterR->SetFeedbackDevice( CANTalon::CtreMagEncoder_Relative );

	m_shooterR->SetControlMode( CANTalon::kFollower );
	m_shooterR->Set(10);
	m_shooterR->SetTalonControlMode( CANTalon::kFollowerMode );
	//m_shooterR->SetPID( SHOOTER_P, SHOOTER_I, SHOOTER_D );

	m_shooterR->SetClosedLoopOutputDirection(true);


	m_shooterTime = new Timer();

	m_pdp = new PowerDistributionPanel( 0 );

	m_dashboard = new SmartDashboard;
}

PIDShooter::~PIDShooter() {
	// TODO Auto-generated destructor stub
}

void PIDShooter::SetDevice()
{

}

void PIDShooter::Enable()
{
	m_shooterL->Enable();
	//m_shooterR->Enable();

	m_feeder->Set(-1);
}

void PIDShooter::DisableShooter()
{
	m_shooterL->SetSetpoint(0);
	m_shooterL->Reset();

	m_shooterR->SetSetpoint(0);
	m_shooterR->Reset();

	m_feeder->Set(0);
}

void PIDShooter::SetSetpoint( double setpoint)
{
	m_shooterL->SetSetpoint( setpoint );

	shooterSetpoint = setpoint;
}

double PIDShooter::GetRate()
{
	return m_shooterL->GetSpeed();
}

double PIDShooter::GetSetpoint()
{
	return shooterSetpoint;
}

double PIDShooter::GetShooterP()
{
	return p;
}

double PIDShooter::GetShooterI()
{
	return i;
}

double PIDShooter::GetShooterD()
{
	return d;
}

double PIDShooter::GetShooterF()
{
	return m_shooterL->GetF();
}

double PIDShooter::GetOutputCurrent()
{
	return m_shooterL->GetOutputCurrent();
}

double PIDShooter::GetPosition()
{
	return m_shooterL->GetPosition();
}

double PIDShooter::GetSpeed()
{
	return m_shooterL->GetSpeed();
}

double PIDShooter::GetError()
{
	return m_shooterL->GetClosedLoopError();
}

void PIDShooter::OutputValues( SmartDashboard * dashboard )
{
	dashboard->PutNumber( "L Shooter position", m_shooterL->GetPosition() );
	dashboard->PutNumber( "R Shooter position", m_shooterR->GetPosition() );
	dashboard->PutNumber( "L Shooter speed", m_shooterL->GetSpeed() );
	dashboard->PutNumber( "R Shooter speed", m_shooterR->GetSpeed() );
	dashboard->PutNumber( "L Shooter error", m_shooterL->GetClosedLoopError() );
	dashboard->PutNumber( "R Shooter error", m_shooterR->GetClosedLoopError() );
	dashboard->PutNumber( "L Shooter P", p );
	//dashboard->PutNumber( "R Shooter P", m_shooterR->GetP() );
	dashboard->PutNumber( "L Shooter I", i );
	//dashboard->PutNumber( "R Shooter I", m_shooterR->GetI() );
	dashboard->PutNumber( "L Shooter D", d );
	//dashboard->PutNumber( "R Shooter D", m_shooterR->GetD() );
	dashboard->PutNumber( "L Shooter F", m_shooterL->GetF() );
	dashboard->PutNumber( "L Shooter current", m_shooterL->GetOutputCurrent() );
	dashboard->PutNumber( "R Shooter current", m_shooterR->GetOutputCurrent() );
}