#include "Character.h"
#include "Renderer/Camera/Camera.h"
#include <iostream>

Character::Character(_In_ const std::filesystem::path& filePath) 
	: Model(filePath)
	, m_currentPosition(Camera::DEFAULT_TARGET)
	, m_currentRotation(DOWN)
	, m_moveBackForward(0.0f)
	, m_moveLeftRight(0.0f)
	, m_movementSpeed(10.0f)
{
}

void Character::HandleInput(const InputDirections& directions, FLOAT deltaTime) {
	if (directions.bFront) {
		RotateYInObjectCoordinate(XMConvertToRadians(90 * static_cast<CharacterDirection>(int(m_currentRotation) - UP)), m_currentPosition);
		m_moveBackForward = m_movementSpeed * deltaTime;
		m_currentRotation = UP;
	}
	if (directions.bRight) {
		RotateYInObjectCoordinate(XMConvertToRadians(90 * static_cast<CharacterDirection>(int(m_currentRotation) - RIGHT)), m_currentPosition);
		m_moveLeftRight = m_movementSpeed * deltaTime;
		m_currentRotation = RIGHT;
	}
	if (directions.bBack) {
		RotateYInObjectCoordinate(XMConvertToRadians(90 * static_cast<CharacterDirection>(int(m_currentRotation) - DOWN)), m_currentPosition);
		m_moveBackForward = -1 * m_movementSpeed * deltaTime;
		m_currentRotation = DOWN;
	}
	if (directions.bLeft) {
		RotateYInObjectCoordinate(XMConvertToRadians(90 * static_cast<CharacterDirection>(int(m_currentRotation) - LEFT)), m_currentPosition);
		m_moveLeftRight = -1 * m_movementSpeed * deltaTime;
		m_currentRotation = LEFT;
	}
	//bMoving = true;
}

void Character::Update(FLOAT deltaTime) {
	
	Model::Update(deltaTime);


	m_targetPosition = m_currentPosition + (DEFAULT_FORWARD * m_moveBackForward) + (DEFAULT_RIGHT * m_moveLeftRight);
	Translate(m_targetPosition - m_currentPosition);
	m_currentPosition = m_targetPosition;
	m_moveBackForward = 0.0f;
	m_moveLeftRight = 0.0f;

}

XMVECTOR Character::GetCurrentPosition() {
	return m_currentPosition;
}
