#pragma once
#include "Model.h"

class Character : public Model {
private:
	static constexpr XMVECTORF32 DEFAULT_FORWARD = { 0.0f, 0.0f, 1.0f, 0.0f };
	static constexpr XMVECTORF32 DEFAULT_RIGHT = { 1.0f, 0.0f, 0.0f, 0.0f };

	XMVECTOR m_targetPosition;
	XMVECTOR m_currentPosition;
	FLOAT m_moveLeftRight;
	FLOAT m_moveBackForward;

	enum CharacterDirection { DOWN=0, RIGHT=1, UP=2, LEFT=3};

	CharacterDirection m_currentRotation;
	float m_movementSpeed;

public:
	Character() = default;
	Character(const std::filesystem::path& filepath);
	~Character() = default;

	void HandleInput(const InputDirections& directions, FLOAT deltaTime);
	virtual void Update(FLOAT deltaTime) override;
	XMVECTOR GetCurrentPosition();
};